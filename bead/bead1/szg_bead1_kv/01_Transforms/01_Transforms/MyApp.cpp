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
	glEnable(GL_DEPTH_TEST); // m�lys�gi teszt bekapcsol�sa (takar�s)
	glCullFace(GL_BACK); // GL_BACK: a kamer�t�l "elfel�" n�z� lapok, GL_FRONT: a kamera fel� n�z� lapok
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

	// indexpuffer adatai: NxM n�gysz�g = 2xNxM h�romsz�g = h�romsz�glista eset�n 3x2xNxM index
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

		glGenBuffers(1, &m_ibID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


		glBindVertexArray(0); // felt�lt�tt�k a VAO-t, kapcsoljuk le
		glBindBuffer(GL_ARRAY_BUFFER, 0); // felt�lt�tt�k a VBO-t is, ezt is vegy�k le
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

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

		// vet�t�si m�trix l�trehoz�sa
		m_matProj = glm::perspective(45.0f, 640 / 480.0f, 1.0f, 1000.0f);

		// shader-beli transzform�ci�s m�trixok c�m�nek lek�rdez�se
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
	// n�zeti transzform�ci� be�ll�t�sa
	float t = SDL_GetTicks() / 1000.0f;
	m_matView = glm::lookAt(glm::vec3(20, 2, 20),		// honnan n�zz�k a sz�nteret
							glm::vec3( 0,  0,  0),		// a sz�nt�r melyik pontj�t n�zz�k
							glm::vec3( 0,  1,  0));		// felfel� mutat� ir�ny a vil�gban
}

void CMyApp::DrawHenger(int a, int b, int c)
{
	glm::mat4 mvp;
	// kapcsoljuk be a VAO-t (a VBO j�n vele egy�tt)
	glBindVertexArray(m_vaoID);
	
	float t = M_PI * 2 * SDL_GetTicks() / 5000;
	float k = M_PI * 2 * SDL_GetTicks() / 10000;
	int r = 9;
	
	
	m_matWorld = glm::translate<float>(glm::vec3(a+cosf(t)*r, b, c+cosf(t)*r))*glm::rotate<float>(M_PI/2, glm::vec3(1, 0, 0));


	/*

	GLM transzform�ci�s m�trixokra p�ld�k:
	glm::rotate<float>( sz�g, glm::vec3(tengely_x, tengely_y, tengely_z) ) <- tengely_{xyz} k�r�li elforgat�s
	glm::translate<float>( glm::vec3(eltol_x, eltol_y, eltol_z) ) <- eltol�s
	glm::scale<float>( glm::vec3(s_x, s_y, s_z) ) <- l�pt�kez�s

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

	// shader parameterek be�ll�t�sa

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