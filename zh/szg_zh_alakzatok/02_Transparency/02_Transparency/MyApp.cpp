#include "MyApp.h"

#include <math.h>
#include <vector>

#include <array>
#include <list>
#include <tuple>

#include "imgui\imgui.h"

#include "ObjParser_OGL3.h"

const float f_PI = 3.14159265359f;

CMyApp::CMyApp(void)
{
}


CMyApp::~CMyApp(void)
{
	
}

glm::vec3 CMyApp::GetUV(float u, float v) {
	u *= 2 * f_PI;
	v *= f_PI;
	float cu = cosf(u), su = sinf(u), cv = cosf(v), sv = sinf(v);
	return glm::vec3(cu*sv, cv, su*sv);
}

glm::vec3 CMyApp::GetUV_henger(float u, float v) {
	u *= 2 * f_PI;
	//v *= f_PI;
	static const float h = 3.0f;
	float cu = cosf(u), su = sinf(u), cv = cosf(v), sv = sinf(v);
	return glm::vec3(2*cu, 2*su, h*v);
}

bool CMyApp::Init()
{
	// törlési szín legyen fehér
	glClearColor(1,1,1, 1);

	glEnable(GL_CULL_FACE); // kapcsoljuk be a hatrafele nezo lapok eldobasat
	glEnable(GL_DEPTH_TEST); // mélységi teszt bekapcsolása (takarás)
	//glDepthMask(GL_FALSE); // kikapcsolja a z-pufferbe történő írást - https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/glDepthMask.xml

	// átlátszóság engedélyezése
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // meghatározza, hogy az átlátszó objektum az adott pixelben hogyan módosítsa a korábbi fragmentekből oda lerakott színt: https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glBlendFunc.xhtml

	//
	// shaderek betöltése
	//

	// így is létre lehetne hozni:

	// a shadereket tároló program létrehozása
	m_program.Init({
		{ GL_VERTEX_SHADER, "myVert.vert" },
		{ GL_FRAGMENT_SHADER, "myFrag.frag" }
	},
	{
		{ 0, "vs_in_pos" },					// VAO 0-as csatorna menjen a vs_in_pos-ba
		{ 1, "vs_in_normal" },				// VAO 1-es csatorna menjen a vs_in_normal-ba
		{ 2, "vs_out_tex0" },				// VAO 2-es csatorna menjen a vs_in_tex0-ba
	});

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
			glm::vec3(-10,0,-10),	// 0-ás csúcspont
			glm::vec3(10, 0,-10), // 1-es
			glm::vec3(-10,0, 10), // 2-es
			glm::vec3(10, 0, 10)  // 3-as
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
			1,0,2,	// 1. háromszög: 0-1-2 csúcspontokból
			1,2,3	// 2. háromszög: 2-1-3 csúcspontokból
		}
	);

	// geometria VAO-ban való regisztrálása
	m_vao_ground.Init(
		{
			{ CreateAttribute<0, glm::vec3, 0, sizeof(glm::vec3)>, m_gpuBufferPos },	// 0-ás attribútum "lényegében" glm::vec3-ak sorozata és az adatok az m_gpuBufferPos GPU pufferben vannak
			{ CreateAttribute<1, glm::vec3, 0, sizeof(glm::vec3)>, m_gpuBufferNormal },	// 1-es attribútum "lényegében" glm::vec3-ak sorozata és az adatok az m_gpuBufferNormal GPU pufferben vannak
			{ CreateAttribute<2, glm::vec2, 0, sizeof(glm::vec2)>, m_gpuBufferTex }		// 2-es attribútum "lényegében" glm::vec2-ők sorozata és az adatok az m_gpuBufferTex GPU pufferben vannak
		},
		m_gpuBufferIndices
	);

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// GÖMB
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	m_orbPos.reserve(N*M);
	m_orbNorm.reserve(N*M);
	m_orbTex.reserve(N*M);
	m_orbTriangles.reserve(N*M);

	// N*M-es rács bejárása, a vertexek (x,y,z) koordinátáját a GetUV függvénnyel kapjuk
	for (int i = 0; i <= N; ++i) {
		for (int j = 0; j <= M; ++j) {
			float u = i / (float)N;
			float v = j / (float)M;

			// pozíció: gömbi koordináták
			glm::vec3 current_pos = GetUV(u, v);
			m_orbPos.push_back(current_pos);
			// szerencsékre gömb esetén ugyanaz a normálvektor:
			// az origóból a gömbfelszín irányába mutató irányvektor
			// de ez csak a gömbnél van így! (Lásd fentebb a csonkakúp palást, ott másképp kellett számolnunk!)
			glm::vec3 current_norm = GetUV(u, v);
			m_orbNorm.push_back(current_norm);
			// a textúra-koordináták megyeznek az (u,v) pont koordinátájával
			// (kivéve, hogy a kép rossz irányba állna)
			m_orbTex.push_back(glm::vec2(1 - u, v));
		}
	}
	
	for (int i = 0; i < N; ++i) {
		for (int j = 0; j < M; ++j) {
			// az egyik háromszög a négyzet egyik fele
			m_orbTriangles.push_back((i)+(j)*		(N + 1));
			m_orbTriangles.push_back((i)+(j + 1) *	(N + 1));
			m_orbTriangles.push_back((i + 1) + (j) *		(N + 1));
			// a másik háromszög a négyzet másik fele
			m_orbTriangles.push_back((i + 1) + (j) *		(N + 1));
			m_orbTriangles.push_back((i)+(j + 1) *	(N + 1));
			m_orbTriangles.push_back((i + 1) + (j + 1) *	(N + 1));
		}
	}

	m_orbBufferPos.BufferData(m_orbPos);
	m_orbBufferNormal.BufferData(m_orbNorm);
	m_orbBufferTex.BufferData(m_orbTex);
	m_orbBufferIndices.BufferData(m_orbTriangles);

	m_vao_orb.Init(
	{
		{ CreateAttribute<0, glm::vec3, 0, sizeof(glm::vec3)>, m_orbBufferPos },	// 0-ás attribútum "lényegében" glm::vec3-ak sorozata és az adatok az m_gpuBufferPos GPU pufferben vannak
		{ CreateAttribute<1, glm::vec3, 0, sizeof(glm::vec3)>, m_orbBufferNormal },	// 1-es attribútum "lényegében" glm::vec3-ak sorozata és az adatok az m_gpuBufferNormal GPU pufferben vannak
		{ CreateAttribute<2, glm::vec2, 0, sizeof(glm::vec2)>, m_orbBufferTex }		// 2-es attribútum "lényegében" glm::vec2-ők sorozata és az adatok az m_gpuBufferTex GPU pufferben vannak
	},
		m_orbBufferIndices
	);

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// GÖMB VÉGE
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// CSONKAKÚP PALÁST
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	m_conePos.reserve(N);
	m_coneNorm.reserve(N);
	m_coneTex.reserve(N);
	m_coneTriangles.reserve(N);

	// a csonkakúp paraméterei: alsó körvonal sugara, felsö körvonal sugara, magasság
	// a csonkakúp az XZ síkon fog állni
	float r_low = 4;
	float r_high = 2;
	float h = 6;

	// 2*(N+1) vertexet definiálunk egy körív mentén:
	// felváltva rakunk egy vertexet lentre, egyet fentre
	for (int i = 0; i <= N; ++i) {

		float a = i * 2 * f_PI / N; // szög

									// alsó vertexek y = 0 magasságban, r_low sugarú körír mentén,
									// felső vertexek y = h magasságban, r_high sugarú körív mentén
									// (a körívek origó középpontúak, XZ síkon fekszenek)
		m_conePos.push_back(glm::vec3(r_low*cosf(a), 0, r_low*sinf(a)));
		m_conePos.push_back(glm::vec3(r_high*cosf(a), h, r_high*sinf(a)));

		// kiszámoljuk a kúppalástot alkotó vertexek normálisát:
		// középponttól kifele mutatnak (X és Z), de Y irányban enyhén ferdén állnak,
		// a csonkakúp palást ferdeségétől függően
		glm::vec3 n = glm::normalize(glm::vec3(cosf(a), (r_low - r_high) / h, sinf(a)));
		m_coneNorm.push_back(n);
		m_coneNorm.push_back(n);
		// a textúrakoordináták: a textúrát sávokra bonta húzzúk rá a palástunkra
		m_coneTex.push_back(glm::vec2(2 * (float)i / N, 1));
		m_coneTex.push_back(glm::vec2(2 * (float)i / N, 0));

	}

	// vertex indexek a háromszögekhez
	// palást = N darab négyzet, egy négyzet = két háromszög
	for (int i = 0; i < N; ++i) {
		m_coneTriangles.push_back(2 * i);
		m_coneTriangles.push_back(2 * i + 1);
		m_coneTriangles.push_back(2 * i + 2);
		m_coneTriangles.push_back(2 * i + 1);
		m_coneTriangles.push_back(2 * i + 3);
		m_coneTriangles.push_back(2 * i + 2);
	}

	m_coneBufferPos.BufferData(m_conePos);
	m_coneBufferNormal.BufferData(m_coneNorm);
	m_coneBufferTex.BufferData(m_coneTex);
	m_coneBufferIndices.BufferData(m_coneTriangles);

	m_vao_cone.Init(
	{
		{ CreateAttribute<0, glm::vec3, 0, sizeof(glm::vec3)>, m_coneBufferPos },	// 0-ás attribútum "lényegében" glm::vec3-ak sorozata és az adatok az m_gpuBufferPos GPU pufferben vannak
		{ CreateAttribute<1, glm::vec3, 0, sizeof(glm::vec3)>, m_coneBufferNormal },	// 1-es attribútum "lényegében" glm::vec3-ak sorozata és az adatok az m_gpuBufferNormal GPU pufferben vannak
		{ CreateAttribute<2, glm::vec2, 0, sizeof(glm::vec2)>, m_coneBufferTex }		// 2-es attribútum "lényegében" glm::vec2-ők sorozata és az adatok az m_gpuBufferTex GPU pufferben vannak
	},
		m_coneBufferIndices
	);

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// CSONKAKÚP PALÁST VÉGE
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// CSONKAKÚP TETEJE: KÖRLAP
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	m_conetopPos.reserve(N);
	m_conetopNorm.reserve(N);
	m_conetopTex.reserve(N);
	m_conetopTriangles.reserve(N);

	m_conetopPos.push_back(glm::vec3(0, h, 0));
	m_conetopNorm.push_back(glm::vec3(0, 1, 0));
	m_conetopTex.push_back(glm::vec2(0.5f, 0.5f));

	// N+1 vertex a a körvonalon
	for (int i = 0; i <= N; ++i) {

		float a = i * 2 * f_PI / N;
		// y = h magasságban, r_high sugarú körvonal mentén
		m_conetopPos.push_back(glm::vec3(r_high * cosf(a), h, r_high * sinf(a)));
		// a normálvektorok minden vextex esetén felfelé mutatnak
		m_conetopNorm.push_back(glm::vec3(1, 0, 1));
		// a textúra közepéből kivágunk egy 0.5 sugarú körlapot
		m_conetopTex.push_back(glm::vec2(0.5 * cosf(a) + 0.5, 0.5 * sinf(a) + 0.5));
	}
	// indexek a körlap megalkotásához (N darab háromszög)
	for (int i = 0; i < N; ++i) {
		m_conetopTriangles.push_back(0);
		m_conetopTriangles.push_back(i + 2);
		m_conetopTriangles.push_back(i + 1);
	}

	m_conetopBufferPos.BufferData(m_conetopPos);
	m_conetopBufferNormal.BufferData(m_conetopNorm);
	m_conetopBufferTex.BufferData(m_conetopTex);
	m_conetopBufferIndices.BufferData(m_conetopTriangles);

	m_vao_cone_top.Init(
	{
		{ CreateAttribute<0, glm::vec3, 0, sizeof(glm::vec3)>, m_conetopBufferPos },	// 0-ás attribútum "lényegében" glm::vec3-ak sorozata és az adatok az m_gpuBufferPos GPU pufferben vannak
		{ CreateAttribute<1, glm::vec3, 0, sizeof(glm::vec3)>, m_conetopBufferNormal },	// 1-es attribútum "lényegében" glm::vec3-ak sorozata és az adatok az m_gpuBufferNormal GPU pufferben vannak
		{ CreateAttribute<2, glm::vec2, 0, sizeof(glm::vec2)>, m_conetopBufferTex }		// 2-es attribútum "lényegében" glm::vec2-ők sorozata és az adatok az m_gpuBufferTex GPU pufferben vannak
	},
		m_conetopBufferIndices
	);

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// CSONKAKÚP TETEJE: KÖRLAP VÉGE
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// **************************
	// HENGER PALÁST
	// **************************

	m_cylinderPos.reserve(N);
	m_cylinderNorm.reserve(N);
	m_cylinderTex.reserve(N);
	m_cylinderTriangles.reserve(N);

	// a ciklusmagban két vertexet definiálunk: egyet rakunk alulra, egyet felülre
	// a köríven side_num+1 vertexet rakunk le (az elsőt és az utolsót ugyanoda)
	// így összesen 2*(side_num+1) vertexünk lesz
	for (int i = 0; i <= N; ++i) {
		float x = cosf(i * 2 * f_PI / N);
		float z = sinf(i * 2 * f_PI / N);
		// pozíciók: -0.5 és +0.5 magasan lerakunk egymás alá két vertexet
		m_cylinderPos.push_back(glm::vec3(x, -0.5f,z));
		m_cylinderPos.push_back(glm::vec3(x, 0.5,z));
		// egy XZ síkon álló hengerpalást esetében a normálvektor egyenesen áll (Y) és kifelé mutat (XZ)
		// hengerpalástnál az alsó és a felső vertex esetében persze ugyanaz a normálvektor
		m_cylinderNorm.push_back(glm::vec3(x, 0, z));
		m_cylinderNorm.push_back(glm::vec3(x, 0, z));
		// textúrakoordináták: a textúra alját illetve tetejét járjuk be,
		// így a textúrát körbehúzzuk a teljes hengerpaláston
		m_cylinderTex.push_back(glm::vec2((float)i / N, 0));
		m_cylinderTex.push_back(glm::vec2((float)i / N, 1));
		// kirajzolásnál majd TRIANGLE_STRIP-et használunk, így igazából az indexek
		// úgy jönnek, ahogy maguk a vertexek: 0, 1, 2, 3, 4, 5, 6, ..., 2*(side_num+1)
		m_cylinderTriangles.push_back(2 * i);
		m_cylinderTriangles.push_back(2 * i + 1);
	}

	m_cylinderBufferPos.BufferData(m_cylinderPos);
	m_cylinderBufferNormal.BufferData(m_cylinderNorm);
	m_cylinderBufferTex.BufferData(m_cylinderTex);
	m_cylinderBufferIndices.BufferData(m_cylinderTriangles);

	m_vao_cylinder.Init(
	{
		{ CreateAttribute<0, glm::vec3, 0, sizeof(glm::vec3)>, m_cylinderBufferPos },	// 0-ás attribútum "lényegében" glm::vec3-ak sorozata és az adatok az m_gpuBufferPos GPU pufferben vannak
		{ CreateAttribute<1, glm::vec3, 0, sizeof(glm::vec3)>, m_cylinderBufferNormal },	// 1-es attribútum "lényegében" glm::vec3-ak sorozata és az adatok az m_gpuBufferNormal GPU pufferben vannak
		{ CreateAttribute<2, glm::vec2, 0, sizeof(glm::vec2)>, m_cylinderBufferTex }		// 2-es attribútum "lényegében" glm::vec2-ők sorozata és az adatok az m_gpuBufferTex GPU pufferben vannak
	},
		m_cylinderBufferIndices
	);

	// **************************
	// HENGER PALÁST VÉGE
	// **************************
	
	// **************************
	// HENGER TETEJE
	// **************************

	m_cylindertopPos.reserve(N);
	m_cylindertopNorm.reserve(N);
	m_cylindertopTex.reserve(N);
	m_cylindertopTriangles.reserve(N);

	m_cylindertopPos.push_back(glm::vec3(0, 0.5f, 0));
	m_cylindertopNorm.push_back(glm::vec3(0,1,0));
	m_cylindertopTex.push_back(glm::vec2(0.5f, 0.5f));
	m_cylindertopTriangles.push_back(0);

	for (int i = 0; i <= N; ++i) {
		float x = cosf(-i * 2 * M_PI / N);
		float z = sinf(-i * 2 * M_PI / N);
		m_cylindertopPos.push_back(glm::vec3(x, 0.5f, z));
		m_cylindertopNorm.push_back(glm::vec3(0, 1, 0));
		m_cylindertopTex.push_back(glm::vec2(0.5f*x + 0.5f, 0.5f*z + 0.5f));
		m_cylindertopTriangles.push_back(i + 1);
	}

	m_cylindertopBufferPos.BufferData(m_cylindertopPos);
	m_cylindertopBufferNormal.BufferData(m_cylindertopNorm);
	m_cylindertopBufferTex.BufferData(m_cylindertopTex);
	m_cylindertopBufferIndices.BufferData(m_cylindertopTriangles);

	m_vao_cylinder_top.Init(
	{
		{ CreateAttribute<0, glm::vec3, 0, sizeof(glm::vec3)>, m_cylindertopBufferPos },	// 0-ás attribútum "lényegében" glm::vec3-ak sorozata és az adatok az m_gpuBufferPos GPU pufferben vannak
		{ CreateAttribute<1, glm::vec3, 0, sizeof(glm::vec3)>, m_cylindertopBufferNormal },	// 1-es attribútum "lényegében" glm::vec3-ak sorozata és az adatok az m_gpuBufferNormal GPU pufferben vannak
		{ CreateAttribute<2, glm::vec2, 0, sizeof(glm::vec2)>, m_cylindertopBufferTex }		// 2-es attribútum "lényegében" glm::vec2-ők sorozata és az adatok az m_gpuBufferTex GPU pufferben vannak
	},
		m_cylindertopBufferIndices
	);
	

	// **************************
	// HENGER TETEJE VÉGE
	// **************************
	
	// **************************
	// HENGER ALJA
	// **************************
	
	m_cylinderbottomPos.reserve(N);
	m_cylinderbottomNorm.reserve(N);
	m_cylinderbottomTex.reserve(N);
	m_cylinderbottomTriangles.reserve(N);

	m_cylindertopPos.push_back(glm::vec3(0, -0.5f, 0));
	m_cylindertopNorm.push_back(glm::vec3(0, -1, 0));
	m_cylindertopTex.push_back(glm::vec2(0.5f, 0.5f));
	m_cylindertopTriangles.push_back(0);

	for (int i = 0; i <= N; ++i) {
		float x = cosf(i * 2 * M_PI / N);
		float z = sinf(i * 2 * M_PI / N);
		m_cylinderbottomPos.push_back(glm::vec3(x, -0.5f, z));
		m_cylinderbottomNorm.push_back(glm::vec3(0, -1, 0));
		m_cylinderbottomTex.push_back(glm::vec2(0.5f*x + 0.5f, 0.5f*z + 0.5f));
		m_cylinderbottomTriangles.push_back(i + 1);
	}

	m_cylinderbottomBufferPos.BufferData(m_cylinderbottomPos);
	m_cylinderbottomBufferNormal.BufferData(m_cylinderbottomNorm);
	m_cylinderbottomBufferTex.BufferData(m_cylinderbottomTex);
	m_cylinderbottomBufferIndices.BufferData(m_cylinderbottomTriangles);

	m_vao_cylinder_bottom.Init(
	{
		{ CreateAttribute<0, glm::vec3, 0, sizeof(glm::vec3)>, m_cylinderbottomBufferPos },	// 0-ás attribútum "lényegében" glm::vec3-ak sorozata és az adatok az m_gpuBufferPos GPU pufferben vannak
		{ CreateAttribute<1, glm::vec3, 0, sizeof(glm::vec3)>, m_cylinderbottomBufferNormal },	// 1-es attribútum "lényegében" glm::vec3-ak sorozata és az adatok az m_gpuBufferNormal GPU pufferben vannak
		{ CreateAttribute<2, glm::vec2, 0, sizeof(glm::vec2)>, m_cylinderbottomBufferTex }		// 2-es attribútum "lényegében" glm::vec2-ők sorozata és az adatok az m_gpuBufferTex GPU pufferben vannak
	},
		m_cylinderbottomBufferIndices
	);
	// **************************
	// HENGER ALJA VÉGE
	// **************************
	
	// textúra betöltése
	m_textureCone.FromFile("cylinder_texture.bmp");
	m_textureMetal.FromFile("texture.png");
	m_textureOrb.FromFile("gomb_texture.bmp");
	m_textureConeTop.FromFile("top_texture.bmp");
	m_textureCylinder.FromFile("hilt.png");

	// mesh betöltése
	m_mesh = ObjParser::parse("suzanne.obj");

	// kamera
	m_camera.SetProj(45.0f, 640.0f / 480.0f, 0.01f, 1000.0f);

	//skybox

	// vertexek pozíciói:
	/*
	Az m_gpuBufferPos konstruktora már létrehozott egy GPU puffer azonosítót és a most következő BufferData hívás ezt
	1. bind-olni fogja GL_ARRAY_BUFFER target-re (hiszen m_gpuBufferPos típusa ArrayBuffer) és
	2. glBufferData segítségével áttölti a GPU-ra az argumentumban adott tároló értékeit

	*/
	m_skyboxVBO.BufferData(
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
	});

	// és a primitíveket alkotó csúcspontok indexei (az előző tömbökből) - triangle list-el való kirajzolásra felkészülve
	m_skyboxIBO.BufferData(
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
	});

	// geometria VAO-ban való regisztrálása
	m_skyboxVAO.Init(
	{
		// 0-ás attribútum "lényegében" glm::vec3-ak sorozata és az adatok az m_gpuBufferPos GPU pufferben vannak
		{ CreateAttribute<		0,						// csatorna: 0
		glm::vec3,				// CPU oldali adattípus amit a 0-ás csatorna attribútumainak meghatározására használtunk <- az eljárás a glm::vec3-ból kikövetkezteti, hogy 3 darab float-ból áll a 0-ás attribútum
		0,						// offset: az attribútum tároló elejétől vett offset-je, byte-ban
		sizeof(glm::vec3)		// stride: a következő csúcspont ezen attribútuma hány byte-ra van az aktuálistól
		>, m_skyboxVBO },
	},
		m_skyboxIBO
	);

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

	// shader program rövid létrehozása, egyetlen függvényhívással a fenti három:
	m_programSkybox.Init(
	{
		{ GL_VERTEX_SHADER, "skybox.vert" },
		{ GL_FRAGMENT_SHADER, "skybox.frag" }
	},
	{
		{ 0, "vs_in_pos" },				// VAO 0-as csatorna menjen a vs_in_pos-ba
	});

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
}

void CMyApp::Update()
{
	static Uint32 last_time = SDL_GetTicks();
	float delta_time = (SDL_GetTicks() - last_time) / 1000.0f;

	m_camera.Update(delta_time);

	last_time = SDL_GetTicks();
}

void CMyApp::DrawOrb() {
	// háromszorosára nagyítjuk és eltoljuk
	glm::mat4 matWorld;
	matWorld = glm::translate<float>(glm::vec3(-6, 6, 0)) * glm::scale<float>(glm::vec3(3, 3, 3));
	glm::mat4 matWorldIT = glm::transpose(glm::inverse(matWorld));
	glm::mat4 mvp = m_camera.GetViewProj() *matWorld;

	m_program.SetUniform("world", matWorld);
	m_program.SetUniform("worldIT", matWorldIT);
	m_program.SetUniform("MVP", mvp);

	m_program.SetTexture("texImage", 0, m_textureOrb);

	m_vao_orb.Bind();
	glDrawElements(GL_TRIANGLES, 2 * 3 * N * M, GL_UNSIGNED_INT, 0);
	m_vao_orb.Unbind();

}

void CMyApp::DrawCone() {
	glm::mat4 matWorld;
	matWorld = glm::translate<float>(glm::vec3(5, -5, 0));
	glm::mat4 matWorldIT = glm::transpose(glm::inverse(matWorld));
	glm::mat4 mvp = m_camera.GetViewProj() *matWorld;

	m_program.SetUniform("world", matWorld);
	m_program.SetUniform("worldIT", matWorldIT);
	m_program.SetUniform("MVP", mvp);

	m_program.SetTexture("texImage", 0, m_textureCone);

	m_vao_cone.Bind();
	glDrawElements(GL_TRIANGLES, 2 * 3 * N, GL_UNSIGNED_INT, 0);
	m_vao_cone.Unbind();
}

void CMyApp::DrawConeTop()
{
	glm::mat4 matWorld;
	matWorld = glm::translate<float>(glm::vec3(5, -5, 0));
	glm::mat4 matWorldIT = glm::transpose(glm::inverse(matWorld));
	glm::mat4 mvp = m_camera.GetViewProj() *matWorld;

	m_program.SetUniform("world", matWorld);
	m_program.SetUniform("worldIT", matWorldIT);
	m_program.SetUniform("MVP", mvp);

	m_program.SetTexture("texImage", 0, m_textureConeTop);

	m_vao_cone_top.Bind();
	glDrawElements(GL_TRIANGLES, 3 * N, GL_UNSIGNED_INT, 0);
	m_vao_cone_top.Unbind();
}

void CMyApp::DrawCylinder(glm::mat4 &matWorld)
{
	glm::mat4 matWorldIT = glm::transpose(glm::inverse(matWorld));
	glm::mat4 mvp = m_camera.GetViewProj() *matWorld;

	m_program.SetUniform("world", matWorld);
	m_program.SetUniform("worldIT", matWorldIT);
	m_program.SetUniform("MVP", mvp);

	m_program.SetTexture("texImage", 0, m_textureCylinder);

	m_vao_cylinder.Bind();
	glDrawElements(GL_TRIANGLE_STRIP, 2 * (N + 1), GL_UNSIGNED_INT, 0);
	m_vao_cylinder.Unbind();
	m_vao_cylinder_top.Bind();
	glDrawElements(GL_TRIANGLE_FAN, N + 2, GL_UNSIGNED_INT, 0);
	m_vao_cylinder_top.Unbind();
	m_vao_cylinder_bottom.Bind();
	glDrawElements(GL_TRIANGLE_FAN, N + 2, GL_UNSIGNED_INT, 0);
	m_vao_cylinder_bottom.Unbind();
}


void CMyApp::DrawGround() {

	glm::mat4 matWorld = glm::mat4(1.0f); // nem változtatunk rajta
	glm::mat4 matWorldIT = glm::transpose(glm::inverse(matWorld));
	glm::mat4 mvp = m_camera.GetViewProj() *matWorld;

	m_program.SetUniform("world", matWorld);
	m_program.SetUniform("worldIT", matWorldIT);
	m_program.SetUniform("MVP", mvp);

	m_program.SetTexture("texImage", 0, m_textureMetal);

	m_vao_ground.Bind();
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	m_vao_ground.Unbind();

}

void CMyApp::DrawSuzanne() {
	glm::mat4 suzanne1World = glm::translate(glm::vec3(-2, 0, 0));
	m_program.SetUniform("world", suzanne1World);
	m_program.SetUniform("worldIT", glm::transpose(glm::inverse(suzanne1World)));
	m_program.SetUniform("MVP", m_camera.GetViewProj()*suzanne1World);
	m_program.SetUniform("Kd", glm::vec4(1, 0.3, 0.3, 1));

	m_mesh->draw();
}

void CMyApp::Render()
{
	// töröljük a frampuffert (GL_COLOR_BUFFER_BIT) és a mélységi Z puffert (GL_DEPTH_BUFFER_BIT)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// mindegyik geometria ugyanazt a shader programot hasznalja: ne kapcsolgassuk most ki-be
	m_program.Use();

	DrawGround();
	DrawOrb();
	DrawSuzanne();
	DrawCone();
	DrawConeTop();
	glm::mat4 w = glm::translate<float>(glm::vec3(10, 8, 0))
		*glm::scale<float>(glm::vec3(0.5f, 7, 0.5f));
	DrawCylinder(w);
	w = glm::translate<float>(glm::vec3(5, 8, 0))
		*glm::scale<float>(glm::vec3(0.5f, 7, 0.5f));
	DrawCylinder(w);
	w = glm::translate<float>(glm::vec3(7.5f, 11.5f, 0))
		*glm::rotate<float>(0.5f*M_PI, glm::vec3(0, 0, 1))
		*glm::scale<float>(glm::vec3(0.5f, 9, 0.5f));
	DrawCylinder(w);
	w = glm::translate<float>(glm::vec3(7.5f, 9.5f, 0))
		*glm::rotate<float>(0.5f*M_PI, glm::vec3(0, 0, 1))
		*glm::scale<float>(glm::vec3(0.3f, 7, 0.3f));
	DrawCylinder(w);
	//DrawCylinderTop();
	//DrawCylinderBottom();
	m_program.Unuse();

	// végetért a 3D színtér rajzolása

	// skybox

	// mentsük el az előző Z-test eredményt, azaz azt a relációt, ami alapján update-eljük a pixelt.
	GLint prevDepthFnc;
	glGetIntegerv(GL_DEPTH_FUNC, &prevDepthFnc);

	// most kisebb-egyenlőt használjunk, mert mindent kitolunk a távoli vágósíkokra
	glDepthFunc(GL_LEQUAL);

	m_skyboxVAO.Bind();

	m_programSkybox.Use();
	m_programSkybox.SetUniform("MVP", m_camera.GetViewProj() * glm::translate(m_camera.GetEye()));

	// cube map textúra beállítása 0-ás mintavételezőre és annak a shaderre beállítása
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_skyboxTexture);
	glUniform1i(m_programSkybox.GetLocation("skyboxTexture"), 0);
	//m_program.SetTexture("skyboxTexture",0, m_skyboxTexture)

	// az előző három sor <=> m_programSkybox.SetCubeTexture("skyboxTexture", 0, m_skyboxTexture);

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);

	// végül állítsuk vissza
	glDepthFunc(prevDepthFnc);

	//
	// UI
	//
	// A következő parancs megnyit egy ImGui tesztablakot és így látszik mit tud az ImGui.
	//ImGui::ShowTestWindow();
	// A ImGui::ShowTestWindow implementációja az imgui_demo.cpp-ben található
	// Érdemes még az imgui.h-t böngészni, valamint az imgui.cpp elején a FAQ-ot elolvasni.
	// Rendes dokumentáció nincs, de a fentiek elegendőek kell legyenek.

	ImGui::SetNextWindowPos(ImVec2(300, 400), ImGuiSetCond_FirstUseEver);
	if (ImGui::Begin("Tesztablak"))
	{
		ImGui::Text("Fal (RGBA)");
		ImGui::SliderFloat4("Fal kd", &(m_wallColor[0]), 0, 1);
	}
	ImGui::End();
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