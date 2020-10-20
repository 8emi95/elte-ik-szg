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

glm::vec3 CMyApp::GetPos(float u, float v)
{
	// origó középpontú, egységsugarú gömb parametrikus alakja: http://hu.wikipedia.org/wiki/G%C3%B6mb#Egyenletek 
	// figyeljünk:	matematikában sokszor a Z tengely mutat felfelé, de nálunk az Y, tehát a legtöbb képlethez képest nálunk
	//				az Y és Z koordináták felcserélve szerepelnek
	u *= 3.1415f;
	v *= 3.1415f;
	float cu = cosf(u), su = sinf(u), cv = cosf(v), sv = sinf(v);
	float r = 2;

	return glm::vec3(r*cu*sv, r*cv, r*su*sv);
}
//------------------------------------------------------------------------
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
//---------------------------------------------------------------------------
glm::vec3 CMyApp::GetTalajPos(float u, float v)
{
	return glm::vec3(32 * u - 16, 0, 32 * v - 16);
}

glm::vec3 CMyApp::GetTalajNorm(float u, float v)
{
	// Numerikusan (nem kell ismerni a képletet, elég a pozícióét)
	glm::vec3 uv = GetTalajPos(u, v);
	glm::vec3 du = GetTalajPos(u + 0.01, v) - GetTalajPos(u - 0.01, v);
	glm::vec3 dv = GetTalajPos(u, v + 0.01) - GetTalajPos(u, v - 0.01);

	return (glm::cross(du, dv));
}

glm::vec2 CMyApp::GetTalajTex(float u, float v)
{
	return glm::vec2(u, 1 - v);
}

//---------------------------------------------------------------------------
bool CMyApp::Init()
{
	// törlési szín legyen fehér
	glClearColor(1,1,1, 1);

	//glEnable(GL_CULL_FACE); // kapcsoljuk be a hatrafele nezo lapok eldobasat
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

	korlap_program.AttachShaders({
		{ GL_VERTEX_SHADER, "korlap.vert" },
		{ GL_FRAGMENT_SHADER, "korlap.frag" }
		});

	// attributomok osszerendelese a VAO es shader kozt
	korlap_program.BindAttribLocations({
		{ 0, "vs_in_pos" },				// VAO 0-as csatorna menjen a vs_in_pos-ba
		{ 1, "vs_in_col" }				// VAO 1-es csatorna menjen a vs_in_col-ba
		});

	korlap_program.LinkProgram();

	szellem_program.AttachShaders({
		{ GL_VERTEX_SHADER, "szellem.vert" },
		{ GL_FRAGMENT_SHADER, "szellem.frag" }
		});

	// attributomok osszerendelese a VAO es shader kozt
	szellem_program.BindAttribLocations({
		{ 0, "vs_in_pos" },				// VAO 0-as csatorna menjen a vs_in_pos-ba
		{ 1, "vs_in_col" }				// VAO 1-es csatorna menjen a vs_in_col-ba
		});

	szellem_program.LinkProgram();

	m_talajProgram.Init({
		{ GL_VERTEX_SHADER, "talaj.vert" },
		{ GL_FRAGMENT_SHADER, "talaj.frag" }
		},
	{
		{ 0, "vs_in_pos" },					// VAO 0-as csatorna menjen a vs_in_pos-ba
		{ 1, "vs_in_normal" },				// VAO 1-es csatorna menjen a vs_in_normal-ba
		{ 2, "vs_out_tex0" },				// VAO 2-es csatorna menjen a vs_in_tex0-ba
	});

	m_talajProgram.LinkProgram();
	//-------------------------------------------------------------------------------
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
	//-------------------------------------------------------------------------------
	//szellemekhez
	szellem_BufferPos.BufferData(
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
			//gúla
			glm::vec3(0, -2, 0),

	}
	);

	// és a primitíveket alkotó csúcspontok indexei (az előző tömbökből) - triangle list-el való kirajzolásra felkészülve
	szellem_BufferIndices.BufferData(
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
			// felső
			/*1, 0, 4,
			1, 4, 5,*/
			// alsó
			3, 2, 6,
			3, 6, 7,
			//gúla
			1, 0, 8,
			5, 1, 8,
			4, 5, 8,
			0, 4, 8,
	}
	);
	//--------------------------------------------------------------
	//talaj
	std::vector<glm::vec3> talajPos;
	std::vector<glm::vec3> talajNorm;
	std::vector<glm::vec2> talajText;
	std::vector<int> talajInd;
	//Vertex vert[(N + 1)*(M + 1)];
	for (int i = 0; i <= I; ++i)
		for (int j = 0; j <= J; ++j)
		{
			float u = i / (float)I;
			float v = j / (float)J;

			talajPos.push_back(GetTalajPos(u, v));
			talajNorm.push_back(GetTalajNorm(u, v));
			talajText.push_back(GetTalajTex(u, v));
		}

	//GLushort indices[3 * 2 * (N)*(M)];
	for (int i = 0; i < I; ++i)
		for (int j = 0; j < J; ++j)
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
			talajInd.push_back((i)+(j)*	(I + 1));
			talajInd.push_back((i + 1) + (j)*	(I + 1));
			talajInd.push_back((i)+(j + 1)*(I + 1));
			talajInd.push_back((i + 1) + (j)*	(I + 1));
			talajInd.push_back((i + 1) + (j + 1)*(I + 1));
			talajInd.push_back((i)+(j + 1)*(I + 1));
		}

	talaj_gpuBufferPos.BufferData(talajPos);
	talaj_gpuBufferNormal.BufferData(talajNorm);
	talaj_gpuBufferTex.BufferData(talajText);
	talaj_gpuBufferIndices.BufferData(talajInd);

	//--------------------------------------------------------------
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

	// geometria VAO-ban való regisztrálása
	m_vao.Init(
		{
			{ CreateAttribute<0, glm::vec3, 0, sizeof(glm::vec3)>, m_gpuBufferPos },	// 0-ás attribútum "lényegében" glm::vec3-ak sorozata és az adatok az m_gpuBufferPos GPU pufferben vannak
			{ CreateAttribute<1, glm::vec3, 0, sizeof(glm::vec3)>, m_gpuBufferNormal },	// 1-es attribútum "lényegében" glm::vec3-ak sorozata és az adatok az m_gpuBufferNormal GPU pufferben vannak
			{ CreateAttribute<2, glm::vec2, 0, sizeof(glm::vec2)>, m_gpuBufferTex }		// 2-es attribútum "lényegében" glm::vec2-ők sorozata és az adatok az m_gpuBufferTex GPU pufferben vannak
		},
		m_gpuBufferIndices
	);
//-------------------------------------------------------------------------------------------------
	gomb_vao.Init({
		{ CreateAttribute<0, glm::vec3, 0, sizeof(glm::vec3)>, gomb_BufferPos },
		{ CreateAttribute<1, glm::vec3, 0, sizeof(glm::vec3)>, gomb_BufferNorm },
		{ CreateAttribute<2, glm::vec2, 0, sizeof(glm::vec2)>, gomb_BufferText },
		},
		gomb_BufferIndices
		);

	//-------------------------------------------------------------------------------
	std::vector<glm::vec3> korlapPos;
	std::vector<int> korlapInd;

	korlapPos.push_back(glm::vec3(0, 0, 0));

	int r = 2;

	for (int i = 0; i < 40; i++) {
		korlapPos.push_back(glm::vec3(r * sinf(((float)i * (360 / 40)) * (M_PI / 180)), 0, r * cosf(((float)i * (360 / 40)) * (M_PI / 180))));
		//if (i <= N) {
		korlapInd.push_back(i);
		//}
	}

	korlapInd.push_back(40);
	korlapInd.push_back(1);

	korlap_BufferPos.BufferData(korlapPos);
	korlap_BufferIndices.BufferData(korlapInd);


	korlap_vao.Init({
		{
			CreateAttribute<0, glm::vec3, 0, sizeof(glm::vec3)>, korlap_BufferPos
		},
		},
		korlap_BufferIndices
	);

	//-------------------------------------------------------------------------------
	szellem_vao.Init(
		{
			// 0-ás attribútum "lényegében" glm::vec3-ak sorozata és az adatok az m_gpuBufferPos GPU pufferben vannak
			{ CreateAttribute<		0,						// csatorna: 0
			glm::vec3,				// CPU oldali adattípus amit a 0-ás csatorna attribútumainak meghatározására használtunk <- az eljárás a glm::vec3-ból kikövetkezteti, hogy 3 darab float-ból áll a 0-ás attribútum
			0,						// offset: az attribútum tároló elejétől vett offset-je, byte-ban
			sizeof(glm::vec3)		// stride: a következő csúcspont ezen attribútuma hány byte-ra van az aktuálistól
			>, szellem_BufferPos },
		},
		szellem_BufferIndices
	);

	talaj_vao.Init(
		{
			{ CreateAttribute<0, glm::vec3, 0, sizeof(glm::vec3)>, talaj_gpuBufferPos },	// 0-ás attribútum "lényegében" glm::vec3-ak sorozata és az adatok az m_gpuBufferPos GPU pufferben vannak
		{ CreateAttribute<1, glm::vec3, 0, sizeof(glm::vec3)>, talaj_gpuBufferNormal },	// 1-es attribútum "lényegében" glm::vec3-ak sorozata és az adatok az m_gpuBufferNormal GPU pufferben vannak
		{ CreateAttribute<2, glm::vec2, 0, sizeof(glm::vec2)>, talaj_gpuBufferTex }		// 2-es attribútum "lényegében" glm::vec2-ők sorozata és az adatok az m_gpuBufferTex GPU pufferben vannak
		},
		talaj_gpuBufferIndices
	);
	//-------------------------------------------------------------------------------

	// textúra betöltése
	m_textureMetal.FromFile("texture.png");
	m_textureMetalGomb.FromFile("yellow.jpg");

	// mesh betöltése
	//m_mesh = ObjParser::parse("suzanne.obj");

	// kamera
	m_camera.SetProj(45.0f, 640.0f / 480.0f, 0.01f, 1000.0f);


	// véletlenszám generátor inicializálása
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> rnd(-16, 16);

	for (int i = 0; i < m_szellemCount; ++i)
	{
		m_szellemPos.push_back(glm::vec3(rnd(gen), 2.f, rnd(gen)));
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

	//saját talaj
	m_talajProgram.Use();

	m_talajProgram.SetTexture("texImage", 0, m_textureMetal);

	glm::mat4 talajWorld = glm::translate(glm::vec3(0, 0, 0));
	m_talajProgram.SetUniform("world", talajWorld);
	m_talajProgram.SetUniform("worldIT", glm::transpose(glm::inverse(talajWorld)));
	m_talajProgram.SetUniform("MVP", m_camera.GetViewProj()*talajWorld);
	m_talajProgram.SetUniform("Kd", glm::vec4(1, 1, 1, 1));

	talaj_vao.Bind();
	glDrawElements(GL_TRIANGLES, 3 * 2 * I * J, GL_UNSIGNED_INT, 0);
	talaj_vao.Unbind(); // nem feltétlen szükséges: a m_mesh->draw beállítja a saját VAO-ját
	//----------------------------------------------------------------	
	//gömb
	float t = SDL_GetTicks() / 1000.0f;

	float x = -1 * ((15 * cosf(6 * hammTime * t)) + 15);

	gomb_vao.Bind();

	gomb_program.Use();

	gomb_program.SetTexture("texImage2", 0, m_textureMetalGomb);

	glm::mat4 gombWorld = glm::translate(glm::vec3(pacx, 2, pacz)) * glm::rotate(glm::radians(pFace), glm::vec3(0, 1, 0)) * glm::rotate(glm::radians(x), glm::vec3(1, 0, 0)) * glm::rotate(glm::radians(90.f), glm::vec3(1, 0, 0))* glm::scale(glm::vec3(-1, -1, -1));

	gomb_program.SetUniform("world", gombWorld);
	gomb_program.SetUniform("worldIT", glm::transpose(glm::inverse(gombWorld)));
	gomb_program.SetUniform("Kd", glm::vec4(1, 0.3, 0.3, 1));
	gomb_program.SetUniform("MVP", m_camera.GetViewProj()*gombWorld);
	glDrawElements(GL_TRIANGLES, 3 * 2 * (N)*(M), GL_UNSIGNED_INT, nullptr);


	//----------------------------------------------------------------
	//körlap
	korlap_vao.Bind();

	korlap_program.Use();

	gomb_program.SetTexture("texImage", 0, m_textureMetalGomb);

	glm::mat4 korlapWorld = glm::translate(glm::vec3(pacx, 2, pacz)) * glm::rotate(glm::radians(pFace), glm::vec3(0, 1, 0)) * glm::rotate(glm::radians(x), glm::vec3(1, 0, 0));

	korlap_program.SetUniform("world", korlapWorld);
	korlap_program.SetUniform("worldIT", glm::transpose(glm::inverse(korlapWorld)));
	korlap_program.SetUniform("Kd", glm::vec4(1, 0.3, 0.3, 1));
	korlap_program.SetUniform("MVP", m_camera.GetViewProj()*korlapWorld);
	glDrawElements(GL_TRIANGLE_FAN, 40 + 2, GL_UNSIGNED_INT, nullptr);

	//----------------------------------------------------------------
	//masodikok
	gomb_vao.Bind();

	gomb_program.Use();

	gomb_program.SetTexture("texImage2", 0, m_textureMetalGomb);

	gombWorld = glm::translate(glm::vec3(pacx, 2, pacz)) * glm::rotate(glm::radians(pFace), glm::vec3(0, 1, 0)) * glm::rotate(glm::radians(180.f), glm::vec3(0, 0, 1)) * glm::rotate(glm::radians(x), glm::vec3(1, 0, 0)) * glm::rotate(glm::radians(90.f), glm::vec3(1, 0, 0))* glm::scale(glm::vec3(-1, -1, -1));

	gomb_program.SetUniform("world", gombWorld);
	gomb_program.SetUniform("worldIT", glm::transpose(glm::inverse(gombWorld)));
	gomb_program.SetUniform("Kd", glm::vec4(1, 0.3, 0.3, 1));
	gomb_program.SetUniform("MVP", m_camera.GetViewProj()*gombWorld);
	glDrawElements(GL_TRIANGLES, 3 * 2 * (N)*(M), GL_UNSIGNED_INT, nullptr);


	//----------------------------------------------------------------
	//körlap
	korlap_vao.Bind();

	korlap_program.Use();

	gomb_program.SetTexture("texImage", 0, m_textureMetalGomb);

	korlapWorld = glm::translate(glm::vec3(pacx, 2, pacz)) * glm::rotate(glm::radians(pFace), glm::vec3(0, 1, 0)) * glm::rotate(glm::radians(180.f), glm::vec3(0, 0, 1)) * glm::rotate(glm::radians(x), glm::vec3(1, 0, 0));

	korlap_program.SetUniform("world", korlapWorld);
	korlap_program.SetUniform("worldIT", glm::transpose(glm::inverse(korlapWorld)));
	korlap_program.SetUniform("Kd", glm::vec4(1, 0.3, 0.3, 1));
	korlap_program.SetUniform("MVP", m_camera.GetViewProj()*korlapWorld);
	glDrawElements(GL_TRIANGLE_FAN, 40 + 2, GL_UNSIGNED_INT, nullptr);

	//-------------------------------------------------------------------
	//szellemek
	// kocka
	szellem_vao.Bind();

	szellem_program.Use();
	// véletlenszám generátor inicializálása
	int atlagX = 0;
	int atlagZ = 0;

	for (int i = 0; i < m_szellemCount; ++i) {
		glm::mat4 szellemWorld = glm::translate(m_szellemPos.at(i)) * glm::rotate(glm::radians(180.f), glm::vec3(1, 0, 0)) * glm::rotate(glm::radians(t * 80.f), glm::vec3(0, 1, 0)) * glm::rotate(glm::radians(180.f), glm::vec3(1, 0, 0)) * glm::scale(glm::vec3(-1, -1, -1));
		szellem_program.SetUniform("MVP", m_camera.GetViewProj()*szellemWorld);
		szellem_program.SetUniform("time", t);

		atlagX += m_szellemPos.at(i).x;
		atlagZ += m_szellemPos.at(i).z;

		glDrawElements(GL_TRIANGLES, 42, GL_UNSIGNED_INT, nullptr);
	}

	glm::vec3 lightpos = glm::vec3(atlagX/m_szellemCount, 5, atlagZ / m_szellemCount);
	gomb_program.SetUniform("light_pos", lightpos);
	//-------------------------------------------------------------------

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
	if (ImGui::Begin("Pac-Man"))	
	{

		ImGui::SliderInt("Pac-Man hamm-hamm", &hammTime, 1, 8);

	}
	ImGui::End(); // ...de még ha le is volt, End()-et hívnunk kell
}

void CMyApp::KeyboardDown(SDL_KeyboardEvent& key)
{
	m_camera.KeyboardDown(key);

	bool b = false;

	if (key.keysym.sym == SDLK_UP && pacz > -16) {//fel
		pacz--;
		pFace = 180.f;
		b = !b;
		pacStepCount++;
	}
	else if (key.keysym.sym == SDLK_DOWN && pacz < 16) {//le
		pacz++;
		pFace = 0.f;
		b = !b;
		pacStepCount++;
	}
	else if (key.keysym.sym == SDLK_LEFT && pacx > -16) {//bal
		pacx--;
		pFace = 270.f;
		b = !b;
		pacStepCount++;
	}
	else if (key.keysym.sym == SDLK_RIGHT && pacx < 16) {//jobb
		pacx++;
		pFace = 90.f;
		b = !b;
		pacStepCount++;
	}

	if (pacStepCount % 5 == 0 && pacStepCount != 0 && b) {
		m_szellemCount++;
		// véletlenszám generátor inicializálása
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<> rnd(-16, 16);
		m_szellemPos.push_back(glm::vec3(rnd(gen), 2.f, rnd(gen)));
	}

	int lepesx;
	int lepesz;

	if (b) {
		for (int i = 0; i < m_szellemCount; i++) {
			lepesx = rand() % 2;
			lepesz = rand() % 2;
			if (lepesx == 0 && m_szellemPos.at(i).x > -16) {
				m_szellemPos.at(i).x--;
			}
			else if (lepesx == 1 && m_szellemPos.at(i).x < 16) {
				m_szellemPos.at(i).x++;
			}

			if (lepesz == 0 && m_szellemPos.at(i).z > -16) {
				m_szellemPos.at(i).z--;
			}
			else if (lepesz == 1 && m_szellemPos.at(i).z < 16) {
				m_szellemPos.at(i).z++;
			}
		}
	}



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
