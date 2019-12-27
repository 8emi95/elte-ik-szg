#include "MyApp.h"
#include "GLUtils.hpp"

#include <math.h>

CMyApp::CMyApp(void)
{
	m_vaoID = 0;
	m_vboID = 0;
	m_programID = 0;
	m_ibID = 0;
}


CMyApp::~CMyApp(void)
{
}

glm::vec3	CMyApp::hengerUV(float u, float v)
{
	float r = 1;
	float h = 3;
	v *= h;
	u *= 2 * M_PI;
	float cu = cosf(u), su = sinf(u), cv = cosf(v), sv = sinf(v);
	return glm::vec3(r*cu,v,-r*su);
	
}


bool CMyApp::Init()
{
	glClearColor(0.125f, 0.25f, 0.5f, 1.0f);

	glEnable(GL_CULL_FACE); // kapcsoljuk be a hatrafele nezo lapok eldobasat
	glEnable(GL_DEPTH_TEST); // mélységi teszt bekapcsolása (takarás)
	glCullFace(GL_BACK); // GL_BACK: a kamerától "elfelé" nézõ lapok, GL_FRONT: a kamera felé nézõ lapok
	glPolygonMode(GL_BACK, GL_LINE);

	Vertex vert[(N + 1)*(M + 1) + 20+20];
	for (int i = 0; i <= N; ++i)
		for (int j = 0; j <= M; ++j)
		{
			float u = i / (float)N;
			float v = j / (float)M;

			vert[i + j*(N + 1)].p = hengerUV(u, v);
			vert[i + j*(N + 1)].c = glm::normalize(glm::vec3(rand() % 256, rand() % 256, rand() % 256));
		}


	vert[(N + 1)*(M + 1)].p = glm::vec3(0, 3, 0);
	vert[(N + 1)*(M + 1)].c = glm::normalize(glm::vec3(rand() % 256, rand() % 256, rand() % 256));
	
	for (int i = (N + 1)*(M + 1) + 1; i < (N + 1)*(M + 1) + 20; i++)
	{
		float theta = 2.0f*M_PI * float(i) / float(19);
		float x = 1.0*cosf(theta);
		float z = 1.0*sinf(theta);
		vert[i].p = glm::vec3(x, 3, z);
		vert[i].c = glm::normalize(glm::vec3(rand() % 256, rand() % 256, rand() % 256));
	}

	vert[(N + 1)*(M + 1)+20].p = glm::vec3(0, 0, 0);
	vert[(N + 1)*(M + 1)+20].c = glm::normalize(glm::vec3(rand() % 256, rand() % 256, rand() % 256));

	for (int i = (N + 1)*(M + 1) + 21; i < (N + 1)*(M + 1) + 20+20; i++)
	{
		float theta = 2.0f*M_PI * float(i) / float(19);
		float x = 1.0*cosf(theta);
		float z = 1.0*sinf(theta);
		vert[i].p = glm::vec3(x, 0, z);
		vert[i].c = glm::normalize(glm::vec3(rand() % 256, rand() % 256, rand() % 256));
	}

	// indexpuffer adatai: NxM négyszög = 2xNxM háromszög = háromszöglista esetén 3x2xNxM index
	GLushort indices[3 * 2 * (N)*(M)+3*20+3*20];
	for (int i = 0; i < N; ++i)
	{
		for (int j = 0; j < M; ++j)
		{
			indices[6 * i + j * 3 * 2 * (N)+0] = (i)+(j)*	(N + 1);
			indices[6 * i + j * 3 * 2 * (N)+1] = (i + 1) + (j)*	(N + 1);
			indices[6 * i + j * 3 * 2 * (N)+2] = (i)+(j + 1)*(N + 1);
			indices[6 * i + j * 3 * 2 * (N)+3] = (i + 1) + (j)*	(N + 1);
			indices[6 * i + j * 3 * 2 * (N)+4] = (i + 1) + (j + 1)*(N + 1);
			indices[6 * i + j * 3 * 2 * (N)+5] = (i)+(j + 1)*(N + 1);
		}
	}
	
	for (int i = 0; i < 19; ++i)
	{
		indices[3 * 2 * (N)*(M)+3 * i] = (N + 1) * (M + 1) + i + 1;
		indices[3 * 2 * (N)*(M)+3 * i + 1] = (N + 1) * (M + 1);
		
		if (i != 18)
		{

			indices[3 * 2 * (N)*(M)+3 * i + 2] = (N + 1) * (M + 1) + i + 2;
		}
		else
		{
			indices[3 * 2 * (N)*(M)+3 * i + 2] = (N + 1) * (M + 1)+1;
		}

	}

	for (int i = 0; i < 19; ++i) 
	{
		indices[3 * 2 * (N)*(M)+3*19 +3 * i] = (N + 1) * (M + 1) + 20;
		indices[3 * 2 * (N)*(M)+3*19 +3 * i + 1] =  (N + 1) * (M + 1) + 20 + i + 1;
		
		if (i != 18)
		{

			indices[3 * 2 * (N)*(M)+3*19 +3 * i + 2] = (N + 1) * (M + 1)+20 + i + 2;
		}
		else
		{
			indices[3 * 2 * (N)*(M)+3*19 +3 * i + 2] = (N + 1) * (M + 1)+1+20;
		}

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

		glGenBuffers(1, &m_ibID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


		glBindVertexArray(0); // feltöltüttük a VAO-t, kapcsoljuk le
		glBindBuffer(GL_ARRAY_BUFFER, 0); // feltöltöttük a VBO-t is, ezt is vegyük le
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

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

		// vetítési mátrix létrehozása
		m_matProj = glm::perspective(45.0f, 640 / 480.0f, 1.0f, 1000.0f);

		// shader-beli transzformációs mátrixok címének lekérdezése
		m_loc_mvp = glGetUniformLocation(m_programID, "MVP");

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
	// nézeti transzformáció beállítása
	float t = SDL_GetTicks() / 1000.0f;
	m_matView = glm::lookAt(glm::vec3(20, 2, 20),		// honnan nézzük a színteret
							glm::vec3( 0,  0,  0),		// a színtér melyik pontját nézzük
							glm::vec3( 0,  1,  0));		// felfelé mutató irány a világban
}

void CMyApp::DrawHenger(int a, int b, int c)
{
	glm::mat4 mvp;
	// kapcsoljuk be a VAO-t (a VBO jön vele együtt)
	glBindVertexArray(m_vaoID);
	
	float t = M_PI * 2 * SDL_GetTicks() / 5000;
	float k = M_PI * 2 * SDL_GetTicks() / 10000;
	int r = 9;
	
	
	m_matWorld = glm::translate<float>(glm::vec3(a+cosf(t)*r, b, c+cosf(t)*r))*glm::rotate<float>(M_PI/2, glm::vec3(1, 0, 0));


	/*

	GLM transzformációs mátrixokra példák:
	glm::rotate<float>( szög, glm::vec3(tengely_x, tengely_y, tengely_z) ) <- tengely_{xyz} körüli elforgatás
	glm::translate<float>( glm::vec3(eltol_x, eltol_y, eltol_z) ) <- eltolás
	glm::scale<float>( glm::vec3(s_x, s_y, s_z) ) <- léptékezés

	*/
	
	
	mvp = m_matProj * m_matView * m_matWorld;

	glUniformMatrix4fv(m_loc_mvp, 1, GL_FALSE, &(mvp[0][0]));
	glDrawElements(GL_TRIANGLES, 2 * 3 * N*M + 3 * 19 + 3 * 19, GL_UNSIGNED_SHORT, 0);

};

	
void CMyApp::Render()
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// shader bekapcsolasa
	glUseProgram( m_programID );

	// shader parameterek beállítása

	CMyApp::DrawHenger(1, 1, 1);
	CMyApp::DrawHenger(1, 1, -1);
	CMyApp::DrawHenger(1, -1, -1);
	CMyApp::DrawHenger(1, -1, 1);
	CMyApp::DrawHenger(-1, -1, 1);
	CMyApp::DrawHenger(-1, -1, -1);
	CMyApp::DrawHenger(-1, 1, -1);
	CMyApp::DrawHenger(-1, 1, 1);



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


void CMyApp::Resize(int _w, int _h)
{
	glViewport(0, 0, _w, _h);

	m_matProj = glm::perspective(  45.0f,		// 90 fokos nyilasszog
									_w/(float)_h,	// ablakmereteknek megfelelo nezeti arany
									0.01f,			// kozeli vagosik
									100.0f);		// tavoli vagosik
}