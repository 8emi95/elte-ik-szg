#include "MyApp.h"
#include "GLUtils.hpp"

#include <math.h>

CMyApp::CMyApp(void)
{
	//vao_id, vbo_id, program_id inicializalasa
	m_vaoID = 0;
	m_vboID = 0;
	m_programID = 0;

	c = 4;
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

	//
	// geometria letrehozasa
	//

	Vertex vert[] =
	{ 
		{glm::vec3(-1,  1, 0), glm::vec3(1, 0, 0)},
		{glm::vec3(-1,  0, 0), glm::vec3(0, 1, 0)},
		{glm::vec3( 0,  1, 0), glm::vec3(0, 0, 1)},

		{ glm::vec3( 0,  1, 0), glm::vec3(1, 0, 0) },
		{ glm::vec3( 1,  0, 0), glm::vec3(0, 0, 1) },
		{ glm::vec3( 1,  1, 0), glm::vec3(0, 1, 0) },

		{ glm::vec3(1,  0, 0), glm::vec3(1, 0, 0) },
		{ glm::vec3(0, -1, 0), glm::vec3(0, 0, 1) },
		{ glm::vec3(1, -1, 0), glm::vec3(0, 1, 0) },

		{ glm::vec3(0, -1, 0), glm::vec3(1, 0, 0) },
		{ glm::vec3(-1, 0, 0), glm::vec3(0, 0, 1) },
		{ glm::vec3(-1, -1, 0), glm::vec3(0, 1, 0) },
	};

	// 1 db VAO foglalasa
	glGenVertexArrays(1, &m_vaoID);
	// a frissen gener�lt VAO beallitasa akt�vnak
	glBindVertexArray(m_vaoID);
	
	// hozzunk l�tre egy �j VBO er�forr�s nevet
	glGenBuffers(1, &m_vboID); 
	glBindBuffer(GL_ARRAY_BUFFER, m_vboID); // tegy�k "akt�vv�" a l�trehozott VBO-t
	// t�lts�k fel adatokkal az akt�v VBO-t
	glBufferData( GL_ARRAY_BUFFER,	// az akt�v VBO-ba t�lts�nk adatokat
				  sizeof(vert),		// ennyi b�jt nagys�gban
				  vert,	// err�l a rendszermem�riabeli c�mr�l olvasva
				  GL_STATIC_DRAW);	// �gy, hogy a VBO-nkba nem tervez�nk ezut�n �rni �s minden kirajzol�skor felhasnz�ljuk a benne l�v� adatokat
	

	// VAO-ban jegyezz�k fel, hogy a VBO-ban az els� 3 float sizeof(Vertex)-enk�nt lesz az els� attrib�tum (poz�ci�)
	glEnableVertexAttribArray(0); // ez lesz majd a poz�ci�
	glVertexAttribPointer(
		(GLuint)0,				// a VB-ben tal�lhat� adatok k�z�l a 0. "index�" attrib�tumait �ll�tjuk be
		3,				// komponens szam
		GL_FLOAT,		// adatok tipusa
		GL_FALSE,		// normalizalt legyen-e
		sizeof(Vertex),	// stride (0=egymas utan)
		0				// a 0. index� attrib�tum hol kezd�dik a sizeof(Vertex)-nyi ter�leten bel�l
	); 

	// a m�sodik attrib�tumhoz pedig a VBO-ban sizeof(Vertex) ugr�s ut�n sizeof(glm::vec3)-nyit menve �jabb 3 float adatot tal�lunk (sz�n)
	glEnableVertexAttribArray(1); // ez lesz majd a sz�n
	glVertexAttribPointer(
		(GLuint)1,
		3, 
		GL_FLOAT,
		GL_FALSE,
		sizeof(Vertex),
		(void*)(sizeof(glm::vec3)) );

	glBindVertexArray(0); // felt�lt�tt�k a VAO-t, kapcsoljuk le
	glBindBuffer(GL_ARRAY_BUFFER, 0); // felt�lt�tt�k a VBO-t is, ezt is vegy�k le

	//
	// shaderek bet�lt�se
	//
	GLuint vs_ID = loadShader(GL_VERTEX_SHADER,		"myVert.vert");
	GLuint fs_ID = loadShader(GL_FRAGMENT_SHADER,	"myFrag.frag");
	//shadere "forditasa"

	// a shadereket t�rol� program l�trehoz�sa
	m_programID = glCreateProgram();

	// adjuk hozz� a programhoz a shadereket
	glAttachShader(m_programID, vs_ID);
	glAttachShader(m_programID, fs_ID);

	//osszekotjuk a VAO adatait a vertex shaderrel
	//innen fogja tudni a vertex shader, hogy amire azt mondtuk, 0-s csatorna legyen, az a vs_in_pos a vertex_shaderben (NYILVAN A VAO KIMENETE HATOROZZA MEG A VERTEX SHADER BEMENETEIT IS)
	//vs_in_col pedig az 1-es csatorna
	// attributomok osszerendelese a VAO es shader kozt
	glBindAttribLocation( m_programID, 0, "vs_in_pos");
	glBindAttribLocation( m_programID, 1, "vs_in_col");

	// illessz�k �ssze a shadereket (kimen�-bemen� v�ltoz�k �sszerendel�se stb.)
	//shaderek linkelese 
	glLinkProgram(m_programID);
	//letrejon a futtathato allomany, a gpu program

	// linkeles ellenorzese
	GLint infoLogLength = 0, result = 0;

	glGetProgramiv(m_programID, GL_LINK_STATUS, &result);
	glGetProgramiv(m_programID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (GL_FALSE == result )
	{
		char* error = new char[infoLogLength];
		glGetProgramInfoLog(m_programID, infoLogLength, NULL, error);
		std::cerr << "[app.Init()] Probl�ma a shader l�trehoz�sakor: " << error << std::endl;
		delete[] error;
	}

	// mar nincs ezekre szukseg
	//ezeket kibaszhatjuk
	glDeleteShader( vs_ID );
	glDeleteShader( fs_ID );

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
}

void CMyApp::Render()
{
	// t�r�lj�k a frampuffert (GL_COLOR_BUFFER_BIT) �s a m�lys�gi Z puffert (GL_DEPTH_BUFFER_BIT)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// shader bekapcsolasa
	//RENDERBEN KELL BEKAPCSOLNI A GPU-programot... mindig, minden futasnal hasznaljuk
	glUseProgram( m_programID );

	// kapcsoljuk be a VAO-t (a VBO j�n vele egy�tt)
	glBindVertexArray(m_vaoID);

	static GLuint location = glGetUniformLocation(m_programID, "color");
	glUniform4fv(location, 1, &col[0]);

	// kirajzol�s
	glDrawArrays(GL_TRIANGLES, 0, 3*c);

	// VAO kikapcsolasa
	glBindVertexArray(0);

	// shader kikapcsolasa
	glUseProgram( 0 );
}

void CMyApp::KeyboardDown(SDL_KeyboardEvent& key)
{
	switch (key.keysym.sym) {
	case SDLK_0:
		c = 0;
		break;
	case SDLK_1:
		c = 1;
		break;
	case SDLK_2:
		c = 2;
		break;
	case SDLK_3:
		c = 3;
		break;
	case SDLK_4:
		c = 4;
		break;
	}
}

void CMyApp::KeyboardUp(SDL_KeyboardEvent& key)
{
}

void CMyApp::MouseMove(SDL_MouseMotionEvent& mouse)
{
	col = glm::vec4(mouse.x/640.f, mouse.y/480.f, 1.f, 1.f);
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
	glViewport(0, 0, _w, _h );
}