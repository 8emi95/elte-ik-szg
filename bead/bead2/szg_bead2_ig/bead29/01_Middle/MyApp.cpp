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
	// t�rl�si sz�n legyen k�kes
	glClearColor(0.125f, 0.25f, 0.5f, 1.0f);

	glEnable(GL_CULL_FACE); // kapcsoljuk be a hatrafele nezo lapok eldobasat
	glEnable(GL_DEPTH_TEST); // m�lys�gi teszt bekapcsol�sa (takar�s)
	glCullFace(GL_BACK); // GL_BACK: a kamer�t�l "elfel�" n�z� lapok, GL_FRONT: a kamera fel� n�z� lapok

						 //
						 // geometria letrehozasa
						 //


	Vertex vert[] =
	{
		// 1. h�romsz�g
		//          x,  y, z             R, G, B
		{ glm::vec3(-1, 0, -1), glm::vec3(1, 0, 0) },
		{ glm::vec3(1, 0, -1), glm::vec3(0, 1, 0) },
		{ glm::vec3(-1, 0,  1), glm::vec3(0, 0, 1) },

		// 2. h�romsz�g
		{ glm::vec3(-1, 0,  1), glm::vec3(0, 0, 1) },
		{ glm::vec3(1, 0, -1), glm::vec3(0, 1, 0) },
		{ glm::vec3(1, 0,  1), glm::vec3(1, 1, 1) },

		// szemk�zti lap
		{ glm::vec3(0, 2, 0),  glm::vec3(1, 0, 0) },
		{ glm::vec3(-1, 0, 1), glm::vec3(1, 0, 0) },
		{ glm::vec3(1, 0, 1),  glm::vec3(1, 0, 0) },

		// jobb oldali lap
		{ glm::vec3(0, 2, 0),  glm::vec3(0, 1, 0) },
		{ glm::vec3(1, 0, 1),  glm::vec3(0, 1, 0) },
		{ glm::vec3(1, 0, -1), glm::vec3(0, 1, 0) },

		// h�tuls� lap
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
	// a frissen gener�lt VAO beallitasa akt�vnak
	glBindVertexArray(m_vaoID);

	// hozzunk l�tre egy �j VBO er�forr�s nevet
	glGenBuffers(1, &m_vboID);
	glBindBuffer(GL_ARRAY_BUFFER, m_vboID); // tegy�k "akt�vv�" a l�trehozott VBO-t
											// t�lts�k fel adatokkal az akt�v VBO-t
	glBufferData(GL_ARRAY_BUFFER,	// az akt�v VBO-ba t�lts�nk adatokat
		sizeof(vert),		// ennyi b�jt nagys�gban
		vert,	// err�l a rendszermem�riabeli c�mr�l olvasva
		GL_STATIC_DRAW);	// �gy, hogy a VBO-nkba nem tervez�nk ezut�n �rni �s minden kirajzol�skor felhasnz�ljuk a benne l�v� adatokat


							// VAO-ban jegyezz�k fel, hogy a VBO-ban az els� 3 float sizeof(Vertex)-enk�nt lesz az els� attrib�tum (poz�ci�)
	glEnableVertexAttribArray(0); // ez lesz majd a poz�ci�
	glVertexAttribPointer(
		0,				// a VB-ben tal�lhat� adatok k�z�l a 0. "index�" attrib�tumait �ll�tjuk be
		3,				// komponens szam
		GL_FLOAT,		// adatok tipusa
		GL_FALSE,		// normalizalt legyen-e
		sizeof(Vertex),	// stride (0=egymas utan)
		0				// a 0. index� attrib�tum hol kezd�dik a sizeof(Vertex)-nyi ter�leten bel�l
	);

	// a m�sodik attrib�tumhoz pedig a VBO-ban sizeof(Vertex) ugr�s ut�n sizeof(glm::vec3)-nyit menve �jabb 3 float adatot tal�lunk (sz�n)
	glEnableVertexAttribArray(1); // ez lesz majd a sz�n
	glVertexAttribPointer(
		1,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(Vertex),
		(void*)(sizeof(glm::vec3)));
	
	// text�rakoordin�t�k bekapcsol�sa a 2-es azonos�t�j� attrib�tom csatorn�n
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(
		2,
		2,
		GL_FLOAT,
		GL_FALSE,
		sizeof(Vertex),
		(void*)(2 * sizeof(glm::vec3)));

	glBindVertexArray(0); // felt�lt�tt�k a VAO-t, kapcsoljuk le
	glBindBuffer(GL_ARRAY_BUFFER, 0); // felt�lt�tt�k a VBO-t is, ezt is vegy�k le

									  //
									  // shaderek bet�lt�se
									  //
	GLuint vs_ID = loadShader(GL_VERTEX_SHADER, "myVert.vert");
	GLuint fs_ID = loadShader(GL_FRAGMENT_SHADER, "myFrag.frag");

	// a shadereket t�rol� program l�trehoz�sa
	m_programID = glCreateProgram();

	// adjuk hozz� a programhoz a shadereket
	glAttachShader(m_programID, vs_ID);
	glAttachShader(m_programID, fs_ID);

	// VAO-beli attrib�tumok hozz�rendel�se a shader v�ltoz�khoz
	// FONTOS: linkel�s el�tt kell ezt megtenni!
	glBindAttribLocation(m_programID,	// shader azonos�t�ja, amib�l egy v�ltoz�hoz szeretn�nk hozz�rendel�st csin�lni
		0,				// a VAO-beli azonos�t� index
		"vs_in_pos");	// a shader-beli v�ltoz�n�v
	glBindAttribLocation(m_programID, 1, "vs_in_col");

	// illessz�k �ssze a shadereket (kimen�-bemen� v�ltoz�k �sszerendel�se stb.)
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

		std::cout << "[app.Init()] S�der Huba panasza: " << aSzoveg << std::endl;

		delete aSzoveg;
	}

	// mar nincs ezekre szukseg
	glDeleteShader(vs_ID);
	glDeleteShader(fs_ID);

	//
	// egy�b inicializ�l�s
	//

	// vet�t�si m�trix l�trehoz�sa
	m_matProj = glm::perspective(45.0f, 640 / 480.0f, 1.0f, 1000.0f);

	// shader-beli transzform�ci�s m�trixok c�m�nek lek�rdez�se
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
	// n�zeti transzform�ci� be�ll�t�sa

	m_matView = glm::lookAt(glm::vec3(40, 40, 5),		// honnan n�zz�k a sz�nteret
		glm::vec3(0, 0, 0),		// a sz�nt�r melyik pontj�t n�zz�k
		glm::vec3(0, 1, 0));		// felfel� mutat� ir�ny a vil�gban
}


void CMyApp::Render()
{
	// t�r�lj�k a frampuffert (GL_COLOR_BUFFER_BIT) �s a m�lys�gi Z puffert (GL_DEPTH_BUFFER_BIT)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// shader bekapcsolasa
	glUseProgram(m_programID);

	// majd k�ldj�k �t a megfelel� m�trixokat!
	glUniformMatrix4fv(m_loc_world,// erre a helyre t�lts�nk �t adatot
		1,			// egy darab m�trixot
		GL_FALSE,	// NEM transzpon�lva
		&(m_matWorld[0][0]));
	glUniformMatrix4fv(m_loc_view, 1, GL_FALSE, &(m_matView[0][0]));
	glUniformMatrix4fv(m_loc_proj, 1, GL_FALSE, &(m_matProj[0][0]));



	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_atomicTextureID);
	// aktiv�ljuk a gener�lt text�r�nkat 
	// text�ra mintav�telez� �s shader-beli sampler2D �sszerendel�se
	glUniform1i(m_loc_texture1,	// ezen azonos�t�j� sampler 2D
		0);				// olvassa az ezen index� mintav�telez�t

						// az 1-es text�ra mintav�telez� egys�ghez m�sik text�ra tartozik
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_donotTextureID);
	glUniform1i(m_loc_texture2, 1);

	




	if (textChange != 0)
	{

		// 2 * 3.14152f										-> egy teljes peri�dus (radi�nban 2*PI)
		// 2 * 3.14152f * SDL_GetTicks()					-> mp-k�nt 1000 peri�dus
		// 2 * 3.14152f * SDL_GetTicks() / 1000.0f			-> mp-k�nt 1 peri�dus
		// 2 * 3.14152f * SDL_GetTicks() / 1000.0f / 6.0f	-> mp-k�nt 1/6 peri�dus, azaz 6 mp-k�nt 1 peri�dus
		float t;


		t = sin((2*3.14152f*(SDL_GetTicks() - startTick) / 1000.0f) / 12.0f);
		if (t < 0) {
			t = 0.0;
			textChange = 0;
		}
		//std::cout << "t erteke: " << t << "\n";
		glUniform1f(m_loc_t, t);
	}


	// kapcsoljuk be a VAO-t (a VBO j�n vele egy�tt)
	glBindVertexArray(m_vaoID);


	/*
	// eltoljuk az yz tengely ment�n 5 egys�ggel.
	glm::mat4 eltol = glm::translate<float>(0, 5, 5);

	// Ha egyenletesen akarjuk sz�tsz�rni a , akkor minden ciklus lefut�sakor �jabb 360/N fokokkal kell forgatnunk.
	glm::mat4 korbeszor = glm::rotate<float>(i * 360.0f / N, 0, 1, 0);

	glm::mat4 forogsugar = glm::translate<float>(10, 10, 0);
	glm::mat4 forog = glm::rotate<float>(SDL_GetTicks()/11000.0*360, 1, 1 , 0);


	// a szorz�s sorrendje a logikai sorrendhez k�pest FORD�TOTT!
	m_matWorld =
	forog *
	forogsugar *
	korbeszor *
	eltol
	;
	// Az �jrasz�molt vil�gtranszform�ci�t �jra �t kell adni a shadernek!
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

// a k�t param�terbe az �j ablakm�ret sz�less�ge (_w) �s magass�ga (_h) tal�lhat�
void CMyApp::Resize(int _w, int _h)
{
	glViewport(0, 0, _w, _h);

	m_matProj = glm::perspective(45.0f,		// 90 fokos nyilasszog
		_w / (float)_h,	// ablakmereteknek megfelelo nezeti arany
		0.01f,			// kozeli vagosik
		100.0f);		// tavoli vagosik
}