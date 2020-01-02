
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

//GÖMB
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

//--------------------------------------------
//LEVEGŐBUBI
glm::vec3 CMyApp::GetBubiPos(float u, float v)
{
	// origó középpontú, egységsugarú gömb parametrikus alakja: http://hu.wikipedia.org/wiki/G%C3%B6mb#Egyenletek 
	// figyeljünk:	matematikában sokszor a Z tengely mutat felfelé, de nálunk az Y, tehát a legtöbb képlethez képest nálunk
	//				az Y és Z koordináták felcserélve szerepelnek
	u *= 2 * 3.1415f;
	v *= 3.1415f;
	float cu = cosf(u), su = sinf(u), cv = cosf(v), sv = sinf(v);
	float r = 0.25f;

	return glm::vec3(r*cu*sv, r*cv, r*su*sv);
}

glm::vec3 CMyApp::GetBubiNorm(float u, float v)
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

glm::vec2 CMyApp::GetBubiTex(float u, float v)
{
	return glm::vec2(u, 1 - v);
}


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

	m_gombProgram.AttachShaders({
		{ GL_VERTEX_SHADER, "gomb.vert" },
		{ GL_FRAGMENT_SHADER, "gomb.frag" }
		});

	// attributomok osszerendelese a VAO es shader kozt
	m_gombProgram.BindAttribLocations({
		{ 0, "vs_in_pos" },				// VAO 0-as csatorna menjen a vs_in_pos-ba
		{ 1, "vs_in_norm" },				// VAO 1-es csatorna menjen a vs_in_col-ba
		{ 2, "vs_in_text" }
		});

	m_gombProgram.LinkProgram();

	m_bubiProgram.AttachShaders({
		{ GL_VERTEX_SHADER, "bubi.vert" },
		{ GL_FRAGMENT_SHADER, "bubi.frag" }
		});

	// attributomok osszerendelese a VAO es shader kozt
	m_bubiProgram.BindAttribLocations({
		{ 0, "vs_in_pos" },				// VAO 0-as csatorna menjen a vs_in_pos-ba
		{ 1, "vs_in_norm" },				// VAO 1-es csatorna menjen a vs_in_col-ba
		{ 2, "vs_in_text" }
		});

	m_bubiProgram.LinkProgram();


	//
	// geometria definiálása (std::vector<...>) és GPU pufferekbe (m_buffer*) való feltöltése BufferData-val
	//

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

	//-------------------------------------------------------------
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

	//---------------------------------------------------------
	//BUBI
	std::vector<glm::vec3> bubiPos;
	std::vector<glm::vec3> bubiNorm;
	std::vector<glm::vec2> bubiText;
	std::vector<int> bubiInd;
	//Vertex vert[(N + 1)*(M + 1)];
	for (int i = 0; i <= N; ++i)
		for (int j = 0; j <= M; ++j)
		{
			float u = i / (float)N;
			float v = j / (float)M;

			bubiPos.push_back(GetBubiPos(u, v));
			bubiNorm.push_back(GetBubiNorm(u, v));
			bubiText.push_back(GetBubiTex(u, v));
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
			bubiInd.push_back((i)+(j)*	(N + 1));
			bubiInd.push_back((i + 1) + (j)*	(N + 1));
			bubiInd.push_back((i)+(j + 1)*(N + 1));
			bubiInd.push_back((i + 1) + (j)*	(N + 1));
			bubiInd.push_back((i + 1) + (j + 1)*(N + 1));
			bubiInd.push_back((i)+(j + 1)*(N + 1));
		}

	bubi_BufferPos.BufferData(bubiPos);
	bubi_BufferNorm.BufferData(bubiNorm);
	bubi_BufferText.BufferData(bubiText);
	bubi_BufferIndices.BufferData(bubiInd);

	//---------------------------------------------------------
	m_gpuBufferPos.BufferData(
		std::vector<glm::vec3>{
		// hátsó lap
		glm::vec3(-1, -1, -1),
			glm::vec3(1, -1, -1),
			glm::vec3(1, 1, -1),
			glm::vec3(-1, 1, -1),
			// elülső lap
			glm::vec3(-1, -1, 1),
			glm::vec3(1, -1, 1),
			glm::vec3(1, 1, 1),
			glm::vec3(-1, 1, 1),

	}
	);

	// és a primitíveket alkotó csúcspontok indexei (az előző tömbökből) - triangle list-el való kirajzolásra felkészülve
	m_gpuBufferIndices.BufferData(
		std::vector<int>{
		// hátsó lap
		0, 1, 2,
			2, 3, 0,
			// elülső lap
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
			// felső
			3, 2, 6,
			3, 6, 7,
	}
	);

	//---------------------------------------------------------

	// geometria VAO-ban való regisztrálása
	m_vao.Init(
		{
			{ CreateAttribute<0, glm::vec3, 0, sizeof(glm::vec3)>, m_gpuBufferPos },	// 0-ás attribútum "lényegében" glm::vec3-ak sorozata és az adatok az m_gpuBufferPos GPU pufferben vannak
			{ CreateAttribute<1, glm::vec3, 0, sizeof(glm::vec3)>, m_gpuBufferNormal },	// 1-es attribútum "lényegében" glm::vec3-ak sorozata és az adatok az m_gpuBufferNormal GPU pufferben vannak
			{ CreateAttribute<2, glm::vec2, 0, sizeof(glm::vec2)>, m_gpuBufferTex }		// 2-es attribútum "lényegében" glm::vec2-ők sorozata és az adatok az m_gpuBufferTex GPU pufferben vannak
		},
		m_gpuBufferIndices
	);

	gomb_vao.Init({
		{ CreateAttribute<0, glm::vec3, 0, sizeof(glm::vec3)>, gomb_BufferPos },
		{ CreateAttribute<1, glm::vec3, 0, sizeof(glm::vec3)>, gomb_BufferNorm },
		{ CreateAttribute<2, glm::vec2, 0, sizeof(glm::vec2)>, gomb_BufferText },
		},
		gomb_BufferIndices
		);

	bubi_vao.Init({
			{ CreateAttribute<0, glm::vec3, 0, sizeof(glm::vec3)>, bubi_BufferPos },
			{ CreateAttribute<1, glm::vec3, 0, sizeof(glm::vec3)>, bubi_BufferNorm },
			{ CreateAttribute<2, glm::vec2, 0, sizeof(glm::vec2)>, bubi_BufferText },
		},
		bubi_BufferIndices
		);

	// textúra betöltése
	m_textureMetalAkvarium.FromFile("water.jpg");
	m_textureMetalHal.FromFile("hal.png");
	m_textureMetalPiramis.FromFile("wall.png");

	// mesh betöltése
	//m_mesh = ObjParser::parse("suzanne.obj");

	// kamera
	m_camera.SetProj(45.0f, 640.0f / 480.0f, 0.01f, 1000.0f);

	//BUBIKHOZ RANDOM
	//random elhelyezkedés
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> rnd(-20.f, 14.f);

	rand_db = rand() % 5 + 10;

	for (int i = 0; i < rand_db; i++) {

		int x = rnd(gen);
		int y = 0;
		int z = rnd(gen);

		m_bubi_db.push_back(glm::vec3(x, y, z));
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

	t = SDL_GetTicks() / 1000.f;

	//fő kocka akvárium
	m_vao.Bind();

	m_program.Use();

	m_program.SetTexture("texImage", 0, m_textureMetalAkvarium);
	glm::mat4 cubeWorld = glm::scale(glm::vec3(40.f, 15.f, 60.f)) * glm::translate(glm::vec3(0, 0, 0));	// kifordítjuk, mert egyébként "kívül a belül"
	m_program.SetUniform("MVP", m_camera.GetViewProj()*cubeWorld);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);


	//HAL - GÖMB
	gomb_vao.Bind();

	m_gombProgram.Use();

	m_gombProgram.SetTexture("texImage", 0, m_textureMetalHal);

	glm::mat4 gombWorld;

	for (int i = 0; i < halDb; i++) {
		gombWorld = glm::translate(glm::vec3(0, cosf(glm::radians((float)(360/(i+1)) * t)), 0)) *
					glm::rotate(glm::radians(t * (360.f / korIdo)), glm::vec3(0, 1, 0)) *
					glm::rotate(glm::radians(-36.f * (i + 1)), glm::vec3(0, 1, 0)) * 
					glm::translate(glm::vec3((float)korSugar, -11.f, 0)) *
					glm::rotate(glm::radians(-90.f), glm::vec3(0, 1.f, 0)) * 
					glm::scale(glm::vec3(3.f, 2.f, 1.5f)) * glm::scale(glm::vec3(-1.f, -1.f, -1.f));

		m_gombProgram.SetUniform("MVP", m_camera.GetViewProj() * gombWorld);
		glDrawElements(GL_TRIANGLES, 3 * 2 * (N)*(M), GL_UNSIGNED_INT, nullptr);
	}


	//piramis
	m_vao.Bind();

	m_program.Use();

	m_program.SetTexture("texImage", 0, m_textureMetalPiramis);
	cubeWorld = glm::translate(glm::vec3(0, -13.f, 0)) * glm::scale(glm::vec3(8.f, 2.f, 8.f)) * glm::scale(glm::vec3(-1, -1, -1));	// kifordítjuk, mert egyébként "kívül a belül"
	m_program.SetUniform("MVP", m_camera.GetViewProj()*cubeWorld);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);

	m_vao.Bind();

	m_program.Use();

	m_program.SetTexture("texImage", 0, m_textureMetalPiramis);
	cubeWorld = glm::translate(glm::vec3(0, -9.f, 0)) * glm::scale(glm::vec3(6.f, 2.f, 6.f)) * glm::scale(glm::vec3(-1, -1, -1));	// kifordítjuk, mert egyébként "kívül a belül"
	m_program.SetUniform("MVP", m_camera.GetViewProj()*cubeWorld);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);

	m_vao.Bind();

	m_program.Use();

	m_program.SetTexture("texImage", 0, m_textureMetalPiramis);
	cubeWorld = glm::translate(glm::vec3(0, -5.f, 0)) * glm::scale(glm::vec3(4.f, 2.f, 4.f)) * glm::scale(glm::vec3(-1, -1, -1));	// kifordítjuk, mert egyébként "kívül a belül"
	m_program.SetUniform("MVP", m_camera.GetViewProj()*cubeWorld);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);

	m_vao.Bind();

	m_program.Use();

	m_program.SetTexture("texImage", 0, m_textureMetalPiramis);
	cubeWorld = glm::translate(glm::vec3(0, -1.f, 0)) * glm::scale(glm::vec3(2.f, 2.f, 2.f)) * glm::scale(glm::vec3(-1, -1, -1));	// kifordítjuk, mert egyébként "kívül a belül"
	m_program.SetUniform("MVP", m_camera.GetViewProj()*cubeWorld);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
	
	//BUBI
	bubi_vao.Bind();

	m_bubiProgram.Use();

	//m_gombProgram.SetTexture("texImage", 0, m_textureMetalHal);

	glm::mat4 bubiWorld;

	

	if (bUp > 5.f) {
		kulonbseg += 25.f;
	}

	bUp = t - kulonbseg - 20.f;

	for (int i = 0; i < m_bubi_db.size(); i++) {
		bubiWorld = glm::translate(glm::vec3(0, bUp, 0)) *glm::translate(m_bubi_db.at(i)) * glm::translate(glm::vec3(5.f, 5.f, 0)) * glm::scale(glm::vec3(-1.f, -1.f, -1.f));

		m_bubiProgram.SetUniform("world", bubiWorld);
		m_bubiProgram.SetUniform("worldIT", glm::transpose(glm::inverse(bubiWorld)));
		m_bubiProgram.SetUniform("Kd", glm::vec4(1, 0.3, 0.3, 1));
		m_bubiProgram.SetUniform("MVP", m_camera.GetViewProj() * bubiWorld);
		glDrawElements(GL_TRIANGLES, 3 * 2 * (N)*(M), GL_UNSIGNED_INT, nullptr);
	}

	//két random pici piramis
	//piramis
	m_vao.Bind();

	m_program.Use();

	m_program.SetTexture("texImage", 0, m_textureMetalPiramis);
	cubeWorld = glm::translate(glm::vec3(25.f, -13.f, 10.f)) * glm::scale(glm::vec3(4.f, 1.f, 4.f)) * glm::scale(glm::vec3(-1, -1, -1));	// kifordítjuk, mert egyébként "kívül a belül"
	m_program.SetUniform("MVP", m_camera.GetViewProj()*cubeWorld);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);

	m_vao.Bind();

	m_program.Use();

	m_program.SetTexture("texImage", 0, m_textureMetalPiramis);
	cubeWorld = glm::translate(glm::vec3(25.f, -11.f, 10.f)) * glm::scale(glm::vec3(3.f, 1.f, 3.f)) * glm::scale(glm::vec3(-1, -1, -1));	// kifordítjuk, mert egyébként "kívül a belül"
	m_program.SetUniform("MVP", m_camera.GetViewProj()*cubeWorld);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);

	m_vao.Bind();

	m_program.Use();

	m_program.SetTexture("texImage", 0, m_textureMetalPiramis);
	cubeWorld = glm::translate(glm::vec3(25.f, -9.f, 10.f)) * glm::scale(glm::vec3(2.f, 1.f, 2.f)) * glm::scale(glm::vec3(-1, -1, -1));	// kifordítjuk, mert egyébként "kívül a belül"
	m_program.SetUniform("MVP", m_camera.GetViewProj()*cubeWorld);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);

	m_vao.Bind();

	m_program.Use();

	m_program.SetTexture("texImage", 0, m_textureMetalPiramis);
	cubeWorld = glm::translate(glm::vec3(25.f, -7.f, 10.f)) * glm::scale(glm::vec3(1.f, 1.f, 1.f)) * glm::scale(glm::vec3(-1, -1, -1));	// kifordítjuk, mert egyébként "kívül a belül"
	m_program.SetUniform("MVP", m_camera.GetViewProj()*cubeWorld);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);

	//masodik piramis kicsike
	m_vao.Bind();

	m_program.Use();

	m_program.SetTexture("texImage", 0, m_textureMetalPiramis);
	cubeWorld = glm::translate(glm::vec3(25.f, -13.f, -10.f)) * glm::scale(glm::vec3(4.f, 1.f, 4.f)) * glm::scale(glm::vec3(-1, -1, -1));	// kifordítjuk, mert egyébként "kívül a belül"
	m_program.SetUniform("MVP", m_camera.GetViewProj()*cubeWorld);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);

	m_vao.Bind();

	m_program.Use();

	m_program.SetTexture("texImage", 0, m_textureMetalPiramis);
	cubeWorld = glm::translate(glm::vec3(25.f, -11.f, -10.f)) * glm::scale(glm::vec3(3.f, 1.f, 3.f)) * glm::scale(glm::vec3(-1, -1, -1));	// kifordítjuk, mert egyébként "kívül a belül"
	m_program.SetUniform("MVP", m_camera.GetViewProj()*cubeWorld);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);

	m_vao.Bind();

	m_program.Use();

	m_program.SetTexture("texImage", 0, m_textureMetalPiramis);
	cubeWorld = glm::translate(glm::vec3(25.f, -9.f, -10.f)) * glm::scale(glm::vec3(2.f, 1.f, 2.f)) * glm::scale(glm::vec3(-1, -1, -1));	// kifordítjuk, mert egyébként "kívül a belül"
	m_program.SetUniform("MVP", m_camera.GetViewProj()*cubeWorld);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);

	m_vao.Bind();

	m_program.Use();

	m_program.SetTexture("texImage", 0, m_textureMetalPiramis);
	cubeWorld = glm::translate(glm::vec3(25.f, -7.f, -10.f)) * glm::scale(glm::vec3(1.f, 1.f, 1.f)) * glm::scale(glm::vec3(-1, -1, -1));	// kifordítjuk, mert egyébként "kívül a belül"
	m_program.SetUniform("MVP", m_camera.GetViewProj()*cubeWorld);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
	//
	// UI
	//
	// A következő parancs megnyit egy ImGui tesztablakot és így látszik mit tud az ImGui.
	//ImGui::ShowTestWindow();
	// A ImGui::ShowTestWindow implementációja az imgui_demo.cpp-ben található
	// Érdemes még az imgui.h-t böngészni, valamint az imgui.cpp elején a FAQ-ot elolvasni.
	// Rendes dokumentáció nincs, de a fentiek elegendőek kell legyenek.
	
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiSetCond_FirstUseEver);
	// csak akkor lépjünk be, hogy ha az ablak nincs csíkká lekicsinyítve...
	if (ImGui::Begin("Akvarium"))	
	{
		ImGui::SliderInt("Halak szama", &halDb, 0, 10);
		ImGui::SliderInt("Kor sugara", &korSugar, 10, 25);
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
