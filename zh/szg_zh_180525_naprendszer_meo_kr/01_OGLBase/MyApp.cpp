#include "MyApp.h"

#include <math.h>
#include <vector>

#include <imgui/imgui.h>

CMyApp::CMyApp(void)
{
	m_camera.SetView(glm::vec3(5, 5, 5), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
}


CMyApp::~CMyApp(void)
{
	std::cout << "dtor!\n";
}

bool CMyApp::Init()
{
	// törlési szín legyen kékes
	glClearColor(0.125f, 0.25f, 0.5f, 1.0f);

	glEnable(GL_CULL_FACE); // kapcsoljuk be a hatrafele nezo lapok eldobasat
	glEnable(GL_DEPTH_TEST); // mélységi teszt bekapcsolása (takarás)

	//
	// shaderek betöltése
	//

	// a shadereket tároló program létrehozása az OpenGL-hez hasonló módon:
	m_program.AttachShaders({
		{ GL_VERTEX_SHADER, "myVert.vert"},
		{ GL_FRAGMENT_SHADER, "myFrag.frag"}
	});

	// attributomok osszerendelese a VAO es shader kozt
	m_program.BindAttribLocations({
		{ 0, "vs_in_pos" },				// VAO 0-as csatorna menjen a vs_in_pos-ba
		{ 1, "vs_in_norm" },				// VAO 1-es csatorna menjen a vs_in_col-ba
		{ 2, "vs_in_tex" },				// VAO 1-es csatorna menjen a vs_in_col-ba
	});

	m_program.LinkProgram();

	// shader program rövid létrehozása, egyetlen függvényhívással a fenti három:
	m_programSkybox.Init(
	{
		{ GL_VERTEX_SHADER, "skybox.vert" },
		{ GL_FRAGMENT_SHADER, "skybox.frag" }
	},
	{
		{ 0, "vs_in_pos" },				// VAO 0-as csatorna menjen a vs_in_pos-ba
	}
	);

	CreateGeometry();

	Register("shell");
	Register("earth");
	Register("leg");
	Register("box");

	// skybox
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	glGenTextures(1, &m_skyboxTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_skyboxTexture);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	TextureFromFileAttach("xpos.png", GL_TEXTURE_CUBE_MAP_POSITIVE_X);
	TextureFromFileAttach("xneg.png", GL_TEXTURE_CUBE_MAP_NEGATIVE_X);
	TextureFromFileAttach("ypos.png", GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
	TextureFromFileAttach("yneg.png", GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);
	TextureFromFileAttach("zpos.png", GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
	TextureFromFileAttach("zneg.png", GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	// kamera
	m_camera.SetProj(45.0f, 640.0f / 480.0f, 0.01f, 1000.0f);

	return true;
}

void CMyApp::TextureFromFileAttach(const char* filename, GLuint role) const
{
	SDL_Surface* loaded_img = IMG_Load(filename);

	int img_mode = 0;

	if (loaded_img == 0)
	{
		std::cout << "[TextureFromFile] Hiba a kép betöltése közben: " << filename << std::endl;
		return;
	}

#if SDL_BYTEORDER == SDL_LIL_ENDIAN
	if (loaded_img->format->BytesPerPixel == 4)
		img_mode = GL_BGRA;
	else
		img_mode = GL_BGR;
#else
	if (loaded_img->format->BytesPerPixel == 4)
		img_mode = GL_RGBA;
	else
		img_mode = GL_RGB;
#endif

	glTexImage2D(role, 0, GL_RGBA, loaded_img->w, loaded_img->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, loaded_img->pixels);

	SDL_FreeSurface(loaded_img);
}

void CMyApp::Clean()
{
	glDeleteTextures(1, &m_skyboxTexture);
}

glm::vec3	CMyApp::GetUVHalfSphere(float u, float v)
{
	u *= 2*3.1415f;
	v *= 3.1415f/2;
	float cu = cosf(u), su = sinf(u), cv = cosf(v), sv = sinf(v);
	float r = 2;

	return glm::vec3(r*cu*sv, r*cv, r*su*sv);
}

glm::vec3	CMyApp::GetUVCylinder(float u, float v)
{
	u *= 1;
	v *= 2 * 3.1415f;
	float cv = cosf(v), sv = sinf(v);
	float r = 1.0;

	return glm::vec3(r*cv, u, r*sv);
}

std::vector<decltype(CMyApp::dt)::type> CMyApp::Triangulate(size_t N, size_t M)
{
	std::vector<decltype(dt)::type> indices(3 * 2 * (N)*(M), 0);
	for (int i = 0; i < N; ++i)
		for (int j = 0; j < M; ++j)
		{
			/*minden négyszögre csináljunk kettõ háromszöget, amelyek a következõ
			(i,j) indexeknél született (u_i, v_i) paraméterértékekhez tartozó
			pontokat kötik össze:

			(i,j+1)
			o-----o(i+1,j+1)
			|\    |			a = p(u_i, v_i)
			| \   |			b = p(u_{i+1}, v_i)
			|  \  |			c = p(u_i, v_{i+1})
			|   \ |			d = p(u_{i+1}, v_{i+1})
			|	   \|
			(i,j) o-----o(i+1, j)

			- az (i,j)-hez tartózó 1D-s index a VBO-ban: i+j*(N+1)
			- az (i,j)-hez tartózó 1D-s index az IB-ben: i*6+j*6*(N+1)
			(mert minden négyszöghöz 2db háromszög = 6 index tartozik)
			*/
			indices[6 * i + j * 3 * 2 * (N)+0] = (i)+(j)*	(N + 1);
			indices[6 * i + j * 3 * 2 * (N)+1] = (i + 1) + (j)*	(N + 1);
			indices[6 * i + j * 3 * 2 * (N)+2] = (i)+(j + 1)*(N + 1);
			indices[6 * i + j * 3 * 2 * (N)+3] = (i + 1) + (j)*	(N + 1);
			indices[6 * i + j * 3 * 2 * (N)+4] = (i + 1) + (j + 1)*(N + 1);
			indices[6 * i + j * 3 * 2 * (N)+5] = (i)+(j + 1)*(N + 1);
		}
	return indices;
}


void CMyApp::Register(std::string object)
{	
	// geometria VAO-ban való regisztrálása
	objects[object].m_vao.Init(
		{
			// 0-ás attribútum "lényegében" glm::vec3-ak sorozata és az adatok az m_gpuBufferPos GPU pufferben vannak
			{ CreateAttribute<		0,						// csatorna: 0
									glm::vec3,				// CPU oldali adattípus amit a 0-ás csatorna attribútumainak meghatározására használtunk <- az eljárás a glm::vec3-ból kikövetkezteti, hogy 3 darab float-ból áll a 0-ás attribútum
									0,						// offset: az attribútum tároló elejétõl vett offset-je, byte-ban
									sizeof(Vertex)		// stride: a következõ csúcspont ezen attribútuma hány byte-ra van az aktuálistól
								>, objects[object].m_gpuBuffer },
			{ CreateAttribute<		1,						// csatorna: 0
									glm::vec3,				// CPU oldali adattípus amit a 0-ás csatorna attribútumainak meghatározására használtunk <- az eljárás a glm::vec3-ból kikövetkezteti, hogy 3 darab float-ból áll a 0-ás attribútum
									sizeof(glm::vec3),				// offset: az attribútum tároló elejétõl vett offset-je, byte-ban
									sizeof(Vertex)		// stride: a következõ csúcspont ezen attribútuma hány byte-ra van az aktuálistól
								>, objects[object].m_gpuBuffer },
			{ CreateAttribute<		2,						// csatorna: 0
									glm::vec2,				// CPU oldali adattípus amit a 0-ás csatorna attribútumainak meghatározására használtunk <- az eljárás a glm::vec3-ból kikövetkezteti, hogy 3 darab float-ból áll a 0-ás attribútum
									2 * sizeof(glm::vec3),	// offset: az attribútum tároló elejétõl vett offset-je, byte-ban
									sizeof(Vertex)		// stride: a következõ csúcspont ezen attribútuma hány byte-ra van az aktuálistól
								>, objects[object].m_gpuBuffer },
		},
		objects[object].m_gpuBufferIndices
	);

}

void CMyApp::CreateGeometry()
{
	// NxM darab négyszöggel közelítjük a parametrikus felületünket => (N+1)x(M+1) pontban kell kiértékelni
	std::map<Type, std::pair<std::vector<Vertex>, std::vector<decltype(dt)::type>>> vert = {
		{ Type::HalfSphere, {{(N + 1)*(M + 1) , Vertex()}, {}} },
		{ Type::Cylinder, {{(N + 1)*(M + 1) , Vertex()}, {}} },
		{ Type::Circle, {{(N + 1) , Vertex()}, {}} },
	};

	vert[Type::Circle].first[0].pos = glm::vec3{ 0,0,0 };
	vert[Type::Circle].first[0].norm = glm::vec3{ 0,1,0 };
	vert[Type::Circle].first[0].uv = glm::vec2(0.5, 0.5);

	for (int i = 0; i < N; ++i)
	{
		float t = i / (float)N * 2 * glm::pi<float>();
		vert[Type::Circle].first[i + 1].pos = glm::vec3{ cos(t), 0, sin(t) };
		vert[Type::Circle].first[i + 1].norm = glm::normalize(vert[Type::Circle].first[i + 1].pos);
		vert[Type::Circle].first[i + 1].uv = glm::vec2{ 0.5+cos(t)/2, 0.5+sin(t)/2 };
		vert[Type::Circle].second.push_back(0);
		vert[Type::Circle].second.push_back((i + 1) % N + 1);
		vert[Type::Circle].second.push_back((i) % N + 1);
	}


	for (int i = 0; i <= N; ++i)
		for (int j = 0; j <= M; ++j)
		{
			float u = i / (float)N;
			float v = j / (float)M;

			vert[Type::HalfSphere].first[i + j * (N + 1)].pos = GetUVHalfSphere(u, v);
			vert[Type::HalfSphere].first[i + j * (N + 1)].norm = glm::normalize(vert[Type::HalfSphere].first[i + j * (N + 1)].pos);
			vert[Type::HalfSphere].first[i + j * (N + 1)].uv = glm::vec2(u, v);

			vert[Type::Cylinder].first[i + j * (N + 1)].pos = GetUVCylinder(u, v);
			vert[Type::Cylinder].first[i + j * (N + 1)].norm = glm::normalize(
				glm::vec3{ vert[Type::Cylinder].first[i + j * (N + 1)].pos.x,0,vert[Type::Cylinder].first[i + j * (N + 1)].pos.z });
			vert[Type::Cylinder].first[i + j * (N + 1)].uv = glm::vec2(u, v);
		}

	// indexpuffer adatai: NxM négyszög = 2xNxM háromszög = háromszöglista esetén 3x2xNxM index
	vert[Type::HalfSphere].second = Triangulate(N, M);
	vert[Type::Cylinder].second = Triangulate(N, M);

	/* vertexek pozíciói:
	Az m_gpuBufferPos konstruktora már létrehozott egy GPU puffer azonosítót és a most következõ BufferData hívás ezt
	1. bind-olni fogja GL_ARRAY_BUFFER target-re (hiszen m_gpuBufferPos típusa ArrayBuffer) és
	2. glBufferData segítségével áttölti a GPU-ra az argumentumban adott tároló értékeits
	*/

	//shell
	std::vector<Vertex> aggrbuffer = vert[Type::Circle].first;
	std::transform(aggrbuffer.begin(), aggrbuffer.end(), aggrbuffer.begin(),
		[](Vertex &v) -> Vertex { v.pos *= 2;  return v; });

	std::vector<decltype(dt)::type> aggrindices = {};
	for (size_t i = 0; i < vert[Type::Circle].second.size(); i += 3)
	{
		aggrindices.push_back(vert[Type::Circle].second[i]);
		aggrindices.push_back(vert[Type::Circle].second[i + 2]);
		aggrindices.push_back(vert[Type::Circle].second[i + 1]);
	}
	size_t end = aggrbuffer.size();
	
	aggrbuffer.insert(aggrbuffer.end(), vert[Type::HalfSphere].first.begin(), vert[Type::HalfSphere].first.end());
	for (auto idx : vert[Type::HalfSphere].second)
	{
		aggrindices.push_back(end + idx);
	}

	objects["shell"].m_gpuBuffer.BufferData(aggrbuffer);
	objects["shell"].idxNum = aggrindices.size();
	objects["shell"].m_gpuBufferIndices.BufferData(aggrindices);
	objects["shell"].m_texture = TextureObject<>("shell.jpg");

//earth
	aggrbuffer = vert[Type::Circle].first;
	std::transform(aggrbuffer.begin(), aggrbuffer.end(), aggrbuffer.begin(),
		[](Vertex &v) -> Vertex { v.pos *= 2;  return v; });
	aggrindices = vert[Type::Circle].second; 
	for (size_t i = 0; i < vert[Type::Circle].second.size(); i+=3)
	{
		aggrindices.push_back(vert[Type::Circle].second[i]);
		aggrindices.push_back(vert[Type::Circle].second[i + 2]);
		aggrindices.push_back(vert[Type::Circle].second[i + 1]);
	}

	objects["earth"].m_gpuBuffer.BufferData(aggrbuffer);
	objects["earth"].idxNum = aggrindices.size();
	objects["earth"].m_gpuBufferIndices.BufferData(aggrindices);
	objects["earth"].m_texture = TextureObject<>("earth.png");

//leg
	aggrbuffer = vert[Type::Circle].first;
	std::transform(aggrbuffer.begin(), aggrbuffer.end(), aggrbuffer.begin(),
		[](Vertex &v) -> Vertex { v.pos.y += 1; return v; });
	aggrindices = vert[Type::Circle].second;
	end = aggrbuffer.size();

	aggrbuffer.insert(aggrbuffer.end(), vert[Type::Cylinder].first.begin(), vert[Type::Cylinder].first.end());
	for (auto idx : vert[Type::Cylinder].second)
	{
		aggrindices.push_back(end + idx);
	}

	end = aggrbuffer.size();
	aggrbuffer.insert(aggrbuffer.end(), vert[Type::Circle].first.begin(), vert[Type::Circle].first.end());
	for (size_t i = 0; i < vert[Type::Circle].second.size(); i += 3)
	{
		aggrindices.push_back(end + vert[Type::Circle].second[i]);
		aggrindices.push_back(end + vert[Type::Circle].second[i + 2]);
		aggrindices.push_back(end + vert[Type::Circle].second[i + 1]);
	}

	objects["leg"].m_gpuBuffer.BufferData(aggrbuffer);
	objects["leg"].idxNum = aggrindices.size();
	objects["leg"].m_gpuBufferIndices.BufferData(aggrindices);
	objects["leg"].m_texture = TextureObject<>("scales.jpg");

//skybox
	objects["box"].m_gpuBuffer.BufferData(
		std::vector<Vertex>{
		// hátsó lap
			{glm::vec3(-1, -1, -1), glm::normalize(glm::vec3(-1, -1, -1)), glm::vec2(0, 0)},
			{glm::vec3(1, -1, -1), glm::normalize(glm::vec3(1, -1, -1)), glm::vec2(0, 0) },
			{glm::vec3(1, 1, -1), glm::normalize(glm::vec3(1, 1, -1)), glm::vec2(0, 0) },
			{glm::vec3(-1, 1, -1), glm::normalize(glm::vec3(-1, 1, -1)), glm::vec2(0, 0) },
			// elülsõ lap
			{glm::vec3(-1, -1, 1), glm::normalize(glm::vec3(-1, -1, 1)), glm::vec2(0, 0) },
			{glm::vec3(1, -1, 1), glm::normalize(glm::vec3(1, -1, 1)), glm::vec2(0, 0) },
			{glm::vec3(1, 1, 1), glm::normalize(glm::vec3(1, 1, 1)), glm::vec2(0, 0) },
			{glm::vec3(-1, 1, 1), glm::normalize(glm::vec3(-1, 1, 1)), glm::vec2(0, 0) },

	}
	);

	// és a primitíveket alkotó csúcspontok indexei (az elõzõ tömbökbõl) - triangle list-el való kirajzolásra felkészülve
	objects["box"].m_gpuBufferIndices.BufferData(
		std::vector<decltype(dt)::type>{
			// hátsó lap
			0, 1, 2,
			2, 3, 0,
			// elülsõ lap
			4, 6, 5,
			6, 4, 7,
			// bal
			0, 3, 4,
			4, 3, 7,
			// jobb
			1, 5, 2,
			5, 6, 2,
			// alsó
			1, 0, 4,
			1, 4, 5,
			// felsõ
			3, 2, 6,
			3, 6, 7,
	}
	);
	objects["box"].idxNum = 36;


	// skybox
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	glGenTextures(1, &m_skyboxTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_skyboxTexture);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	TextureFromFileAttach("xpos.png", GL_TEXTURE_CUBE_MAP_POSITIVE_X);
	TextureFromFileAttach("xneg.png", GL_TEXTURE_CUBE_MAP_NEGATIVE_X);
	TextureFromFileAttach("ypos.png", GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
	TextureFromFileAttach("yneg.png", GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);
	TextureFromFileAttach("zpos.png", GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
	TextureFromFileAttach("zneg.png", GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void CMyApp::Update()
{
	static Uint32 last_time = SDL_GetTicks();
	float delta_time = (SDL_GetTicks() - last_time) / 1000.0f;

	m_camera.Update(delta_time);

	last_time = SDL_GetTicks();
}

void CMyApp::Render()
{
	// töröljük a frampuffert (GL_COLOR_BUFFER_BIT) és a mélységi Z puffert (GL_DEPTH_BUFFER_BIT)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	float t = SDL_GetTicks() / 1000.f / 4;

	m_program.Use();

	glm::mat4 world = glm::scale(glm::vec3(1, 1, 1));
	objects["shell"].draw(m_program, m_camera.GetViewProj(), world, dt.flag);
	
	world = glm::translate(glm::vec3(0, 2, 0));
	objects["earth"].draw(m_program, m_camera.GetViewProj(), world, dt.flag);

	world = glm::scale(glm::vec3(0.25, 1, 0.25));

	float swing = glm::radians(30*sin(16*t));
	for(size_t i = 0; i < 4; ++i)
	objects["leg"].draw(m_program, m_camera.GetViewProj(),
		glm::translate(glm::vec3(1.5*cos(glm::radians(45.0) + 2 * glm::pi<float>() / 4.0*i), 0, 1.5*sin(glm::radians(45.0) + 2 * glm::pi<float>() / 4.0*i)))*
		glm::rotate(swing, glm::vec3(0, 0, 1)) *
		glm::translate(glm::vec3(0,-1,0))*
		world, dt.flag);


	world = glm::translate(glm::vec3(0, 0, 1)) *
		glm::rotate(2*glm::pi<float>()*t, glm::vec3(0, 0, 1))*
		glm::rotate(glm::pi<float>() / 2, glm::vec3(1, 0, 0)) *
		glm::scale(glm::vec3(0.5, 2, 0.5));
	objects["leg"].draw(m_program, m_camera.GetViewProj(), world, dt.flag);

	world = glm::scale(glm::vec3(0.25, 0.25, 2)) *
		glm::translate(glm::vec3(0, 0, -1.5)) *
		glm::rotate(glm::pi<float>() / 2, glm::vec3(1, 0, 0));
	objects["leg"].draw(m_program, m_camera.GetViewProj(), world, dt.flag);

	// skybox

	// mentsük el az elõzõ Z-test eredményt, azaz azt a relációt, ami alapján update-eljük a pixelt.
	GLint prevDepthFnc;
	glGetIntegerv(GL_DEPTH_FUNC, &prevDepthFnc);

	// most kisebb-egyenlõt használjunk, mert mindent kitolunk a távoli vágósíkokra
	glDepthFunc(GL_LEQUAL);

	objects["box"].m_vao.Bind();

	m_programSkybox.Use();
	m_programSkybox.SetUniform("MVP", m_camera.GetViewProj() * glm::translate(m_camera.GetEye()));
	m_programSkybox.SetCubeTexture("skyboxTexture", 0, m_skyboxTexture);
	glDrawElements(GL_TRIANGLES, objects["box"].idxNum, dt.flag, nullptr);

	// végül állítsuk vissza
	glDepthFunc(prevDepthFnc);
	// végül állítsuk vissza
	glDepthFunc(prevDepthFnc);
}

void CMyApp::KeyboardDown(SDL_KeyboardEvent& key)
{
	m_camera.KeyboardDown(key);
}

void CMyApp::KeyboardUp(SDL_KeyboardEvent& key)
{
	m_camera.KeyboardUp(key);
}

void CMyApp::MouseMove(SDL_MouseMotionEvent& mouse)
{
	m_camera.MouseMove(mouse);
}

void CMyApp::MouseDown(SDL_MouseButtonEvent& mouse)
{
}

void CMyApp::MouseUp(SDL_MouseButtonEvent& mouse)
{
}

void CMyApp::MouseWheel(SDL_MouseWheelEvent& wheel)
{
}

// a két paraméterbe az új ablakméret szélessége (_w) és magassága (_h) található
void CMyApp::Resize(int _w, int _h)
{
	glViewport(0, 0, _w, _h );

	m_camera.Resize(_w, _h);
}

void CMyApp::obj::draw(ProgramObject &program, glm::mat4 &proj, const glm::mat4 &world, int flag)
{
	m_vao.Bind();

	size_t indexNum = idxNum;

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glUniform1i(program.GetLocation("tex"), 0);

	program.SetUniform("MVP", proj*world);
	glDrawElements(GL_TRIANGLES, indexNum, flag, nullptr);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

}
