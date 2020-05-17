﻿/*
Készíts egy alkalmazást, amiben létrehozol 5 egységkockából felépített
T formát ( http://cg.elte.hu/~bsc_cg/gyak/kisbead1/image001.jpg ) (2 pont)

A fent elkészített geometriát rajzold ki a következő formációban:
az y=f(x,z)=(x/2)^3 – x*z/4 + (z/2)^2 felület –4<=x,z<=4 feletti részének
azon 9 pontjában, ahol x és z osztható 4-gyel
(tehát pl. a (–4, f(–4,4), 4) pontban) (2 pont)

Az így kirajzolt formációt mozgasd a következő pályán:
egy XY síkban fekvő y = 0.02*x^2 parabola –10<=x<=5 közötti részén
a –10 és 5 közötti végpontok között oda-vissza mozogva,
egy ilyen utat 12 másodperc alatt megtéve (2 pont)

A felhasználó a következőképpen állíthassa a színtérbeli történéseket:
a szóköz billentyű lenyomására kezdjenek el forogni a
formációban résztvevő geometriák a saját X tengelyük körül
(tehát ne az egész alakzat, hanem külön-külön a geometriák).
Újabb szóköz billentyű esetén álljon meg ez a forgás. (2 pont)
*/

#include "MyApp.h"
#include "GLUtils.hpp"
#include <ctime>

#include <vector>

#include <math.h>

CMyApp::CMyApp(void)
{
	m_vaoID = 0;
	m_vboID = 0;
	m_ibID = 0;

	m_programID = 0;
}


CMyApp::~CMyApp(void)
{
}

int get_random_int(int min, int max)
{
	return rand() % (max - min + 1) + min;
}

glm::vec3 CMyApp::get_random_point()
{
	possible_points.clear();
	for (int i = -4; i <= 4; i++)
	{
		if (i % 4 == 0)
		{
			possible_points.push_back(i);
		}
	}

	/*
	std::cout << "possible_points: ";
	for (int i = 0; i < possible_points.size(); ++i)
		std::cout << possible_points[i] << ' ';
	std::cout << "\n";
	*/

	int x = get_random_int(0, possible_points.size() - 1);
	int z = get_random_int(0, possible_points.size() - 1);
	float y = (x / 2) ^ 3 - x * z / 4 + (z / 2) ^ 2;
	return glm::vec3(possible_points[x], y, possible_points[z]);
}

bool CMyApp::Init()
{
	srand(time(0));
	// törlési szín legyen kékes
	glClearColor(0.125f, 0.25f, 0.5f, 1.0f);

	glEnable(GL_CULL_FACE); // kapcsoljuk be a hatrafele nezo lapok eldobasat
	glEnable(GL_DEPTH_TEST); // mélységi teszt bekapcsolása (takarás)
	glCullFace(GL_BACK); // GL_BACK: a kamerától "elfelé" nézõ lapok, GL_FRONT: a kamera felé nézõ lapok

	//
	// geometria letrehozasa
	//

	Vertex vert[] =
	{
		//          x,  y, z             R, G, B
		{glm::vec3(0,0,0), glm::vec3(0,0,0)},		//0
		{glm::vec3(0,1,0), glm::vec3(0,1,0)},		//1
		{glm::vec3(1,0,0), glm::vec3(1,0,0)},		//2
		{glm::vec3(1,1,0), glm::vec3(1,1,0)},		//3
		{glm::vec3(1,0,1), glm::vec3(1,0,1)},		//4
		{glm::vec3(1,1,1), glm::vec3(1,1,1)},		//5
		{glm::vec3(0,0,1), glm::vec3(0,0,1)},		//6
		{glm::vec3(0,1,1), glm::vec3(0,1,1)}		//7
	};

	// indexpuffer adatai
	GLushort indices[] =
	{
		//FRONT
		0,3,2,
		3,0,1,
		//RIGHT
		4,2,5,
		2,3,5,
		//BACK
		4,5,6,
		5,7,6,
		//LEFT
		6,7,0,
		7,1,0,
		//TOP
		1,7,3,
		7,5,3,
		//BOTTOM
		4,6,0,
		4,0,2
	};

	for (int i = 0; i < 9; i++) {
		glm::vec3 generated_point = get_random_point();
		if (std::find(std::begin(generated_points), std::end(generated_points), generated_point) != std::end(generated_points))
		{
			i--;
			continue;
		}
		generated_points[i] = generated_point;

		std::cout << generated_points[i].x << ", " << generated_points[i].y << ", " << generated_points[i].z << std::endl;
	}

	// 1 db VAO foglalasa
	glGenVertexArrays(1, &m_vaoID);
	// a frissen generált VAO beallitasa aktívnak
	glBindVertexArray(m_vaoID);

	// hozzunk létre egy új VBO erõforrás nevet
	glGenBuffers(1, &m_vboID);
	glBindBuffer(GL_ARRAY_BUFFER, m_vboID); // tegyük "aktívvá" a létrehozott VBO-t
	// töltsük fel adatokkal az aktív VBO-t
	glBufferData(GL_ARRAY_BUFFER,	// az aktív VBO-ba töltsünk adatokat
		sizeof(vert),		// ennyi bájt nagyságban
		vert,	// errõl a rendszermemóriabeli címrõl olvasva
		GL_STATIC_DRAW);	// úgy, hogy a VBO-nkba nem tervezünk ezután írni és minden kirajzoláskor felhasnzáljuk a benne lévõ adatokat


// VAO-ban jegyezzük fel, hogy a VBO-ban az elsõ 3 float sizeof(Vertex)-enként lesz az elsõ attribútum (pozíció)
	glEnableVertexAttribArray(0); // ez lesz majd a pozíció
	glVertexAttribPointer(
		0,				// a VB-ben található adatok közül a 0. "indexû" attribútumait állítjuk be
		3,				// komponens szam
		GL_FLOAT,		// adatok tipusa
		GL_FALSE,		// normalizalt legyen-e
		sizeof(Vertex),	// stride (0=egymas utan)
		0				// a 0. indexû attribútum hol kezdõdik a sizeof(Vertex)-nyi területen belül
	);

	// a második attribútumhoz pedig a VBO-ban sizeof(Vertex) ugrás után sizeof(glm::vec3)-nyit menve újabb 3 float adatot találunk (szín)
	glEnableVertexAttribArray(1); // ez lesz majd a szín
	glVertexAttribPointer(
		1,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(Vertex),
		(void*)(sizeof(glm::vec3)));

	// index puffer létrehozása
	glGenBuffers(1, &m_ibID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindVertexArray(0); // feltöltüttük a VAO-t, kapcsoljuk le
	glBindBuffer(GL_ARRAY_BUFFER, 0); // feltöltöttük a VBO-t is, ezt is vegyük le
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // feltöltöttük a VBO-t is, ezt is vegyük le

	//
	// shaderek betöltése
	//
	GLuint vs_ID = loadShader(GL_VERTEX_SHADER, "myVert.vert");
	GLuint fs_ID = loadShader(GL_FRAGMENT_SHADER, "myFrag.frag");

	// a shadereket tároló program létrehozása
	m_programID = glCreateProgram();

	// adjuk hozzá a programhoz a shadereket
	glAttachShader(m_programID, vs_ID);
	glAttachShader(m_programID, fs_ID);

	// VAO-beli attribútumok hozzárendelése a shader változókhoz
	// FONTOS: linkelés elõtt kell ezt megtenni!
	glBindAttribLocation(m_programID,	// shader azonosítója, amibõl egy változóhoz szeretnénk hozzárendelést csinálni
		0,				// a VAO-beli azonosító index
		"vs_in_pos");	// a shader-beli változónév
	glBindAttribLocation(m_programID, 1, "vs_in_col");

	// illesszük össze a shadereket (kimenõ-bemenõ változók összerendelése stb.)
	glLinkProgram(m_programID);

	// linkeles ellenorzese
	GLint infoLogLength = 0, result = 0;

	glGetProgramiv(m_programID, GL_LINK_STATUS, &result);
	glGetProgramiv(m_programID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (GL_FALSE == result)
	{
		std::vector<char> ProgramErrorMessage(infoLogLength);
		glGetProgramInfoLog(m_programID, infoLogLength, NULL, &ProgramErrorMessage[0]);
		fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);

		char* aSzoveg = new char[ProgramErrorMessage.size()];
		memcpy(aSzoveg, &ProgramErrorMessage[0], ProgramErrorMessage.size());

		std::cout << "[app.Init()] Sáder Huba panasza: " << aSzoveg << std::endl;

		delete aSzoveg;
	}

	// mar nincs ezekre szukseg
	glDeleteShader(vs_ID);
	glDeleteShader(fs_ID);

	//
	// egyéb inicializálás
	//

	// vetítési mátrix létrehozása
	m_matProj = glm::perspective(45.0f, 640 / 480.0f, 1.0f, 1000.0f);

	// shader-beli transzformációs mátrixok címének lekérdezése
	m_loc_mvp = glGetUniformLocation(m_programID, "MVP");

	return true;
}

void CMyApp::Clean()
{
	glDeleteBuffers(1, &m_vboID);
	glDeleteBuffers(1, &m_ibID);
	glDeleteVertexArrays(1, &m_vaoID);

	glDeleteProgram(m_programID);
}

void CMyApp::Update()
{
	// nézeti transzformáció beállítása
	float t = SDL_GetTicks() / 1000.0f;
	//m_matView = glm::lookAt(glm::vec3(5 * cosf(t), 5, 5 * sinf(t)),	// honnan nézzük a színteret
	m_matView = glm::lookAt(glm::vec3(20, 20, 20),		// honnan nézzük a színteret
		glm::vec3(0, 0, 0),		// a színtér melyik pontját nézzük
		glm::vec3(0, 1, 0));		// felfelé mutató irány a világban
}


void CMyApp::Render()
{
	// töröljük a frampuffert (GL_COLOR_BUFFER_BIT) és a mélységi Z puffert (GL_DEPTH_BUFFER_BIT)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// shader bekapcsolasa
	glUseProgram(m_programID);

	// shader parameterek beállítása
	/*

	GLM transzformációs mátrixokra példák:
		glm::rotate<float>( szög, glm::vec3(tengely_x, tengely_y, tengely_z) ) <- tengely_{xyz} körüli elforgatás
		glm::translate<float>( glm::vec3(eltol_x, eltol_y, eltol_z) ) <- eltolás
		glm::scale<float>( glm::vec3(s_x, s_y, s_z) ) <- léptékezés

	*/

	float rotate_angle;

	std::vector<glm::vec3> cube_shift = std::vector<glm::vec3>{
		glm::vec3(-1,0,0),
		glm::vec3(0,0,0),
		glm::vec3(1,0,0),
		glm::vec3(0,-1,0),
		glm::vec3(0,-2,0)
	};

	if (space_pressed % 2 == 1) {
		// rotate
		rotate_angle = SDL_GetTicks() / 1000.0f;// *2 * float(M_PI) / 10;
		rotate_var = glm::vec3(1, 0, 0);
	}
	else {
		// stop
		rotate_angle = 0;
		rotate_var = glm::vec3(1, 0, 0);
	}

	for (int i = 0; i < 9; i++)
	{
		for (int j = 0; j < 5; j++) {
			m_matWorld =
				glm::translate<float>(generated_points[i]) *
				glm::translate<float>(cube_shift[j]) *
				glm::rotate<float>(rotate_angle, rotate_var);

			glm::mat4 mvp = m_matProj * m_matView * m_matWorld;

			// majd küldjük át a megfelelõ mátrixot!
			glUniformMatrix4fv(m_loc_mvp,// erre a helyre töltsünk át adatot
				1,			// egy darab mátrixot
				GL_FALSE,	// NEM transzponálva
				&(mvp[0][0])); // innen olvasva a 16 x sizeof(float)-nyi adatot

			// kapcsoljuk be a VAO-t (a VBO jön vele együtt)
			glBindVertexArray(m_vaoID);

			// kirajzolás
			glDrawElements(GL_TRIANGLES,		// primitív típus
				36,					// hany csucspontot hasznalunk a kirajzolashoz
				GL_UNSIGNED_SHORT,	// indexek tipusa
				0);					// indexek cime
		}
	}
	// VAO kikapcsolasa
	glBindVertexArray(0);

	// shader kikapcsolasa
	glUseProgram(0);
}

void CMyApp::KeyboardDown(SDL_KeyboardEvent& key)
{
	if (key.keysym.sym == SDLK_SPACE) {
		space_pressed++;
	}
}

void CMyApp::KeyboardUp(SDL_KeyboardEvent& key)
{
}

void CMyApp::MouseMove(SDL_MouseMotionEvent& mouse)
{

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
	glViewport(0, 0, _w, _h);

	m_matProj = glm::perspective(45.0f,		// 90 fokos nyilasszog
		_w / (float)_h,	// ablakmereteknek megfelelo nezeti arany
		0.01f,			// kozeli vagosik
		100.0f);		// tavoli vagosik
}