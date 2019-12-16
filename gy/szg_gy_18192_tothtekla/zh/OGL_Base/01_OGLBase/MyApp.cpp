#include "MyApp.h"

#include <math.h>
#include <vector>

#include <array>
#include <list>
#include <tuple>
#include <imgui/imgui.h>

#include "SDL_Utils.h"

CMyApp::CMyApp(void)
{
	m_camera.SetView(glm::vec3(5, 5, 5), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
}


CMyApp::~CMyApp(void)
{
	std::cout << "dtor!\n";
	delete m_suzanne;
}

// A parametrikus egyenletben néha az y és z koordinátát fordítva adják meg, mit ahogyan mi használjuk

const float pi = glm::pi<float>();

// http://mathworld.wolfram.com/Cylinder.html
glm::vec3 p_cylinder(glm::vec2 uv, float r = 1.f) {
	return{
		r*cosf(uv.x*2.f*pi),
		uv.y*2.f - 1.f,			 // Eltoljuk negatív irányba, hogy az origó legyen a középpontja függõlegesen is
		-r * sinf(uv.x*2.f*pi)  // Negatív elõjellel vettük a Z-t, hogy jó legyen a körüljárási irány
	};
}

// http://mathworld.wolfram.com/Sphere.html
glm::vec3 p_sphere(glm::vec2 uv, float r = 1.f) {
	return{
		r*cosf(uv.x*2.f*pi)*sinf(uv.y*pi),
		-r * cosf(uv.y*pi),
		-r * sinf(uv.x*2.f*pi)*sinf(uv.y*pi)
	};
}

// A henger alapján: a magasság függvényében csükkentjük a szélességet felfelé
glm::vec3 p_cone(glm::vec2 uv, float r = 1.f) {
	float hrad = (1.0 - uv.y)*r;
	return{
		hrad*cosf(uv.x*2.f*pi),
		uv.y*2.f - 1.f,
		-hrad * sinf(uv.x*2.f*pi)
	};
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
		{ 1, "vs_in_norm" },			// VAO 1-es csatorna menjen a vs_in_norm-ba
		{ 2, "vs_in_tex" }				// VAO 2-es csatorna menjen a vs_in_tex-be
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


	//
	// geometria definiálása (std::vector<...>) és GPU pufferekbe (m_buffer*) való feltöltése BufferData-val
	//

	// vertexek pozíciói:
	/*
	Az m_gpuBufferPos konstruktora már létrehozott egy GPU puffer azonosítót és a most következõ BufferData hívás ezt
	1. bind-olni fogja GL_ARRAY_BUFFER target-re (hiszen m_gpuBufferPos típusa ArrayBuffer) és
	2. glBufferData segítségével áttölti a GPU-ra az argumentumban adott tároló értékeit

	*/
	m_cube_gpuBufferPos.BufferData(
		std::vector<glm::vec3>{
		// hátsó lap
		glm::vec3(-1, -1, -1),
			glm::vec3(1, -1, -1),
			glm::vec3(1, 1, -1),
			glm::vec3(-1, 1, -1),
			// elülsõ lap
			glm::vec3(-1, -1, 1),
			glm::vec3(1, -1, 1),
			glm::vec3(1, 1, 1),
			glm::vec3(-1, 1, 1),

	}
	);

	// és a primitíveket alkotó csúcspontok indexei (az elõzõ tömbökbõl) - triangle list-el való kirajzolásra felkészülve
	m_cube_gpuBufferIndices.BufferData(
		std::vector<int>{
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

	// geometria VAO-ban való regisztrálása
	m_cube_vao.Init(
	{
		// 0-ás attribútum "lényegében" glm::vec3-ak sorozata és az adatok az m_gpuBufferPos GPU pufferben vannak
		{ CreateAttribute<		0,						// csatorna: 0
								glm::vec3,				// CPU oldali adattípus amit a 0-ás csatorna attribútumainak meghatározására használtunk <- az eljárás a glm::vec3-ból kikövetkezteti, hogy 3 darab float-ból áll a 0-ás attribútum
								0,						// offset: az attribútum tároló elejétõl vett offset-je, byte-ban
								sizeof(glm::vec3)		// stride: a következõ csúcspont ezen attribútuma hány byte-ra van az aktuálistól
							>, m_cube_gpuBufferPos },
	},
	m_cube_gpuBufferIndices
	);

	///////////////////////////////////////////////////////////////

	BuildSphereGeometry();

	///////////////////////////////////////////////////////////////

	// earth
	glGenTextures(1, &m_earthTexture);
	glBindTexture(GL_TEXTURE_2D, m_earthTexture);
	TextureFromFileAttach("asset/earth.png", GL_TEXTURE_2D);

	// Mipmap generálása
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);

	// skybox
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	glGenTextures(1, &m_skyboxTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_skyboxTexture);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	TextureFromFileAttach("asset/xpos.png", GL_TEXTURE_CUBE_MAP_POSITIVE_X, false);
	TextureFromFileAttach("asset/xneg.png", GL_TEXTURE_CUBE_MAP_NEGATIVE_X, false);
	TextureFromFileAttach("asset/ypos.png", GL_TEXTURE_CUBE_MAP_POSITIVE_Y, false);
	TextureFromFileAttach("asset/yneg.png", GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, false);
	TextureFromFileAttach("asset/zpos.png", GL_TEXTURE_CUBE_MAP_POSITIVE_Z, false);
	TextureFromFileAttach("asset/zneg.png", GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, false);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	// suzanne
	m_suzanne = ObjParser::parse("asset/suzanne.obj");
	m_suzanne->initBuffers();

	// kamera
	m_camera.SetProj(45.0f, 640.0f / 480.0f, 0.01f, 1000.0f);

	return true;
}

void CMyApp::BuildSphereGeometry() {
	std::vector<glm::vec3> param_pos;
	std::vector<glm::vec3> param_norm;
	std::vector<glm::vec2> param_tex;

	const int P_HEIGHT = 15;
	const int P_WIDTH = 31;

	for (int y = 0; y <= P_HEIGHT; y++) {
		for (int x = 0; x <= P_WIDTH; x++) {

			glm::vec2 uv = { x / (float)P_WIDTH, y / (float)P_HEIGHT };


			glm::vec3 pos = p_sphere(uv, 1);
			param_pos.push_back(pos);
			param_norm.push_back(glm::normalize(pos)); // gömb esetén egyenlõ a normalizált vertex pozícióval
			param_tex.push_back(uv);

		}
	}

	std::vector<int> indices;
	indices.resize(3 * 2 * (P_WIDTH)*(P_HEIGHT));

	for (int i = 0; i < P_WIDTH; i++) {
		for (int j = 0; j < P_HEIGHT; j++)
		{
			// minden négyszögre csináljunk kettõ háromszöget, amelyek a következõ 
			// (i,j) indexeknél született (u_i, v_i) paraméterértékekhez tartozó
			// pontokat kötik össze:
			//
			//		(i,j+1)
			//		  o-----o(i+1,j+1)
			//		  |\    |			a = p(u_i, v_i)
			//		  | \   |			b = p(u_{i+1}, v_i)
			//		  |  \  |			c = p(u_i, v_{i+1})
			//		  |   \ |			d = p(u_{i+1}, v_{i+1})
			//		  |	   \|
			//	(i,j) o-----o(i+1, j)
			//
			// - az (i,j)-hez tartózó 1D-s index a VBO-ban: i+j*(P_WIDTH+1)
			// - az (i,j)-hez tartózó 1D-s index az IB-ben: i*6+j*6*(P_WIDTH+1) 
			//		(mert minden négyszöghöz 2db háromszög = 6 index tartozik)
			//
			indices[6 * i + j * 3 * 2 * (P_WIDTH)+0] = (i)+(j)*	(P_WIDTH + 1);
			indices[6 * i + j * 3 * 2 * (P_WIDTH)+1] = (i + 1) + (j)*	(P_WIDTH + 1);
			indices[6 * i + j * 3 * 2 * (P_WIDTH)+2] = (i)+(j + 1)*(P_WIDTH + 1);
			indices[6 * i + j * 3 * 2 * (P_WIDTH)+3] = (i + 1) + (j)*	(P_WIDTH + 1);
			indices[6 * i + j * 3 * 2 * (P_WIDTH)+4] = (i + 1) + (j + 1)*(P_WIDTH + 1);
			indices[6 * i + j * 3 * 2 * (P_WIDTH)+5] = (i)+(j + 1)*(P_WIDTH + 1);
		}
	}

	sphere.size = indices.size();

	sphere.gpuBufferPos.BufferData(param_pos);
	sphere.gpuBufferNorm.BufferData(param_norm);
	sphere.gpuBufferTex.BufferData(param_tex);

	sphere.gpuBufferIndices.BufferData(indices);

	// geometria VAO-ban való regisztrálása
	sphere.vao.Init(
		{
			{ CreateAttribute<0,glm::vec3>, sphere.gpuBufferPos },
		{ CreateAttribute<1,glm::vec3>, sphere.gpuBufferNorm },
		{ CreateAttribute<2,glm::vec2>, sphere.gpuBufferTex }
		},
		sphere.gpuBufferIndices
	);
}

void CMyApp::TextureFromFileAttach(const char* filename, GLuint role, bool flip) const
{
	// Kép betöltése
	SDL_Surface* loaded_img = IMG_Load(filename);
	if (loaded_img == nullptr)
	{
		std::cout << "[TextureFromFile] Hiba a kép betöltése közben: " << filename << std::endl;
		return;
	}

	// Átalakítás 32bit RGBA formátumra, ha nem abban volt
	SDL_Surface* formattedSurf = SDL_ConvertSurfaceFormat(loaded_img, SDL_PIXELFORMAT_RGBA32, 0);
	if (formattedSurf == nullptr)
	{
		std::cout << "[TextureFromFile] Hiba a kép feldolgozása közben: " << SDL_GetError() << std::endl;
		SDL_FreeSurface(loaded_img);
		return;
	}

	// Áttérés SDL koordinátarendszerrõl ( (0,0) balfent ) OpenGL textúra-koordinátarendszerre ( (0,0) ballent )
	if (flip && SDL_InvertSurface(formattedSurf) == -1) {
		std::cout << "[TextureFromFile] Hiba a kép feldolgozása közben: " << SDL_GetError() << std::endl;
		SDL_FreeSurface(formattedSurf);
		SDL_FreeSurface(loaded_img);
		return;
	}

	// OpenGL textúra generálás
	glTexImage2D(role, 0, GL_RGBA, formattedSurf->w, formattedSurf->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, formattedSurf->pixels);

	// Mipmap generálása
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glGenerateMipmap(GL_TEXTURE_2D);

	// Használt SDL_Surface-k felszabadítása
	SDL_FreeSurface(formattedSurf);
	SDL_FreeSurface(loaded_img);

}

void CMyApp::Clean()
{
	glDeleteTextures(1, &m_skyboxTexture);
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

	// kocka

	m_program.Use();

	//

	glm::vec3 m_eye = m_camera.GetEye();
	m_program.SetUniform("eye", m_eye);

	// Suzanne
	m_program.SetTexture("myTexture", 0, m_earthTexture);

	glm::mat4 matWorld = glm::mat4(1.0f);
	glm::mat4 matWorldIT = glm::transpose(glm::inverse(matWorld));
	m_program.SetUniform("MVP", m_camera.GetViewProj()*matWorld);
	m_program.SetUniform("world", matWorld);
	m_program.SetUniform("worldIT", matWorldIT);
	
	m_suzanne->draw();

	sphere.vao.Bind();
	// kis gömbök
	for (int i = 0; i < 10; ++i)
	{
		matWorld =
			glm::rotate(SDL_GetTicks() / 1000.0f + 2 * glm::pi<float>() / 10 * i, glm::vec3(0, 1, 0))*
			glm::translate(glm::vec3(spheres_distance + 5 * sinf(SDL_GetTicks() / 1000.0f), 0, 0))*
			glm::rotate((i + 1)*SDL_GetTicks() / 1000.0f, glm::vec3(0, 1, 0));
		glm::mat4 matWorldIT = glm::transpose(glm::inverse(matWorld));

		m_program.SetUniform("MVP", m_camera.GetViewProj()*matWorld);
		m_program.SetUniform("world", matWorld);
		m_program.SetUniform("worldIT", matWorldIT);
		glDrawElements(GL_TRIANGLES, sphere.size, GL_UNSIGNED_INT, nullptr);
	}

	// skybox

	// mentsük el az elõzõ Z-test eredményt, azaz azt a relációt, ami alapján update-eljük a pixelt.
	GLint prevDepthFnc;
	glGetIntegerv(GL_DEPTH_FUNC, &prevDepthFnc);

	// most kisebb-egyenlõt használjunk, mert mindent kitolunk a távoli vágósíkokra
	glDepthFunc(GL_LEQUAL);

	m_cube_vao.Bind();

	m_programSkybox.Use();
	m_programSkybox.SetUniform("MVP", m_camera.GetViewProj() * glm::translate( m_camera.GetEye()) );
	
	// cube map textúra beállítása 0-ás mintavételezõre és annak a shaderre beállítása
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_skyboxTexture);
	glUniform1i(m_programSkybox.GetLocation("skyboxTexture"), 0);

	// az elõzõ három sor <=> m_programSkybox.SetCubeTexture("skyboxTexture", 0, m_skyboxTexture);

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);

	// végül állítsuk vissza
	glDepthFunc(prevDepthFnc);

	// 1. feladat: készíts egy vertex shader-fragment shader párt, ami tárolt geometria _nélkül_ kirajzol egy tetszõleges pozícióba egy XYZ tengely-hármast,
	//			   ahol az X piros, az Y zöld a Z pedig kék!

	//ImGui Testwindow
	//ImGui::ShowTestWindow();
	
	//Gömbök távolságának állítása GUI-val
	ImGui::SetNextWindowPos(ImVec2(20, 40), ImGuiSetCond_FirstUseEver);
	// csak akkor lépjünk be, hogy ha az ablak nincs csíkká lekicsinyítve...
	if (ImGui::Begin("Distance designer"))
	{
		ImGui::SliderFloat("Radius", &spheres_distance, 8.0, 15.0);
	}
	ImGui::End(); // ...de még ha le is volt, End()-et hívnunk kell
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