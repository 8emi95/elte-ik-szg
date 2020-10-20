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

	Vertex vert[] = // Egy alakzat csúcsai
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

	glBindBuffer(GL_ARRAY_BUFFER, m_vboID); // tegyük "aktívvá" a létrehozott VBO-t
											// töltsük fel adatokkal az aktív VBO-t
	glBufferData(GL_ARRAY_BUFFER,	// az aktív VBO-ba töltsünk adatokat
		sizeof(vert),		// ennyi bájt nagyságban
		vert,	// errõl a rendszermemóriabeli címrõl olvasva
		GL_STATIC_DRAW);	// úgy, hogy a VBO-nkba nem tervezünk ezután írni és minden kirajzoláskor felhasználjuk a benne lévõ adatokat
	glEnableVertexAttribArray(0); // ez lesz majd a pozíció
	glVertexAttribPointer(
		0,				// a VB-ben található adatok közül a 0. "indexû" attribútumait állítjuk be
		3,				// komponens szam
		GL_FLOAT,		// adatok tipusa
		GL_FALSE,		// normalizalt legyen-e
		sizeof(Vertex),	// stride (0=egymas utan)
		0				// a 0. indexû attribútum hol kezdõdik a sizeof(Vertex)-nyi területen belül
	);

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

	GLuint vs_ID = loadShader(GL_VERTEX_SHADER, "myVert.vert");
	GLuint fs_ID = loadShader(GL_FRAGMENT_SHADER, "myFrag.frag");

	m_programID = glCreateProgram();

	glAttachShader(m_programID, vs_ID);
	glAttachShader(m_programID, fs_ID);
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

	m_loc_t = glGetUniformLocation(m_programID, "t"); // a program kezdetétõl számított idõ
	m_loc_key = glGetUniformLocation(m_programID, "key"); // legutóbbi billentyûlenyomás (0 az alapértelmezett)
	m_loc_tclick = glGetUniformLocation(m_programID, "tclick"); // a program kezdetétõl a legutóbbi lenyomásig eltelt idõ

	m_matProj = glm::perspective(55.0f, 640 / 480.0f, 1.0f, 1000.0f);

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
	float p = sinf(m_angleX);
	float q = cosf(m_angleX);

	m_eye = glm::vec3(3 + p * 35, 3 + m_angleY, 3 + q * 35);


	m_matView = glm::lookAt(glm::vec3(5, 0, 5),		// honnan nézzük a színteret
		glm::vec3(0, 0, 0),		// a színtér melyik pontját nézzük
		glm::vec3(0, 1, 0));		// felfelé mutató irány a világban
}


void CMyApp::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_programID);

	glUniform1i(m_loc_t, SDL_GetTicks());
	glUniform1i(m_loc_key, k);
	glUniform1i(m_loc_tclick, tclick);

	float r = (sinf(M_PI * (SDL_GetTicks() / 4000.0f))); // 4 másodpercig tart az egyik szélsõ pozícióból a másikba eljutni
	float t = r * 7.5f - 2.5f; // A [-1; 1] intervallumot felszorozzuk: r * ((15 - (-5))/2)
	// Aztán eltoljuk 2,5-tel, hogy kijöjjön [-5; 10] (mivel a Z felénk közeledve pozitív, ki kell vonni)

	m_matWorld = glm::mat4(1.0f);
	/*m_matWorld = glm::translate<float>(glm::vec3(-5.0f + t, -5.0f, -5.0f + (t * t * 0.15f))) // (x, 0, 0.15x^2), (-5, -5, -5)
		* glm::rotate<float>(tm, glm::vec3(1.0f, 0.0f, 0.0f));*/
	glm::mat4 mvp = m_matProj * m_matView * m_matWorld;

	glUniformMatrix4fv(m_loc_mvp,// erre a helyre töltsünk át adatot
		1,			// egy darab mátrixot
		GL_FALSE,	// NEM transzponálva
		&(mvp[0][0])); // innen olvasva a 16 x sizeof(float)-nyi adatot

	glBindVertexArray(m_vaoID);

	// kirajzolás
	glDrawElements(GL_TRIANGLES,		// primitív típus
		num_vertex,				// hány csúcspontot használunk a kirajzoláshoz
		GL_UNSIGNED_SHORT,	// indexek típusa
		0);					// indexek címe

	/*m_matWorld = glm::translate<float>(glm::vec3(5.0f + t, -5.0f, -5.0f + (t*t*0.15f))) // (5, -5, -5)
		* glm::rotate<float>(tm, glm::vec3(1.0f, 0.0f, 0.0f));*/
	mvp = m_matProj * m_matView * m_matWorld;
	glUniformMatrix4fv(m_loc_mvp,// erre a helyre töltsünk át adatot
		1,			// egy darab mátrixot
		GL_FALSE,	// NEM transzponálva
		&(mvp[0][0])); // innen olvasva a 16 x sizeof(float)-nyi adatot
	glDrawElements(GL_TRIANGLES,		// primitív típus
		num_vertex,					// hány csúcspontot használunk a kirajzoláshoz
		GL_UNSIGNED_SHORT,	// indexek típusa
		0);					// indexek címe	

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
		if (k == 1) // Ha már az 1-es fokozat be van kapcsolva, akkor kapcsolja ki
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