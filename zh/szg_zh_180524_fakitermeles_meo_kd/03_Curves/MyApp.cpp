#include "MyApp.h"

#include <math.h>
#include <vector>

#include <random>

#include <array>
#include <list>
#include <tuple>

#include "imgui\imgui.h"

#include "ObjParser_OGL3.h"

CMyApp::CMyApp(void)
{
}


CMyApp::~CMyApp(void)
{
	
}

glm::vec3 CMyApp::GetDombPos(float u, float v)
{
	float y = 0;
	int rnd = rand() % 10;
	float randY = rnd / 10.f;
	//std::cout << randY << std::endl;
	return glm::vec3(20 * u - 10, y + randY, 20 * v - 10);
}

glm::vec3 CMyApp::GetDombNorm(float u, float v)
{

	// Numerikusan (nem kell ismerni a képletet, elég a pozícióét)
	glm::vec3 uv = GetDombPos(u, v);
	glm::vec3 du = GetDombPos(u+0.01, v)- GetDombPos(u-0.01, v);
	glm::vec3 dv = GetDombPos(u, v+0.01)- GetDombPos(u, v-0.01);

	return (glm::cross(du, dv));
}

glm::vec2 CMyApp::GetDombTex(float u, float v)
{
	return glm::vec2(u, 1 - v);
}
//------------------------------------------------------------------
//ERDŐ_HÁTTÉR
glm::vec3 CMyApp::GetErdoPos(float u, float v)
{
	return glm::vec3(20 * u - 10, 0, 10 * v - 10);
}

glm::vec3 CMyApp::GetErdoNorm(float u, float v)
{

	// Numerikusan (nem kell ismerni a képletet, elég a pozícióét)
	glm::vec3 uv = GetErdoPos(u, v);
	glm::vec3 du = GetErdoPos(u + 0.01, v) - GetErdoPos(u - 0.01, v);
	glm::vec3 dv = GetErdoPos(u, v + 0.01) - GetErdoPos(u, v - 0.01);

	return (glm::cross(du, dv));
}

glm::vec2 CMyApp::GetErdoTex(float u, float v)
{
	return glm::vec2(u, 1 - v);
}
//------------------------------------------------------------------
//LOMBKORONA
glm::vec3 CMyApp::GetPos(float u, float v)
{
	// origó középpontú, egységsugarú gömb parametrikus alakja: http://hu.wikipedia.org/wiki/G%C3%B6mb#Egyenletek 
	// figyeljünk:	matematikában sokszor a Z tengely mutat felfelé, de nálunk az Y, tehát a legtöbb képlethez képest nálunk
	//				az Y és Z koordináták felcserélve szerepelnek
	u *= 2 * 3.1415f;
	v *= 3.1415f;
	float cu = cosf(u), su = sinf(u), cv = cosf(v), sv = sinf(v);
	float r = 1;

	return glm::vec3(r*cu*sv, r*cv, r*su*sv);
} 
glm::vec3 CMyApp::GetNorm(float u, float v)
{
	// Képlettel
	u *= 3.1415f;
	v *= 3.1415f;
	float cu = cosf(u), su = sinf(u), cv = cosf(v), sv = sinf(v);
	return glm::vec3(cu*sv, cv, su*sv);

	// Numerikusan (nem kell ismerni a képletet, elég a pozícióét)
	/*glm::vec3 uv = GetPos(u, v);
	glm::vec3 du = GetPos(u+0.01, v)-GetPos(u-0.01, v);
	glm::vec3 dv = GetPos(u, v+0.01)-GetPos(u, v-0.01);

	return (glm::cross(du, dv));*/
}

glm::vec2 CMyApp::GetTex(float u, float v)
{
	return glm::vec2(u, 1 - v);
}
//------------------------------------------------------------------
bool CMyApp::Init()
{
	// törlési szín legyen fehér
	glClearColor(1,1,1, 1);

	glEnable(GL_CULL_FACE); // kapcsoljuk be a hatrafele nezo lapok eldobasat
	glEnable(GL_DEPTH_TEST); // mélységi teszt bekapcsolása (takarás)
	//glDepthMask(GL_FALSE); // kikapcsolja a z-pufferbe történő írást - https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/glDepthMask.xml

	glLineWidth(4.0f); // a vonalprimitívek vastagsága: https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glLineWidth.xhtml
	//glEnable(GL_LINE_SMOOTH);

	// átlátszóság engedélyezése
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // meghatározza, hogy az átlátszó objektum az adott pixelben hogyan módosítsa a korábbi fragmentekből oda lerakott színt: https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glBlendFunc.xhtml

	// a raszterizált pontprimitívek mérete
	glPointSize(15.0f);

	//
	// shaderek betöltése
	//

	// így is létre lehetne hozni:

	// a shadereket tároló program létrehozása
	m_program.Init({ 
		{ GL_VERTEX_SHADER, "myVert.vert"},
		{ GL_FRAGMENT_SHADER, "myFrag.frag"}
	},
	{ 
		{ 0, "vs_in_pos" },					// VAO 0-as csatorna menjen a vs_in_pos-ba
		{ 1, "vs_in_normal" },				// VAO 1-es csatorna menjen a vs_in_normal-ba
		{ 2, "vs_out_tex0" },				// VAO 2-es csatorna menjen a vs_in_tex0-ba
	});

	m_program.LinkProgram();

	// tengelyeket kirajzoló shader rövid inicializálása
	m_axesProgram.Init({
		{GL_VERTEX_SHADER, "axes.vert"},
		{GL_FRAGMENT_SHADER, "axes.frag"}
	});
	// haladóknak, még rövidebb: 
	//m_axesProgram.Init({"axes.vert"_vs, "axes.frag"_fs});

	// kontrollpontokat kirajzoló program
	m_pointProgram.Init({
		{ GL_VERTEX_SHADER, "pointDrawer.vert" },
		{ GL_FRAGMENT_SHADER, "pointDrawer.frag" }
	});

	//
	// geometria definiálása (std::vector<...>) és GPU pufferekbe (m_buffer*) való feltöltése BufferData-val
	//

	domborzat_program.Init({
		{ GL_VERTEX_SHADER, "domborzat.vert" },
		{ GL_FRAGMENT_SHADER, "domborzat.frag" }
		},
	{
		{ 0, "vs_in_pos" },					// VAO 0-as csatorna menjen a vs_in_pos-ba
		{ 1, "vs_in_normal" },				// VAO 1-es csatorna menjen a vs_in_normal-ba
		{ 2, "vs_out_tex0" },				// VAO 2-es csatorna menjen a vs_in_tex0-ba
	});

	erdo_program.LinkProgram();

	erdo_program.Init({
		{ GL_VERTEX_SHADER, "erdo.vert" },
		{ GL_FRAGMENT_SHADER, "erdo.frag" }
		},
	{
		{ 0, "vs_in_pos" },					// VAO 0-as csatorna menjen a vs_in_pos-ba
		{ 1, "vs_in_normal" },				// VAO 1-es csatorna menjen a vs_in_normal-ba
		{ 2, "vs_out_tex0" },				// VAO 2-es csatorna menjen a vs_in_tex0-ba
	});

	erdo_program.LinkProgram();

	gomb_program.AttachShaders({
		{ GL_VERTEX_SHADER, "gomb.vert" },
		{ GL_FRAGMENT_SHADER, "gomb.frag" }
		});

	// attributomok osszerendelese a VAO es shader kozt
	gomb_program.BindAttribLocations({
		{ 0, "vs_in_pos" },				// VAO 0-as csatorna menjen a vs_in_pos-ba
		{ 1, "vs_in_norm" },				// VAO 1-es csatorna menjen a vs_in_col-ba
		{ 2, "vs_in_text" }
		});

	gomb_program.LinkProgram();

	// vertexek pozíciói:
	/*
	Az m_gpuBufferPos konstruktora már létrehozott egy GPU puffer azonosítót és a most következő BufferData hívás ezt 
		1. bind-olni fogja GL_ARRAY_BUFFER target-re (hiszen m_gpuBufferPos típusa ArrayBuffer) és
		2. glBufferData segítségével áttölti a GPU-ra az argumentumban adott tároló értékeit

	*/
	m_gpuBufferPos.BufferData( 
		std::vector<glm::vec3>{
			//		  X, Y, Z
			glm::vec3(-10,-1, -10),
			glm::vec3(-10,-1,  10), 
			glm::vec3( 10,-1, -10),
			glm::vec3( 10,-1,  10) 
		}
	);

	// normálisai
	m_gpuBufferNormal.BufferData(
		std::vector<glm::vec3>{
			// normal.X,.Y,.Z
			glm::vec3(0, 1, 0), // 0-ás csúcspont
			glm::vec3(0, 1, 0), // 1-es
			glm::vec3(0, 1, 0), // 2-es
			glm::vec3(0, 1, 0)  // 3-as
		}
	);

	// textúrakoordinátái
	m_gpuBufferTex.BufferData(
		std::vector<glm::vec2>{
			//        u, v
			glm::vec2(0, 0), // 0-ás csúcspont
			glm::vec2(1, 0), // 1-es
			glm::vec2(0, 1), // 2-es
			glm::vec2(1, 1)	 // 3-as
		}
	);

	// és a primitíveket alkotó csúcspontok indexei (az előző tömbökből) - triangle list-el való kirajzolásra felkészülve
	m_gpuBufferIndices.BufferData(
		std::vector<int>{
			0,1,2,	// 1. háromszög: 0-1-2 csúcspontokból
			2,1,3	// 2. háromszög: 2-1-3 csúcspontokból
		}
	);

	//domborzat feltöltése adatokkal
	std::vector<glm::vec3> dombNorm;
	std::vector<glm::vec2> dombText;

	std::vector<int> dombInd;

	

	for (int i = 0; i <= N; i++) {
		for (int j = 0; j <= M; j++) {
			float u = i / (float)N;
			float v = j / (float)M;
			
			glm::vec3 temp = GetDombPos(u, v);
			dombPos.push_back(temp);
			dombNorm.push_back(GetDombNorm(u,v));
			dombText.push_back(GetDombTex(u,v));

			dombY.insert(std::pair<std::pair<int, int>, float>(std::pair<int, int>((int)temp.x, (int)temp.z), temp.y));
			//kezdetben sehol sincs fa
			van_e_Fa.insert(std::pair<std::pair<int, int>, bool>(std::pair<int, int>((int)temp.x, (int)temp.z), false));
			fa_kora.insert(std::pair<std::pair<int, int>, bool>(std::pair<int, int>((int)temp.x, (int)temp.z), 0));

		}
	}

	for (int i = 0; i < N; ++i)
		for (int j = 0; j < M; ++j)
		{
			// minden négyszögre csináljunk kettő háromszöget, amelyek a következő 
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
			// - az (i,j)-hez tartózó 1D-s index a VBO-ban: i+j*(N+1)
			// - az (i,j)-hez tartózó 1D-s index az IB-ben: i*6+j*6*(N+1) 
			//		(mert minden négyszöghöz 2db háromszög = 6 index tartozik)
			//
			dombInd.push_back((i)+(j)*	(N + 1));
			dombInd.push_back((i + 1) + (j)*	(N + 1));
			dombInd.push_back((i)+(j + 1)*(N + 1));
			dombInd.push_back((i + 1) + (j)*	(N + 1));
			dombInd.push_back((i + 1) + (j + 1)*(N + 1));
			dombInd.push_back((i)+(j + 1)*(N + 1));
		}

	domborzat_BufferPos.BufferData(dombPos);
	domborzat_BufferNorm.BufferData(dombNorm);
	domborzat_BufferText.BufferData(dombText);
	domborzat_BufferIndices.BufferData(dombInd);

	//-------------------------------------------------------------
	//erdő feltöltése
	//domborzat feltöltése adatokkal
	std::vector<glm::vec3> erdoPos;
	std::vector<glm::vec3> erdoNorm;
	std::vector<glm::vec2> erdoText;

	std::vector<int> erdoInd;

	for (int i = 0; i <= N; i++) {
		for (int j = 0; j <= M; j++) {
			float u = i / (float)N;
			float v = j / (float)M;

			erdoPos.push_back(GetErdoPos(u, v));
			erdoNorm.push_back(GetErdoNorm(u, v));
			erdoText.push_back(GetErdoTex(u, v));
		}
	}

	for (int i = 0; i < N; ++i)
		for (int j = 0; j < M; ++j)
		{
			// minden négyszögre csináljunk kettő háromszöget, amelyek a következő 
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
			// - az (i,j)-hez tartózó 1D-s index a VBO-ban: i+j*(N+1)
			// - az (i,j)-hez tartózó 1D-s index az IB-ben: i*6+j*6*(N+1) 
			//		(mert minden négyszöghöz 2db háromszög = 6 index tartozik)
			//
			erdoInd.push_back((i)+(j)*	(N + 1));
			erdoInd.push_back((i + 1) + (j)*	(N + 1));
			erdoInd.push_back((i)+(j + 1)*(N + 1));
			erdoInd.push_back((i + 1) + (j)*	(N + 1));
			erdoInd.push_back((i + 1) + (j + 1)*(N + 1));
			erdoInd.push_back((i)+(j + 1)*(N + 1));
		}

	erdo_BufferPos.BufferData(erdoPos);
	erdo_BufferNorm.BufferData(erdoNorm);
	erdo_BufferText.BufferData(erdoText);
	erdo_BufferIndices.BufferData(erdoInd);
	//-------------------------------------------------------------
	//LOMBKORONA
	std::vector<glm::vec3> gombPos;
	std::vector<glm::vec3> gombNorm;
	std::vector<glm::vec2> gombText;
	std::vector<int> gombInd;
	//Vertex vert[(N + 1)*(M + 1)];
	for (int i = 0; i <= N; ++i)
		for (int j = 0; j <= M; ++j)
		{
			float u = i / (float)N;
			float v = j / (float)M;

			gombPos.push_back(GetPos(u, v));
			gombNorm.push_back(GetNorm(u, v));
			gombText.push_back(GetTex(u, v));
		}
	//GLushort indices[3 * 2 * (N)*(M)];
	for (int i = 0; i<N; ++i)
		for (int j = 0; j<M; ++j)
		{
			// minden négyszögre csináljunk kettő háromszöget, amelyek a következő 
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
			// - az (i,j)-hez tartózó 1D-s index a VBO-ban: i+j*(N+1)
			// - az (i,j)-hez tartózó 1D-s index az IB-ben: i*6+j*6*(N+1) 
			//		(mert minden négyszöghöz 2db háromszög = 6 index tartozik)
			//
			gombInd.push_back((i)+(j)*	(N + 1));
			gombInd.push_back((i + 1) + (j)*	(N + 1));
			gombInd.push_back((i)+(j + 1)*(N + 1));
			gombInd.push_back((i + 1) + (j)*	(N + 1));
			gombInd.push_back((i + 1) + (j + 1)*(N + 1));
			gombInd.push_back((i)+(j + 1)*(N + 1));
		}

	gomb_BufferPos.BufferData(gombPos);
	gomb_BufferNorm.BufferData(gombNorm);
	gomb_BufferText.BufferData(gombText);
	gomb_BufferIndices.BufferData(gombInd);
	// geometria VAO-ban való regisztrálása
	m_vao.Init(
		{
			{ CreateAttribute<0, glm::vec3, 0, sizeof(glm::vec3)>, m_gpuBufferPos },	// 0-ás attribútum "lényegében" glm::vec3-ak sorozata és az adatok az m_gpuBufferPos GPU pufferben vannak
			{ CreateAttribute<1, glm::vec3, 0, sizeof(glm::vec3)>, m_gpuBufferNormal },	// 1-es attribútum "lényegében" glm::vec3-ak sorozata és az adatok az m_gpuBufferNormal GPU pufferben vannak
			{ CreateAttribute<2, glm::vec2, 0, sizeof(glm::vec2)>, m_gpuBufferTex }		// 2-es attribútum "lényegében" glm::vec2-ők sorozata és az adatok az m_gpuBufferTex GPU pufferben vannak
		},
		m_gpuBufferIndices
	);

	domborzat_vao.Init(
		{
			{ CreateAttribute<0, glm::vec3, 0, sizeof(glm::vec3)>, domborzat_BufferPos },	// 0-ás attribútum "lényegében" glm::vec3-ak sorozata és az adatok az m_gpuBufferPos GPU pufferben vannak
			{ CreateAttribute<1, glm::vec3, 0, sizeof(glm::vec3)>, domborzat_BufferNorm },	// 1-es attribútum "lényegében" glm::vec3-ak sorozata és az adatok az m_gpuBufferNormal GPU pufferben vannak
			{ CreateAttribute<2, glm::vec2, 0, sizeof(glm::vec2)>, domborzat_BufferText }		// 2-es attribútum "lényegében" glm::vec2-ők sorozata és az adatok az m_gpuBufferTex GPU pufferben vannak
		},
		domborzat_BufferIndices
	);

	erdo_vao.Init(
		{
			{ CreateAttribute<0, glm::vec3, 0, sizeof(glm::vec3)>, erdo_BufferPos },	// 0-ás attribútum "lényegében" glm::vec3-ak sorozata és az adatok az m_gpuBufferPos GPU pufferben vannak
			{ CreateAttribute<1, glm::vec3, 0, sizeof(glm::vec3)>, erdo_BufferNorm },	// 1-es attribútum "lényegében" glm::vec3-ak sorozata és az adatok az m_gpuBufferNormal GPU pufferben vannak
			{ CreateAttribute<2, glm::vec2, 0, sizeof(glm::vec2)>, erdo_BufferText }		// 2-es attribútum "lényegében" glm::vec2-ők sorozata és az adatok az m_gpuBufferTex GPU pufferben vannak
		},
		erdo_BufferIndices
	);

	gomb_vao.Init({
		{ CreateAttribute<0, glm::vec3, 0, sizeof(glm::vec3)>, gomb_BufferPos },
		{ CreateAttribute<1, glm::vec3, 0, sizeof(glm::vec3)>, gomb_BufferNorm },
		{ CreateAttribute<2, glm::vec2, 0, sizeof(glm::vec2)>, gomb_BufferText },
		},
		gomb_BufferIndices
		);


	// textúra betöltése
	m_textureMetalDomborzat.FromFile("grass.jpg");
	m_textureMetalErdo.FromFile("forest.jpg");
	m_textureMetalGomb.FromFile("leaves.jpg");
	m_textureMetalHenger.FromFile("bark.jpg");

	// mesh betöltése
	m_mesh = ObjParser::parse("henger.obj");

	// kamera
	m_camera.SetProj(45.0f, 640.0f / 480.0f, 0.01f, 1000.0f);

	//random elhelyezkedés
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> rnd(-9, 9);

	int rand_db = (rand() % 50) + 50;

	for (int i = 0; i < rand_db; i++) {

		int x = rnd(gen);
		int z = rnd(gen);
		float y = dombY.at(std::pair<int, int>(x,z));

		m_fa_db.push_back(glm::vec3(x, y, z));
		fa_merete.push_back((0.95f / fa_ido) * t + 0.5f);
		van_e_Fa.at(std::pair<int, int>(x, z)) = true;
	}

	return true;
}

void CMyApp::Clean()
{
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
	t = SDL_GetTicks() / 1000.0f;
	
	// talaj - domborzat
	domborzat_program.Use();

	domborzat_program.SetTexture("texImage", 0, m_textureMetalDomborzat);

	glm::mat4 wallWorld = glm::mat4(1.f);
	domborzat_program.SetUniform("world", wallWorld);
	domborzat_program.SetUniform("worldIT", glm::transpose(glm::inverse(wallWorld)));
	domborzat_program.SetUniform("MVP", m_camera.GetViewProj()*wallWorld);
	domborzat_program.SetUniform("Kd", glm::vec4(1,1,1, 1));

	domborzat_vao.Bind();
	glDrawElements(GL_TRIANGLES, 3 * 2 * N * M, GL_UNSIGNED_INT, 0);
	domborzat_vao.Unbind(); // nem feltétlen szükséges: a m_mesh->draw beállítja a saját VAO-ját

	//---------------------------------------------------------------------------
	//ERDŐ_HÁTTÉR
	erdo_program.Use();

	erdo_program.SetTexture("texImage", 0, m_textureMetalErdo);

	glm::mat4 forestWorld = glm::rotate(glm::radians(90.f), glm::vec3(1.f, 0, 0)) * glm::translate(glm::vec3(0, -10, -10)) * glm::scale(glm::vec3(-1.f, -0.5f, -1.f));
	erdo_program.SetUniform("world", forestWorld);
	erdo_program.SetUniform("worldIT", glm::transpose(glm::inverse(forestWorld)));
	erdo_program.SetUniform("MVP", m_camera.GetViewProj()*forestWorld);
	erdo_program.SetUniform("Kd", glm::vec4(1, 1, 1, 1));

	erdo_vao.Bind();
	glDrawElements(GL_TRIANGLES, 3 * 2 * N * M, GL_UNSIGNED_INT, 0);
	erdo_vao.Unbind(); // nem feltétlen szükséges: a m_mesh->draw beállítja a saját VAO-ját

	//ERDŐ-JOBB HÁTTÉR
	erdo_program.Use();

	erdo_program.SetTexture("texImage", 0, m_textureMetalErdo);

	forestWorld = glm::rotate(glm::radians(90.f), glm::vec3(0, 0, 1.f)) * glm::rotate(glm::radians(270.f), glm::vec3(0, 1.f, 0)) * glm::translate(glm::vec3(0, -10.f, -10.f)) * glm::scale(glm::vec3(-1.f, -0.5f, -1.f));
	erdo_program.SetUniform("world", forestWorld);
	erdo_program.SetUniform("worldIT", glm::transpose(glm::inverse(forestWorld)));
	erdo_program.SetUniform("MVP", m_camera.GetViewProj()*forestWorld);
	erdo_program.SetUniform("Kd", glm::vec4(1, 1, 1, 1));

	erdo_vao.Bind();
	glDrawElements(GL_TRIANGLES, 3 * 2 * N * M, GL_UNSIGNED_INT, 0);
	erdo_vao.Unbind(); // nem feltétlen szükséges: a m_mesh->draw beállítja a saját VAO-ját

	//ERDŐ-BAL HÁTTÉR
	erdo_program.Use();

	erdo_program.SetTexture("texImage", 0, m_textureMetalErdo);

	forestWorld = glm::rotate(glm::radians(90.f), glm::vec3(0, 1.f, 0)) * glm::translate(glm::vec3(0, 10.f, -10.f)) * glm::rotate(glm::radians(90.f), glm::vec3(1.f, 0, 0)) * glm::scale(glm::vec3(-1.f, -0.5f, -1.f));
	erdo_program.SetUniform("world", forestWorld);
	erdo_program.SetUniform("worldIT", glm::transpose(glm::inverse(forestWorld)));
	erdo_program.SetUniform("MVP", m_camera.GetViewProj()*forestWorld);
	erdo_program.SetUniform("Kd", glm::vec4(1, 1, 1, 1));

	erdo_vao.Bind();
	glDrawElements(GL_TRIANGLES, 3 * 2 * N * M, GL_UNSIGNED_INT, 0);
	erdo_vao.Unbind(); // nem feltétlen szükséges: a m_mesh->draw beállítja a saját VAO-ját

	//ERDŐ-ELÜLSŐ HÁTTÉR
	erdo_program.Use();

	erdo_program.SetTexture("texImage", 0, m_textureMetalErdo);

	forestWorld = glm::rotate(glm::radians(270.f), glm::vec3(1.f, 0, 0)) * glm::rotate(glm::radians(180.f), glm::vec3(0, 1.f, 0)) * glm::translate(glm::vec3(0, -10.f, -10.f)) * glm::scale(glm::vec3(-1.f, -0.5f, -1.f));
	erdo_program.SetUniform("world", forestWorld);
	erdo_program.SetUniform("worldIT", glm::transpose(glm::inverse(forestWorld)));
	erdo_program.SetUniform("MVP", m_camera.GetViewProj()*forestWorld);
	erdo_program.SetUniform("Kd", glm::vec4(1, 1, 1, 1));

	erdo_vao.Bind();
	glDrawElements(GL_TRIANGLES, 3 * 2 * N * M, GL_UNSIGNED_INT, 0);
	erdo_vao.Unbind(); // nem feltétlen szükséges: a m_mesh->draw beállítja a saját VAO-ját

	//---------------------------------------------------------------------------
	
	for (int i = 0; i < m_fa_db.size(); i++) {
		//gömb
		gomb_vao.Bind();

		gomb_program.Use();

		gomb_program.SetTexture("texImage", 0, m_textureMetalGomb);
		if (fa_merete.at(i) < 1.f) {
			//std::cout << "change: " << (95.f / 10000.f) *t + 0.5f << std::endl;
			fa_merete.at(i) = (0.95f / fa_ido) *t + 0.5f;
		}
		//std::cout << fa_merete.at(i) << " " << t << std::endl;
		glm::mat4 gombWorld = glm::translate(m_fa_db.at(i)) *  glm::scale(glm::vec3(fa_merete.at(i), fa_merete.at(i), fa_merete.at(i))) * glm::translate(glm::vec3(0, 4.f, 0)) * glm::scale(glm::vec3(-1.f, -1.f, -1.f));

		gomb_program.SetUniform("world", gombWorld);
		gomb_program.SetUniform("worldIT", glm::transpose(glm::inverse(gombWorld)));
		gomb_program.SetUniform("Kd", glm::vec4(1, 0.3, 0.3, 1));
		gomb_program.SetUniform("MVP", m_camera.GetViewProj()*gombWorld);
		glDrawElements(GL_TRIANGLES, 3 * 2 * (N)*(M), GL_UNSIGNED_INT, nullptr);

		// Henger
		m_program.Use();

		m_program.SetTexture("texImage", 0, m_textureMetalHenger);
		glm::mat4 hengerWorld = glm::translate(m_fa_db.at(i)) *  glm::scale(glm::vec3(fa_merete.at(i), fa_merete.at(i), fa_merete.at(i))) * glm::translate(glm::vec3(0, 2.f, 0));
		m_program.SetUniform("world", hengerWorld);
		m_program.SetUniform("worldIT", glm::transpose(glm::inverse(hengerWorld)));
		m_program.SetUniform("MVP", m_camera.GetViewProj()*hengerWorld);
		m_program.SetUniform("Kd", glm::vec4(1, 0.3, 0.3, 1));

		m_mesh->draw();

	}

	//új csemeték
	
	if (((int)(t*10) % 20) == 0 && t != 0) {
		for (int i = 0; i <= dombPos.size()-1; i++) {
			// 5/100-ad az 1/20-ad, ami 20 számból pont egy db szám
			int rand_fa_letet = rand() % 20;
			if (van_e_Fa.at(std::pair<int, int>(dombPos.at(i).x, dombPos.at(i).z)) == false && rand_fa_letet == 0) {
				m_fa_db.push_back(glm::vec3(dombPos.at(i).x, dombY.at(std::pair<int, int>(dombPos.at(i).x, dombPos.at(i).z)), dombPos.at(i).z));
				fa_merete.push_back((95.f / 10000.f) * (t - (SDL_GetTicks() / 1000.f))  + 0.5f);
				van_e_Fa.at(std::pair<int, int>(dombPos.at(i).x, dombPos.at(i).z)) = true;
				fa_kora.at(std::pair<int, int>(dombPos.at(i).x, dombPos.at(i).z)) = 0;
			}
			//std::cout << "lassabban" << std::endl;
			if (van_e_Fa.at(std::pair<int, int>(dombPos.at(i).x, dombPos.at(i).z)) == true && fa_kora.at(std::pair<int, int>(dombPos.at(i).x, dombPos.at(i).z)) == 10) {
				van_e_Fa.at(std::pair<int, int>(dombPos.at(i).x, dombPos.at(i).z)) = false;
				
				int index = 0;
				for (int j = 0; j < m_fa_db.size(); j++) {
					if (m_fa_db[j].x == dombPos.at(i).x && m_fa_db[j].z == dombPos.at(i).z) {
						
						index = j;
					}
				}
				m_fa_db.erase(m_fa_db.begin() + index);
				fa_merete.erase(fa_merete.begin() + index);
				treeCount++;
			}
			else if (van_e_Fa.at(std::pair<int, int>(dombPos.at(i).x, dombPos.at(i).z)) == true) {
				fa_kora.at(std::pair<int, int>(dombPos.at(i).x, dombPos.at(i).z)) += 2;
				//std::cout << "Fa kora: " << fa_kora.at(std::pair<int, int>(dombPos.at(i).x, dombPos.at(i).z)) << std::endl;
			}
		}
	}
	//std::cout << m_fa_db.size() << std::endl;
	// végetért a 3D színtér rajzolása
	m_program.Unuse();
	
	//
	// UI
	//
	// A következő parancs megnyit egy ImGui tesztablakot és így látszik mit tud az ImGui.
	ImGui::ShowTestWindow();
	// A ImGui::ShowTestWindow implementációja az imgui_demo.cpp-ben található
	// Érdemes még az imgui.h-t böngészni, valamint az imgui.cpp elején a FAQ-ot elolvasni.
	// Rendes dokumentáció nincs, de a fentiek elegendőek kell legyenek.
	
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiSetCond_FirstUseEver);
	// csak akkor lépjünk be, hogy ha az ablak nincs csíkká lekicsinyítve...
	if (ImGui::Begin("Fakitermeles"))	
	{
		ImGui::Text("Kivagott fa szamlalo: %d", treeCount);

		if (ImGui::Button("Delete")) {
			m_fa_db.clear();
		}

		ImGui::SliderFloat("Fak novekedesenek az ideje", &fa_ido, 4.f, 12.f);

		for (int i = 0; i < m_fa_db.size(); i++) {
			ImGui::SliderFloat("Kivagando fak merete", &fa_merete.at(i), 0.5f, 1.2f);
		}

		

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

glm::vec3 CMyApp::Eval(float t)
{
	if (m_controlPoints.size() == 0)
		return glm::vec3(0);

	int interval = (int)t;

	if (interval < 0)
		return m_controlPoints[0];
		
	if (interval >= m_controlPoints.size()-1)
		return m_controlPoints[m_controlPoints.size()-1];

	float localT = t - interval;
	return (1- localT)*m_controlPoints[interval] + localT*m_controlPoints[interval+1];
}
