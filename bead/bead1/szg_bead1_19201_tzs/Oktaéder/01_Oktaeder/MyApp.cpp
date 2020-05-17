#include "MyApp.h"
#include "GLUtils.hpp"
#include <vector>
#include <ctime>
#include <cstdlib>

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

float random_between_two_float(float min, float max)
{
	return (min + 1) + (((float)rand()) / (float)RAND_MAX) * (max - (min + 1));
}

//
// egy parametrikus felület (u,v) paraméterértékekhez tartozó pontjának
// kiszámítását végzõ függvény
//
glm::vec3	CMyApp::GetRandomGombPont()
{
	// origó középpontú, egységsugarú gömb parametrikus alakja: http://hu.wikipedia.org/wiki/G%C3%B6mb#Egyenletek 
	// figyeljünk:	matematikában sokszor a Z tengely mutat felfelé, de nálunk az Y, tehát a legtöbb képlethez képest nálunk
	//				az Y és Z koordináták felcserélve szerepelnek
	int u = random_between_two_float(0, 2 * M_PI); // random 0 - 2pi
	int v = random_between_two_float(0, 2 * M_PI);; // random 0 - 2pi
	float r = random_between_two_float(0, 10 - sqrt(2)/2); // gömb sugara-oktaéder magassága, hogy ne eshessen kívül az alakzat a gömbön 
	float cu = cosf(u), su = sinf(u), cv = cosf(v), sv = sinf(v);
	//					x,          z,      y
	return glm::vec3(r * cu * sv, r * cv, r * su * sv);
}

bool CMyApp::Init()
{
	srand(time(0));
	// törlési szín legyen kékes
	glClearColor(0.125f, 0.25f, 0.5f, 1.0f);

	glEnable(GL_CULL_FACE); // kapcsoljuk be a hatrafele nezo lapok eldobasat
	glEnable(GL_DEPTH_TEST); // mélységi teszt bekapcsolása (takarás)
	glCullFace(GL_BACK); // GL_BACK: a kamerától "elfelé" nézõ lapok, GL_FRONT: a kamera felé nézõ lapok

	//glPolygonMode(GL_BACK, GL_LINE);
	//
	// geometria letrehozasa
	//

	Vertex vert[] =
	{
		//				x,			y,			z						  R, G, B
		{glm::vec3(0,			sqrt(2)/2,		0),				glm::vec3(0, 0, 0)	},			//0
		{glm::vec3(0,			-sqrt(2)/2,		0),				glm::vec3(0, 0, 1)	},			//1
		{glm::vec3(0,			0,				sqrt(2)/2),		glm::vec3(0, 1, 1)	},			//2
		{glm::vec3(sqrt(2)/2,	0,				0),				glm::vec3(1, 0, 0)	},			//3
		{glm::vec3(-sqrt(2)/2,	0,				0),				glm::vec3(1, 1, 0),	},			//4
		{glm::vec3(0,			0,				-sqrt(2)/2),	glm::vec3(1, 1, 1)	}			//5

	};

	// indexpuffer adatai
	GLushort indices[]=
	{
		// 1. háromszög
		0,3,5,
		// 2. háromszög
		0,5,4,
		// 3. háromszög
		0,4,2,
		// 4. háromszög
		0,2,3,
		// 5. háromszög
		1,5,3,
		// 6. háromszög
		1,4,5,
		// 7. háromszög
		1,2,4,
		// 8. háromszög
		1,3,2
	};

	for (int i = 0; i < 7; i++) {
		gombPontok[i] = GetRandomGombPont();	
	}

	// 1 db VAO foglalasa
	glGenVertexArrays(1, &m_vaoID);
	// a frissen generált VAO beallitasa aktívnak
	glBindVertexArray(m_vaoID);

	// hozzunk létre egy új VBO erõforrás nevet
	glGenBuffers(1, &m_vboID);
	glBindBuffer( GL_ARRAY_BUFFER, m_vboID); // tegyük "aktívvá" a létrehozott VBO-t
	// töltsük fel adatokkal az aktív VBO-t
	glBufferData( GL_ARRAY_BUFFER,	// az aktív VBO-ba töltsünk adatokat
		sizeof(vert),		// ennyi bájt nagyságban
		vert,				// errõl a rendszermemóriabeli címrõl olvasva
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

	glBindVertexArray(0);					  // feltöltüttük a VAO-t, kapcsoljuk le
	glBindBuffer(GL_ARRAY_BUFFER, 0);		  // feltöltöttük a VBO-t is, ezt is vegyük le
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
	glBindAttribLocation(	m_programID,	// shader azonosítója, amibõl egy változóhoz szeretnénk hozzárendelést csinálni
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
	c_loc_intp_end = glGetUniformLocation(m_programID, "intp_end");
	f_loc_intp_val = glGetUniformLocation(m_programID, "intp_val");

	elozoIdo = SDL_GetTicks();

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
	//m_matView = glm::lookAt(glm::vec3(5 * cosf(t), 1, 5 * sinf(t)),	// honnan nézzük a színteret
	m_matView = glm::lookAt(glm::vec3(20, 10, 20),		// honnan nézzük a színteret
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

	// kapcsoljuk be a VAO-t (a VBO jön vele együtt)
	glBindVertexArray(m_vaoID);


	//az eltelt idõ (delta_t) függvényében változtatjuk a színt
	unsigned int ido = SDL_GetTicks();
	int delta_t = ido - elozoIdo;
	elozoIdo = ido;

	float intp_n = 0;
	if (gombLenyomva) {
		szinInterpolaciosErtek += delta_t;
		szinInterpolaciosErtek %= szinvaltasPeriodus;
		//a periódus felénél (2,5 mp) elérjük a kiválasztott színt
		if (szinInterpolaciosErtek < szinvaltasPeriodus / 2) { //kiválasztott szín felé haladunk
			intp_n = (float)szinInterpolaciosErtek / (float)(szinvaltasPeriodus / 2);
		}
		else {	//eredeti szín felé haladunk
			intp_n = (float)(szinvaltasPeriodus - szinInterpolaciosErtek) / (float)(szinvaltasPeriodus / 2);
		}
	}


	glUniform1f(f_loc_intp_val, intp_n);			//aktuális interpolációs érték átadása a shadernek
	glUniform3fv(c_loc_intp_end, 1, &szinErtek[0]);	//kiválasztott szín átadása a shadernek

	double u = SDL_GetTicks() / 10000.0 * 2 * M_PI;
	double v = SDL_GetTicks() / 6000.0 * 2 * M_PI;
	double r = 8;

	float cu = cosf(u), su = sinf(u), cv = cosf(v), sv = sinf(v);

	for(int i = 0; i < 7; i++)
	{
		m_matWorld = 
			glm::translate<float>(glm::vec3(r * cu * sv, r * cv, r * su * sv)) *	//majd mozgatjuk a gömb felületén
			glm::translate<float>(gombPontok[i]);									//kirajzoljuk a 7 oktaédert
		glm::mat4 mvp = m_matProj * m_matView * m_matWorld;

		// majd küldjük át a megfelelõ mátrixot!
		glUniformMatrix4fv( m_loc_mvp,		// erre a helyre töltsünk át adatot
			1,				// egy darab mátrixot
			GL_FALSE,		// NEM transzponálva
			&(mvp[0][0]));	// innen olvasva a 16 x sizeof(float)-nyi adatot


		// kirajzolás
		glDrawElements(	GL_TRIANGLES,		// primitív típus
			24,					// hany csucspontot hasznalunk a kirajzolashoz
			GL_UNSIGNED_SHORT,	// indexek tipusa
			0);					// indexek cime
	}					

	// VAO kikapcsolasa
	glBindVertexArray(0);

	// shader kikapcsolasa
	glUseProgram(0);

}

void CMyApp::KeyboardDown(SDL_KeyboardEvent& key)
{
	switch (key.keysym.sym) {
	case SDLK_1: {
		if(!gombLenyomva) {
			szinInterpolaciosErtek = 0;
			szinErtek = glm::vec3(1, 0, 0);
		}
		break;
	}
	case SDLK_2: {
		if (!gombLenyomva) {
			szinInterpolaciosErtek = 0;
			szinErtek = glm::vec3(0, 1, 0);
		}

		break;
	}
	case SDLK_3: {
		if (!gombLenyomva) {
			szinInterpolaciosErtek = 0;
			szinErtek = glm::vec3(0, 0, 1);
		}

		break;
	}
	}
	gombLenyomva = true;
}

void CMyApp::KeyboardUp(SDL_KeyboardEvent& key)
{
	gombLenyomva = false;
}

void CMyApp::MouseMove(SDL_MouseMotionEvent& mouse) {}

void CMyApp::MouseDown(SDL_MouseButtonEvent& mouse) {}

void CMyApp::MouseUp(SDL_MouseButtonEvent& mouse) {}

void CMyApp::MouseWheel(SDL_MouseWheelEvent& wheel) {}

// a két paraméterbe az új ablakméret szélessége (_w) és magassága (_h) található
void CMyApp::Resize(int _w, int _h)
{
	glViewport(0, 0, _w, _h);

	m_matProj = glm::perspective(45.0f,			// 90 fokos nyilasszog
		_w / (float)_h,	// ablakmereteknek megfelelo nezeti arany
		0.01f,			// kozeli vagosik
		100.0f);		// tavoli vagosik
}