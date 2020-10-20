#include "MyApp.h"
#include "GLUtils.hpp"

#include <math.h>

#include "ObjParser_OGL3.h"

CMyApp::CMyApp(void)
{
	m_vaoID = 0;
	m_vboID = 0;
	m_programID = 0;
	m_mesh = 0;
	m_mesh2 = 0;
	m_atomicTextureID = 0;
	m_donotTextureID = 0;
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

						 //
						 // geometria letrehozasa
						 //


	Vertex vert[] =
	{
		// 1. háromszög
		//          x,  y, z             R, G, B
		{ glm::vec3(-1, 0, -1), glm::vec3(1, 0, 0) },
		{ glm::vec3(1, 0, -1), glm::vec3(0, 1, 0) },
		{ glm::vec3(-1, 0,  1), glm::vec3(0, 0, 1) },

		// 2. háromszög
		{ glm::vec3(-1, 0,  1), glm::vec3(0, 0, 1) },
		{ glm::vec3(1, 0, -1), glm::vec3(0, 1, 0) },
		{ glm::vec3(1, 0,  1), glm::vec3(1, 1, 1) },

		// szemközti lap
		{ glm::vec3(0, 2, 0),  glm::vec3(1, 0, 0) },
		{ glm::vec3(-1, 0, 1), glm::vec3(1, 0, 0) },
		{ glm::vec3(1, 0, 1),  glm::vec3(1, 0, 0) },

		// jobb oldali lap
		{ glm::vec3(0, 2, 0),  glm::vec3(0, 1, 0) },
		{ glm::vec3(1, 0, 1),  glm::vec3(0, 1, 0) },
		{ glm::vec3(1, 0, -1), glm::vec3(0, 1, 0) },

		// hátulsó lap
		{ glm::vec3(0, 2, 0),   glm::vec3(0, 0, 1) },
		{ glm::vec3(1, 0, -1),  glm::vec3(0, 0, 1) },
		{ glm::vec3(-1, 0, -1), glm::vec3(0, 0, 1) },

		// bal oldali lap
		{ glm::vec3(0, 2, 0),   glm::vec3(1, 1, 1) },
		{ glm::vec3(-1, 0, -1), glm::vec3(1, 1, 1) },
		{ glm::vec3(-1, 0, 1),  glm::vec3(1, 1, 1) },

	};


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
	
	// textúrakoordináták bekapcsolása a 2-es azonosítójú attribútom csatornán
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(
		2,
		2,
		GL_FLOAT,
		GL_FALSE,
		sizeof(Vertex),
		(void*)(2 * sizeof(glm::vec3)));

	glBindVertexArray(0); // feltöltüttük a VAO-t, kapcsoljuk le
	glBindBuffer(GL_ARRAY_BUFFER, 0); // feltöltöttük a VBO-t is, ezt is vegyük le

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
	m_loc_world = glGetUniformLocation(m_programID, "world");
	m_loc_view = glGetUniformLocation(m_programID, "view");
	m_loc_proj = glGetUniformLocation(m_programID, "proj");

	m_loc_texture1 = glGetUniformLocation(m_programID, "texImage1");
	m_loc_texture2 = glGetUniformLocation(m_programID, "texImage2");
	m_loc_t = glGetUniformLocation(m_programID, "t");

	m_atomicTextureID = TextureFromFile("texture.bmp");
	m_donotTextureID = TextureFromFile("vaz.jpg");

	m_mesh = ObjParser::parse("a.obj");
	m_mesh->initBuffers();

	m_mesh2 = ObjParser::parse("b.obj");
	m_mesh2->initBuffers();

	return true;
}

void CMyApp::Clean()
{
	glDeleteTextures(1, &m_atomicTextureID);
	glDeleteTextures(1, &m_donotTextureID);

	glDeleteBuffers(1, &m_vboID);
	glDeleteVertexArrays(1, &m_vaoID);

	glDeleteProgram(m_programID);
}

void CMyApp::Update()
{
	// nézeti transzformáció beállítása

	m_matView = glm::lookAt(glm::vec3(40, 40, 5),		// honnan nézzük a színteret
		glm::vec3(0, 0, 0),		// a színtér melyik pontját nézzük
		glm::vec3(0, 1, 0));		// felfelé mutató irány a világban
}


void CMyApp::Render()
{
	// töröljük a frampuffert (GL_COLOR_BUFFER_BIT) és a mélységi Z puffert (GL_DEPTH_BUFFER_BIT)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// shader bekapcsolasa
	glUseProgram(m_programID);

	// majd küldjük át a megfelelõ mátrixokat!
	glUniformMatrix4fv(m_loc_world,// erre a helyre töltsünk át adatot
		1,			// egy darab mátrixot
		GL_FALSE,	// NEM transzponálva
		&(m_matWorld[0][0]));
	glUniformMatrix4fv(m_loc_view, 1, GL_FALSE, &(m_matView[0][0]));
	glUniformMatrix4fv(m_loc_proj, 1, GL_FALSE, &(m_matProj[0][0]));



	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_atomicTextureID);
	// aktiváljuk a generált textúránkat 
	// textúra mintavételezõ és shader-beli sampler2D összerendelése
	glUniform1i(m_loc_texture1,	// ezen azonosítójú sampler 2D
		0);				// olvassa az ezen indexû mintavételezõt

						// az 1-es textúra mintavételezõ egységhez másik textúra tartozik
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_donotTextureID);
	glUniform1i(m_loc_texture2, 1);

	




	if (textChange != 0)
	{

		// 2 * 3.14152f										-> egy teljes periódus (radiánban 2*PI)
		// 2 * 3.14152f * SDL_GetTicks()					-> mp-ként 1000 periódus
		// 2 * 3.14152f * SDL_GetTicks() / 1000.0f			-> mp-ként 1 periódus
		// 2 * 3.14152f * SDL_GetTicks() / 1000.0f / 6.0f	-> mp-ként 1/6 periódus, azaz 6 mp-ként 1 periódus
		float t;


		t = sin((2*3.14152f*(SDL_GetTicks() - startTick) / 1000.0f) / 12.0f);
		if (t < 0) {
			t = 0.0;
			textChange = 0;
		}
		//std::cout << "t erteke: " << t << "\n";
		glUniform1f(m_loc_t, t);
	}


	// kapcsoljuk be a VAO-t (a VBO jön vele együtt)
	glBindVertexArray(m_vaoID);


	/*
	// eltoljuk az yz tengely mentén 5 egységgel.
	glm::mat4 eltol = glm::translate<float>(0, 5, 5);

	// Ha egyenletesen akarjuk szétszórni a , akkor minden ciklus lefutásakor újabb 360/N fokokkal kell forgatnunk.
	glm::mat4 korbeszor = glm::rotate<float>(i * 360.0f / N, 0, 1, 0);

	glm::mat4 forogsugar = glm::translate<float>(10, 10, 0);
	glm::mat4 forog = glm::rotate<float>(SDL_GetTicks()/11000.0*360, 1, 1 , 0);


	// a szorzás sorrendje a logikai sorrendhez képest FORDÍTOTT!
	m_matWorld =
	forog *
	forogsugar *
	korbeszor *
	eltol
	;
	// Az újraszámolt világtranszformációt újra át kell adni a shadernek!
	*/

	float pAlapX = 10;
	float pAlapY = 2;
	float pAlapZ = 10;

	m_matWorld =
		//glm::rotate<float>(90,1,0,0)*
		
		glm::scale<float>(pAlapX, pAlapY, pAlapZ)
		;


	glUniformMatrix4fv(m_loc_world, 1, GL_FALSE, &(m_matWorld[0][0]));
	
	glDrawArrays(GL_TRIANGLES, 0, 18);

	



	m_matWorld =		
		glm::translate<float>(pAlapX, 0, pAlapZ)
		*glm::scale<float>(1, 1, 1)
		;

	glUniformMatrix4fv(m_loc_world, 1, GL_FALSE, &(m_matWorld[0][0]));
	
	m_mesh->draw();



	m_matWorld =
		glm::translate<float>(-pAlapX, 0, -pAlapZ)
		*glm::scale<float>(1, 1, 1)
		;

	glUniformMatrix4fv(m_loc_world, 1, GL_FALSE, &(m_matWorld[0][0]));

	m_mesh->draw();



	m_matWorld =
		glm::translate<float>(-pAlapX, 0, pAlapZ)
		*glm::scale<float>(1, 1, 1)
		
		;

	glUniformMatrix4fv(m_loc_world, 1, GL_FALSE, &(m_matWorld[0][0]));

	m_mesh2->draw();



	m_matWorld =
		glm::translate<float>(pAlapX, 0, -pAlapZ)
		*glm::scale<float>(1, 1, 1)
		
		;

	glUniformMatrix4fv(m_loc_world, 1, GL_FALSE, &(m_matWorld[0][0]));

	m_mesh2->draw();
	



	// VAO kikapcsolasa
	glBindVertexArray(0);

	// shader kikapcsolasa
	glUseProgram(0);
}

void CMyApp::KeyboardDown(SDL_KeyboardEvent& key)
{

	if (textChange != 0)
		return;
	if (key.keysym.sym == SDLK_2) {
		startTick = SDL_GetTicks();
		textChange = 1;
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