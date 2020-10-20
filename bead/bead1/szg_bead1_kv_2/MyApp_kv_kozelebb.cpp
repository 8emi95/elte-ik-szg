#include "MyApp.h"
#include "GLUtils.hpp"

#include <math.h>

CMyApp::CMyApp(void)
{
	m_vaoID = 0;
	m_vboID = 0;
	m_ibID = 0;

	m_angleY = 0;
	m_angleX = 0;
	m_programID = 0;
	tm = 0.0f;
	k = 0;
}


CMyApp::~CMyApp(void)
{
}

bool CMyApp::Init()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glCullFace(GL_BACK);

	Vertex vert[] = // Egy alakzat cs�csai
	{
		//          x,  y, z           R, G, B
		{ glm::vec3(1, 1, 0), glm::vec3(1, 0, 0) },
		{ glm::vec3(0, 0, 0), glm::vec3(1, 0, 0) },
		{ glm::vec3(1, 0, 0), glm::vec3(1, 0, 0) },

		{ glm::vec3(1, 1, 0), glm::vec3(0, 1, 0) },
		{ glm::vec3(0, 1, 0), glm::vec3(0, 1, 0) },
		{ glm::vec3(0, 0, 0), glm::vec3(0, 1, 0) },
		//1

		{ glm::vec3(2, 1, 0), glm::vec3(0, 0, 1) },
		{ glm::vec3(1, 0, 0), glm::vec3(0, 0, 1) },
		{ glm::vec3(2, 0, 0), glm::vec3(0, 0, 1) },

		{ glm::vec3(2, 1, 0), glm::vec3(1, 1, 0) },
		{ glm::vec3(1, 1, 0), glm::vec3(1, 1, 0) },
		{ glm::vec3(1, 0, 0), glm::vec3(1, 1, 0) },
		//2

		{ glm::vec3(1, 2, 0), glm::vec3(1, 0, 1) },
		{ glm::vec3(1, 1, 0), glm::vec3(1, 0, 1) },
		{ glm::vec3(2, 1, 0), glm::vec3(1, 0, 1) },

		{ glm::vec3(2, 1, 0), glm::vec3(0, 1, 1) },
		{ glm::vec3(2, 2, 0), glm::vec3(0, 1, 1) },
		{ glm::vec3(1, 2, 0), glm::vec3(0, 1, 1) },
		//3

		{ glm::vec3(2, 3, 0), glm::vec3(1, 1, 1) },
		{ glm::vec3(1, 3, 0), glm::vec3(1, 1, 1) },
		{ glm::vec3(1, 2, 0), glm::vec3(1, 1, 1) },

		{ glm::vec3(1, 2, 0), glm::vec3(0.5f, 0, 0) },
		{ glm::vec3(2, 2, 0), glm::vec3(0.5f, 0, 0) },
		{ glm::vec3(2, 3, 0), glm::vec3(0.5f, 0, 0) },
		//4

		{ glm::vec3(3, 3, 0), glm::vec3(0, 0.5f, 0) },
		{ glm::vec3(2, 3, 0), glm::vec3(0, 0.5f, 0) },
		{ glm::vec3(2, 2, 0), glm::vec3(0, 0.5f, 0) },

		{ glm::vec3(2, 2, 0), glm::vec3(0, 0, 0.5f) },
		{ glm::vec3(3, 2, 0), glm::vec3(0, 0, 0.5f) },
		{ glm::vec3(3, 3, 0), glm::vec3(0, 0, 0.5f) },
		//5

		{ glm::vec3(1, 1, -1), glm::vec3(0.5f, 0.5f, 0) },
		{ glm::vec3(1, 0, -1), glm::vec3(0.5f, 0.5f, 0) },
		{ glm::vec3(0, 0, -1), glm::vec3(0.5f, 0.5f, 0) },

		{ glm::vec3(0, 0, -1), glm::vec3(0.5f, 0, 0.5f) },
		{ glm::vec3(0, 1, -1), glm::vec3(0.5f, 0, 0.5f) },
		{ glm::vec3(1, 1, -1), glm::vec3(0.5f, 0, 0.5f) },
		//6

		{ glm::vec3(2, 1, -1), glm::vec3(0, 0.5f, 0.5f) },
		{ glm::vec3(2, 0, -1), glm::vec3(0, 0.5f, 0.5f) },
		{ glm::vec3(1, 0, -1), glm::vec3(0, 0.5f, 0.5f) },

		{ glm::vec3(1, 0, -1), glm::vec3(0.5f, 0.5f, 0.5f) },
		{ glm::vec3(1, 1, -1), glm::vec3(0.5f, 0.5f, 0.5f) },
		{ glm::vec3(2, 1, -1), glm::vec3(0.5f, 0.5f, 0.5f) },
		//7

		{ glm::vec3(1, 2, -1), glm::vec3(0.5f, 1, 1) },
		{ glm::vec3(2, 2, -1), glm::vec3(0.5f, 1, 1) },
		{ glm::vec3(2, 1, -1), glm::vec3(0.5f, 1, 1) },

		{ glm::vec3(2, 1, -1), glm::vec3(1, 0.5f, 1) },
		{ glm::vec3(1, 1, -1), glm::vec3(1, 0.5f, 1) },
		{ glm::vec3(1, 2, -1), glm::vec3(1, 0.5f, 1) },
		//8

		{ glm::vec3(2, 3, -1), glm::vec3(1, 1, 0.5f) },
		{ glm::vec3(2, 2, -1), glm::vec3(1, 1, 0.5f) },
		{ glm::vec3(1, 2, -1), glm::vec3(1, 1, 0.5f) },

		{ glm::vec3(1, 2, -1), glm::vec3(0.5f, 0.5f, 1) },
		{ glm::vec3(1, 3, -1), glm::vec3(0.5f, 0.5f, 1) },
		{ glm::vec3(2, 3, -1), glm::vec3(0.5f, 0.5f, 1) },
		//9

		{ glm::vec3(3, 3, -1), glm::vec3(0.5f, 1, 0.5f) },
		{ glm::vec3(3, 2, -1), glm::vec3(0.5f, 1, 0.5f) },
		{ glm::vec3(2, 2, -1), glm::vec3(0.5f, 1, 0.5f) },

		{ glm::vec3(2, 2, -1), glm::vec3(0.5f, 0.5f, 1) },
		{ glm::vec3(2, 3, -1), glm::vec3(0.5f, 0.5f, 1) },
		{ glm::vec3(3, 3, -1), glm::vec3(0.5f, 0.5f, 1) },
		//10

		//----------

		{ glm::vec3(0, 0, 0), glm::vec3(0.5f, 0.2f, 1) },
		{ glm::vec3(0, 1, 0), glm::vec3(0.5f, 0.2f, 1) },
		{ glm::vec3(0, 1, -1), glm::vec3(0.5f, 0.2f, 1) },

		{ glm::vec3(0, 1, -1), glm::vec3(0.5f, 0.2f, 0.2f) },
		{ glm::vec3(0, 0, -1), glm::vec3(0.5f, 0.2f, 0.2f) },
		{ glm::vec3(0, 0, 0), glm::vec3(0.5f, 0.2f, 0.2f) },
		//11

		{ glm::vec3(0, 1, 0), glm::vec3(0.8f, 0.4f, 1) },
		{ glm::vec3(1, 1, 0), glm::vec3(0.8f, 0.4f, 1) },
		{ glm::vec3(1, 1, -1), glm::vec3(0.8f, 0.4f, 1) },

		{ glm::vec3(1, 1, -1), glm::vec3(0.5f, 1, 0.2f) },
		{ glm::vec3(0, 1, -1), glm::vec3(0.5f, 1, 0.2f) },
		{ glm::vec3(0, 1, 0), glm::vec3(0.5f, 1, 0.2f) },
		//12

		{ glm::vec3(1, 1, 0), glm::vec3(0.5f, 0, 1) },
		{ glm::vec3(1, 2, 0), glm::vec3(0.5f, 0, 1) },
		{ glm::vec3(1, 2, -1), glm::vec3(0.5f, 0, 1) },

		{ glm::vec3(1, 2, -1), glm::vec3(0.5f, 1, 0) },
		{ glm::vec3(1, 1, -1), glm::vec3(0.5f, 1, 0) },
		{ glm::vec3(1, 1, 0), glm::vec3(0.5f, 1, 0) },
		//13

		{ glm::vec3(1, 2, 0), glm::vec3(0, 0.5f, 1) },
		{ glm::vec3(1, 3, 0), glm::vec3(0, 0.5f, 1) },
		{ glm::vec3(1, 3, -1), glm::vec3(0, 0.5f, 1) },

		{ glm::vec3(1, 3, -1), glm::vec3(1, 0.5f, 0) },
		{ glm::vec3(1, 2, -1), glm::vec3(1, 0.5f, 0) },
		{ glm::vec3(1, 2, 0), glm::vec3(1, 0.5f, 0) },
		//14

		{ glm::vec3(1, 3, 0), glm::vec3(0, 1, 0.5f) },
		{ glm::vec3(2, 3, 0), glm::vec3(0, 1, 0.5f) },
		{ glm::vec3(2, 3, -1), glm::vec3(0, 1, 0.5f) },

		{ glm::vec3(2, 3, -1), glm::vec3(1, 0, 0.5f) },
		{ glm::vec3(1, 3, -1), glm::vec3(1, 0, 0.5f) },
		{ glm::vec3(1, 3, 0), glm::vec3(1, 0, 0.5f) },
		//15

		{ glm::vec3(2, 3, 0), glm::vec3(1, 0.75f, 1) },
		{ glm::vec3(3, 3, 0), glm::vec3(1, 0.75f, 1) },
		{ glm::vec3(3, 3, -1), glm::vec3(1, 0.75f, 1) },

		{ glm::vec3(3, 3, -1), glm::vec3(0.75f, 1, 1) },
		{ glm::vec3(2, 3, -1), glm::vec3(0.75f, 1, 1) },
		{ glm::vec3(2, 3, 0), glm::vec3(0.75f, 1, 1) },
		//16

		{ glm::vec3(3, 2, -1), glm::vec3(0, 0.75f, 0) },
		{ glm::vec3(3, 3, -1), glm::vec3(0, 0.75f, 0) },
		{ glm::vec3(3, 3, 0), glm::vec3(0, 0.75f, 0) },

		{ glm::vec3(3, 3, 0), glm::vec3(0, 0, 0.75f) },
		{ glm::vec3(3, 2, 0), glm::vec3(0, 0, 0.75f) },
		{ glm::vec3(3, 2, -1), glm::vec3(0, 0, 0.75f) },
		//17

		{ glm::vec3(3, 2, -1), glm::vec3(1, 1, 0.75f) },
		{ glm::vec3(3, 2, 0), glm::vec3(1, 1, 0.75f) },
		{ glm::vec3(2, 2, 0), glm::vec3(1, 1, 0.75f) },

		{ glm::vec3(2, 2, 0), glm::vec3(0.75f, 0, 0) },
		{ glm::vec3(2, 2, -1), glm::vec3(0.75f, 0, 0) },
		{ glm::vec3(3, 2, -1), glm::vec3(0.75f, 0, 0) },
		//18

		{ glm::vec3(2, 2, -1), glm::vec3(0.75f, 0, 1) },
		{ glm::vec3(2, 2, 0), glm::vec3(0.75f, 0, 1) },
		{ glm::vec3(2, 1, 0), glm::vec3(0.75f, 0, 1) },

		{ glm::vec3(2, 1, 0), glm::vec3(0.75f, 1, 0) },
		{ glm::vec3(2, 1, -1), glm::vec3(0.75f, 1, 0) },
		{ glm::vec3(2, 2, -1), glm::vec3(0.75f, 1, 0) },
		//19

		{ glm::vec3(2, 1, -1), glm::vec3(0, 0.75f, 1) },
		{ glm::vec3(2, 1, 0), glm::vec3(0, 0.75f, 1) },
		{ glm::vec3(2, 0, 0), glm::vec3(0, 0.75f, 1) },

		{ glm::vec3(2, 0, 0), glm::vec3(1, 0.75f, 0) },
		{ glm::vec3(2, 0, -1), glm::vec3(1, 0.75f, 0) },
		{ glm::vec3(2, 1, -1), glm::vec3(1, 0.75f, 0) },
		// 20

		{ glm::vec3(2, 0, -1), glm::vec3(0, 1, 0.75f) },
		{ glm::vec3(2, 0, 0), glm::vec3(0, 1, 0.75f) },
		{ glm::vec3(1, 0, 0), glm::vec3(0, 1, 0.75f) },

		{ glm::vec3(1, 0, 0), glm::vec3(1, 0, 0.75f) },
		{ glm::vec3(1, 0, -1), glm::vec3(1, 0, 0.75f) },
		{ glm::vec3(2, 0, -1), glm::vec3(1, 0, 0.75f) },
		//21

		{ glm::vec3(1, 0, -1), glm::vec3(0.75f, 0.75f, 0) },
		{ glm::vec3(1, 0, 0), glm::vec3(0.75f, 0.75f, 0) },
		{ glm::vec3(0, 0, 0), glm::vec3(0.75f, 0.75f, 0) },

		{ glm::vec3(0, 0, 0), glm::vec3(0.75f, 0.75f, 0.75f) },
		{ glm::vec3(0, 0, -1), glm::vec3(0.75f, 0.75f, 0.75f) },
		{ glm::vec3(1, 0, -1), glm::vec3(0.75f, 0.75f, 0.75f) },
		//22
	};
	GLushort indices[] =
	{
		0,1,2,
		3,4,5,
		6,7,8,
		9,10,11,
		12,13,14,
		15,16,17,
		18,19,20,
		21,22,23,
		24,25,26,
		27,28,29,
		30,31,32,
		33,34,35,
		36,37,38,
		39,40,41,
		42,43,44,
		45,46,47,
		48,49,50,
		51,52,53,
		54,55,56,
		57,58,59,
		60,61,62,
		63,64,65,
		66,67,68,
		69,70,71,
		72,73,74,
		75,76,77,
		78,79,80,
		81,82,83,
		84,85,86,
		87,88,89,
		90,91,92,
		93,94,95,
		96,97,98,
		99,100,101,
		102,103,104,
		105,106,107,
		108,109,110,
		111,112,113,
		114,115,116,
		117,118,119,
		120,121,122,
		123,124,125,
		126,127,128,
		129,130,131
	};

	glGenVertexArrays(1, &m_vaoID);

	glBindVertexArray(m_vaoID);

	glGenBuffers(1, &m_vboID);

	glBindBuffer(GL_ARRAY_BUFFER, m_vboID); // tegy�k "akt�vv�" a l�trehozott VBO-t
											// t�lts�k fel adatokkal az akt�v VBO-t
	glBufferData(GL_ARRAY_BUFFER,	// az akt�v VBO-ba t�lts�nk adatokat
		sizeof(vert),		// ennyi b�jt nagys�gban
		vert,	// err�l a rendszermem�riabeli c�mr�l olvasva
		GL_STATIC_DRAW);	// �gy, hogy a VBO-nkba nem tervez�nk ezut�n �rni �s minden kirajzol�skor felhaszn�ljuk a benne l�v� adatokat
	glEnableVertexAttribArray(0); // ez lesz majd a poz�ci�
	glVertexAttribPointer(
		0,				// a VB-ben tal�lhat� adatok k�z�l a 0. "index�" attrib�tumait �ll�tjuk be
		3,				// komponens szam
		GL_FLOAT,		// adatok tipusa
		GL_FALSE,		// normalizalt legyen-e
		sizeof(Vertex),	// stride (0=egymas utan)
		0				// a 0. index� attrib�tum hol kezd�dik a sizeof(Vertex)-nyi ter�leten bel�l
	);

	glEnableVertexAttribArray(1); // ez lesz majd a sz�n
	glVertexAttribPointer(
		1,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(Vertex),
		(void*)(sizeof(glm::vec3)));

	// index puffer l�trehoz�sa
	glGenBuffers(1, &m_ibID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindVertexArray(0); // felt�lt�tt�k a VAO-t, kapcsoljuk le
	glBindBuffer(GL_ARRAY_BUFFER, 0); // felt�lt�tt�k a VBO-t is, ezt is vegy�k le
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // felt�lt�tt�k a VBO-t is, ezt is vegy�k le

	GLuint vs_ID = loadShader(GL_VERTEX_SHADER, "myVert.vert");
	GLuint fs_ID = loadShader(GL_FRAGMENT_SHADER, "myFrag.frag");

	m_programID = glCreateProgram();

	glAttachShader(m_programID, vs_ID);
	glAttachShader(m_programID, fs_ID);
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

	m_loc_t = glGetUniformLocation(m_programID, "t"); // a program kezdet�t�l sz�m�tott id�
	m_loc_key = glGetUniformLocation(m_programID, "key"); // legut�bbi billenty�lenyom�s (0 az alap�rtelmezett)
	m_loc_tclick = glGetUniformLocation(m_programID, "tclick"); // a program kezdet�t�l a legut�bbi lenyom�sig eltelt id�

	m_matProj = glm::perspective(55.0f, 640 / 480.0f, 1.0f, 1000.0f);

	// shader-beli transzform�ci�s m�trixok c�m�nek lek�rdez�se
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
	float p = sinf(m_angleX);
	float q = cosf(m_angleX);

	m_eye = glm::vec3(3 + p * 35, 3 + m_angleY, 3 + q * 35);


	m_matView = glm::lookAt(glm::vec3(5, 0, 5),		// honnan n�zz�k a sz�nteret
		glm::vec3(0, 0, 0),		// a sz�nt�r melyik pontj�t n�zz�k
		glm::vec3(0, 1, 0));		// felfel� mutat� ir�ny a vil�gban
}


void CMyApp::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_programID);

	glUniform1i(m_loc_t, SDL_GetTicks());
	glUniform1i(m_loc_key, k);
	glUniform1i(m_loc_tclick, tclick);

	float r = (sinf(M_PI * (SDL_GetTicks() / 4000.0f))); // 4 m�sodpercig tart az egyik sz�ls� poz�ci�b�l a m�sikba eljutni
	float t = r * 7.5f - 2.5f; // A [-1; 1] intervallumot felszorozzuk: r * ((15 - (-5))/2)
	// Azt�n eltoljuk 2,5-tel, hogy kij�jj�n [-5; 10] (mivel a Z fel�nk k�zeledve pozit�v, ki kell vonni)

	m_matWorld = glm::mat4(1.0f);
	/*m_matWorld = glm::translate<float>(glm::vec3(-5.0f + t, -5.0f, -5.0f + (t * t * 0.15f))) // (x, 0, 0.15x^2), (-5, -5, -5)
		* glm::rotate<float>(tm, glm::vec3(1.0f, 0.0f, 0.0f));*/
	glm::mat4 mvp = m_matProj * m_matView * m_matWorld;

	glUniformMatrix4fv(m_loc_mvp,// erre a helyre t�lts�nk �t adatot
		1,			// egy darab m�trixot
		GL_FALSE,	// NEM transzpon�lva
		&(mvp[0][0])); // innen olvasva a 16 x sizeof(float)-nyi adatot

	glBindVertexArray(m_vaoID);

	// kirajzol�s
	glDrawElements(GL_TRIANGLES,		// primit�v t�pus
		num_vertex,				// h�ny cs�cspontot haszn�lunk a kirajzol�shoz
		GL_UNSIGNED_SHORT,	// indexek t�pusa
		0);					// indexek c�me

	/*m_matWorld = glm::translate<float>(glm::vec3(5.0f + t, -5.0f, -5.0f + (t*t*0.15f))) // (5, -5, -5)
		* glm::rotate<float>(tm, glm::vec3(1.0f, 0.0f, 0.0f));*/
	mvp = m_matProj * m_matView * m_matWorld;
	glUniformMatrix4fv(m_loc_mvp,// erre a helyre t�lts�nk �t adatot
		1,			// egy darab m�trixot
		GL_FALSE,	// NEM transzpon�lva
		&(mvp[0][0])); // innen olvasva a 16 x sizeof(float)-nyi adatot
	glDrawElements(GL_TRIANGLES,		// primit�v t�pus
		num_vertex,					// h�ny cs�cspontot haszn�lunk a kirajzol�shoz
		GL_UNSIGNED_SHORT,	// indexek t�pusa
		0);					// indexek c�me	

	/*m_matWorld = glm::translate<float>(glm::vec3(-5.0f + t, 5.0f, -5.0f + (t*t*0.15f))) // (-5, 5, -5)
		* glm::rotate<float>(tm, glm::vec3(1.0f, 0.0f, 0.0f));*/
	/*mvp = m_matProj * m_matView * m_matWorld;
	glUniformMatrix4fv(m_loc_mvp,
		1,
		GL_FALSE,
		&(mvp[0][0]));
	glDrawElements(GL_TRIANGLES,
		num_vertex,
		GL_UNSIGNED_SHORT,
		0);

	/*m_matWorld = glm::translate<float>(glm::vec3(-5.0f + t, -5.0f, 5.0f + (t*t*0.15f))) // (-5, -5, 5)
		* glm::rotate<float>(tm, glm::vec3(1.0f, 0.0f, 0.0f));
	mvp = m_matProj * m_matView * m_matWorld;
	glUniformMatrix4fv(m_loc_mvp,
		1,
		GL_FALSE,
		&(mvp[0][0]));
	glDrawElements(GL_TRIANGLES,
		num_vertex,
		GL_UNSIGNED_SHORT,
		0);

	m_matWorld = glm::translate<float>(glm::vec3(5.0f + t, 5.0f, -5.0f + (t*t*0.15f))) // (5, 5, -5)
		* glm::rotate<float>(tm, glm::vec3(1.0f, 0.0f, 0.0f));
	mvp = m_matProj * m_matView * m_matWorld;
	glUniformMatrix4fv(m_loc_mvp,
		1,			
		GL_FALSE,
		&(mvp[0][0]));
	glDrawElements(GL_TRIANGLES,
		num_vertex,
		GL_UNSIGNED_SHORT,
		0);

	m_matWorld = glm::translate<float>(glm::vec3(5.0f + t, -5.0f, 5.0f + (t*t*0.15f))) // (5, -5, 5)
		* glm::rotate<float>(tm, glm::vec3(1.0f, 0.0f, 0.0f));
	mvp = m_matProj * m_matView * m_matWorld;
	glUniformMatrix4fv(m_loc_mvp,
		1,		
		GL_FALSE,
		&(mvp[0][0]));
	glDrawElements(GL_TRIANGLES,
		num_vertex,
		GL_UNSIGNED_SHORT,
		0);

	m_matWorld = glm::translate<float>(glm::vec3(-5.0f + t, 5.0f, 5.0f + (t*t*0.15f))) * // (-5, 5, 5)
		glm::rotate<float>(tm, glm::vec3(1.0f, 0.0f, 0.0f));
	mvp = m_matProj * m_matView * m_matWorld;
	glUniformMatrix4fv(m_loc_mvp,
		1,
		GL_FALSE,
		&(mvp[0][0]));
	glDrawElements(GL_TRIANGLES,
		num_vertex,
		GL_UNSIGNED_SHORT,
		0);

	m_matWorld = glm::translate<float>(glm::vec3(5.0f + t, 5.0f, 5.0f + (t*t*0.15f))) // (5, 5, 5)
		* glm::rotate<float>(tm, glm::vec3(1.0f, 0.0f, 0.0f));
	mvp = m_matProj * m_matView * m_matWorld;
	glUniformMatrix4fv(m_loc_mvp,
		1,
		GL_FALSE,
		&(mvp[0][0]));
	glDrawElements(GL_TRIANGLES,
		num_vertex,
		GL_UNSIGNED_SHORT,
		0);*/

	// VAO kikapcsolasa
	glBindVertexArray(0);

	// shader kikapcsolasa
	glUseProgram(0);
}

void CMyApp::KeyboardDown(SDL_KeyboardEvent& key)
{
	if (key.keysym.sym == SDLK_1)
	{
		if (k == 1) // Ha m�r az 1-es fokozat be van kapcsolva, akkor kapcsolja ki
			k = 0;
		else
			k = 1;
		tclick = SDL_GetTicks();
	}
	else if (key.keysym.sym == SDLK_2)
	{
		if (k == 2)
			k = 0;
		else
			k = 2;
		tclick = SDL_GetTicks();
	}
	else if (key.keysym.sym == SDLK_3)
	{
		if (k == 3)
			k = 0;
		else
			k = 3;
		tclick = SDL_GetTicks();
	}
}

void CMyApp::KeyboardUp(SDL_KeyboardEvent& key)
{
}

void CMyApp::MouseMove(SDL_MouseMotionEvent& mouse)
{
	m_angleX = mouse.x / 50.f;
	m_angleY = (mouse.y - 240) / 5.0f;
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

void CMyApp::Resize(int _w, int _h)
{
	glViewport(0, 0, _w, _h);

	m_matProj = glm::perspective(45.0f,		// 90 fokos nyilasszog
		_w / (float)_h,	// ablakmereteknek megfelelo nezeti arany
		0.01f,			// kozeli vagosik
		100.0f);		// tavoli vagosik
}