﻿#include "MyApp.h"
#include "GLUtils.hpp"

#include <math.h>

CMyApp::CMyApp(void)
{
	m_vaoID = 0;
	m_vboID = 0;
	m_programID = 0;
}


CMyApp::~CMyApp(void)
{
}

bool CMyApp::Init()
{
	// törlési szín legyen kékes
	glClearColor(0.125f, 0.25f, 0.5f, 1.0f);

	glEnable(GL_CULL_FACE); // kapcsoljuk be a hatrafele nezo lapok eldobasat
	glEnable(GL_DEPTH_TEST); // mélységi teszt bekapcsolása (takarás)
	glCullFace(GL_BACK); // GL_BACK: a kamerától "elfelé" nézõ lapok, GL_FRONT: a kamera felé nézõ lapok

	//glPolygonMode(GL_BACK, GL_LINE); // MEO ------------------------------------------------------------------------------------------------------------------------

	//
	// geometria letrehozasa
	//

	// MEO ------------------------------------------------------------------------------------------------------------------------

	Vertex a = { glm::vec3(-1, 0, -1), glm::vec3(0, 0.5, 0) };
	Vertex b = { glm::vec3(1, 0, -1), glm::vec3(1, 0.5, 0) };
	Vertex c = { glm::vec3(1, 0, 1), glm::vec3(1, 0.5, 1) };
	Vertex d = { glm::vec3(-1, 0, 1), glm::vec3(0, 0.5, 1) };
	Vertex e = { glm::vec3(0, sqrt(2), 0), glm::vec3(1, 1, 1) };

	Vertex vert[] =
	{ 
		b,a,e,
		c,b,e,
		d,c,e,
		a,d,e,

		a,b,c,
		{ glm::vec3(-1, 0, -1), glm::vec3(0, 0, 0) },c,d
	};

	numOfVertexes = sizeof(vert) / sizeof(vert[0]);

	// MEO ------------------------------------------------------------------------------------------------------------------------

	// 1 db VAO foglalasa
	glGenVertexArrays(1, &m_vaoID);
	// a frissen generált VAO beallitasa aktívnak
	glBindVertexArray(m_vaoID);
	
	// hozzunk létre egy új VBO erõforrás nevet
	glGenBuffers(1, &m_vboID); 
	glBindBuffer(GL_ARRAY_BUFFER, m_vboID); // tegyük "aktívvá" a létrehozott VBO-t
	// töltsük fel adatokkal az aktív VBO-t
	glBufferData( GL_ARRAY_BUFFER,	// az aktív VBO-ba töltsünk adatokat
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
		(void*)(sizeof(glm::vec3)) );

	glBindVertexArray(0); // feltöltüttük a VAO-t, kapcsoljuk le
	glBindBuffer(GL_ARRAY_BUFFER, 0); // feltöltöttük a VBO-t is, ezt is vegyük le

	//
	// shaderek betöltése
	//
	GLuint vs_ID = loadShader(GL_VERTEX_SHADER,		"myVert.vert");
	GLuint fs_ID = loadShader(GL_FRAGMENT_SHADER,	"myFrag.frag");

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
	glBindAttribLocation( m_programID, 1, "vs_in_col");

	// illesszük össze a shadereket (kimenõ-bemenõ változók összerendelése stb.)
	glLinkProgram(m_programID);

	// linkeles ellenorzese
	GLint infoLogLength = 0, result = 0;

	glGetProgramiv(m_programID, GL_LINK_STATUS, &result);
	glGetProgramiv(m_programID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if ( GL_FALSE == result )
	{
		std::vector<char> ProgramErrorMessage( infoLogLength );
		glGetProgramInfoLog(m_programID, infoLogLength, NULL, &ProgramErrorMessage[0]);
		fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);
		
		char* aSzoveg = new char[ProgramErrorMessage.size()];
		memcpy( aSzoveg, &ProgramErrorMessage[0], ProgramErrorMessage.size());

		std::cout << "[app.Init()] Sáder Huba panasza: " << aSzoveg << std::endl;

		delete aSzoveg;
	}

	// mar nincs ezekre szukseg
	glDeleteShader( vs_ID );
	glDeleteShader( fs_ID );

	//
	// egyéb inicializálás
	//

	// vetítési mátrix létrehozása
	m_matProj = glm::perspective( 45.0f, 640/480.0f, 1.0f, 1000.0f );

	// shader-beli transzformációs mátrixok címének lekérdezése
	m_loc_world = glGetUniformLocation( m_programID, "world");
	m_loc_view  = glGetUniformLocation( m_programID, "view" );
	m_loc_proj  = glGetUniformLocation( m_programID, "proj" );

	return true;
}

void CMyApp::Clean()
{
	glDeleteBuffers(1, &m_vboID);
	glDeleteVertexArrays(1, &m_vaoID);

	glDeleteProgram( m_programID );
}

void CMyApp::Update()
{
	/*
	// függőleges tengelye körül forog
	float t = SDL_GetTicks() / 1000.0 * M_PI;
	// nézeti transzformáció beállítása
	m_matView = glm::lookAt(glm::vec3(5 * cos(t), 0, 5 * sin(t)),		// honnan nézzük a színteret
							glm::vec3(0, 0, 0),							// a színtér melyik pontját nézzük
							glm::vec3(0, 1, 0));						// felfelé mutató irány a világban
	*/

	float t = SDL_GetTicks() / 1000.0 * M_PI/10; // MEO -----------------------------------------------------------------------------------------------
	// nézeti transzformáció beállítása
	m_matView = glm::lookAt(glm::vec3( 0,  2,  -5),		// honnan nézzük a színteret		// MEO ------------------------------------------------------------
							glm::vec3( 0,  0,  0),		// a színtér melyik pontját nézzük
							glm::vec3( 0,  1,  0));		// felfelé mutató irány a világban
}


void CMyApp::Render()
{
	// töröljük a frampuffert (GL_COLOR_BUFFER_BIT) és a mélységi Z puffert (GL_DEPTH_BUFFER_BIT)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// shader bekapcsolasa
	glUseProgram( m_programID );

	// shader parameterek beállítása
	/*

	GLM transzformációs mátrixokra példák:
		glm::rotate<float>( szög, glm::vec3(tengely_x, tengely_y, tengely_z) ) <- tengely_{xyz} körüli elforgatás
		glm::translate<float>( glm::vec3(eltol_x, eltol_y, eltol_z) ) <- eltolás
		glm::scale<float>( glm::vec3(s_x, s_y, s_z) ) <- léptékezés

	*/
	m_matWorld = glm::mat4(1.0f);

	// majd küldjük át a megfelelõ mátrixokat!
	glUniformMatrix4fv( m_loc_world,// erre a helyre töltsünk át adatot
						1,			// egy darab mátrixot
						GL_FALSE,	// NEM transzponálva
						&(m_matWorld[0][0]) ); // innen olvasva a 16 x sizeof(float)-nyi adatot
	glUniformMatrix4fv( m_loc_view,  1, GL_FALSE, &( m_matView[0][0]) );
	glUniformMatrix4fv( m_loc_proj,  1, GL_FALSE, &( m_matProj[0][0]) );

	// kapcsoljuk be a VAO-t (a VBO jön vele együtt)
	glBindVertexArray(m_vaoID);

	// kirajzolás
	glDrawArrays(	GL_TRIANGLES,	// rajzoljunk ki háromszöglista primitívet
					0,				// a VB elsõ eleme legyen az elsõ kiolvasott vertex
					numOfVertexes);				// és 6db csúcspont segítségével rajzoljunk háromszöglistát // MEO ---------------------------------------------

	// MEO ------------------------------------------------------------------------------------------------------------------------

	float t = SDL_GetTicks() / 1000.0 * M_PI / 2;

	/*
	// egy helyben forgás eltolt helyen
	m_matWorld =
		glm::translate<float>(glm::vec3(2, 0, 0))*
		glm::rotate<float>(t, glm::vec3(0, 1, 0));
	*/

	/*
	// keringés az y-tengely körül
	m_matWorld =
		glm::rotate<float>(t, glm::vec3(0, 1, 0))*
		glm::translate<float>(glm::vec3(2, 0, 0));
	*/

	/*
	// keringés az y-tengely körül orientáció megtartásával
	m_matWorld = 
		glm::rotate<float>(t, glm::vec3(0, 1, 0))*
		glm::translate<float>(glm::vec3(2, 0, 0))*
		glm::rotate<float>(-t, glm::vec3(0, 1, 0));
	*/

	
	// keringés egyszerûbben (egy eltolással)
	m_matWorld =
		glm::translate<float>(2.0f*glm::vec3(cos(t), 0, sin(t)));
	

	glUniformMatrix4fv(m_loc_world,	1,	GL_FALSE, &(m_matWorld[0][0]));
	glDrawArrays(GL_TRIANGLES, 0, numOfVertexes);
	

	// MEO ------------------------------------------------------------------------------------------------------------------------

	// VAO kikapcsolasa
	glBindVertexArray(0);

	// shader kikapcsolasa
	glUseProgram( 0 );
}

void CMyApp::KeyboardDown(SDL_KeyboardEvent& key)
{
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

	m_matProj = glm::perspective(  45.0f,		// 90 fokos nyilasszog
									_w/(float)_h,	// ablakmereteknek megfelelo nezeti arany
									0.01f,			// kozeli vagosik
									100.0f);		// tavoli vagosik
}