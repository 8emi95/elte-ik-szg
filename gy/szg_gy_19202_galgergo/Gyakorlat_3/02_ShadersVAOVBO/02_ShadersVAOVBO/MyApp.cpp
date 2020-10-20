#include "MyApp.h"
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

	//
	// geometria letrehozasa
	//

	Vertex vert[12+N+2] =
	{ 
		{glm::vec3(-1, -1, 0), glm::vec3(1, 0, 0)},
		{glm::vec3(0, -1, 0), glm::vec3(1, 0, 0)},
		{glm::vec3(-1, 0, 0), glm::vec3(1, 0, 0)},
				
		{glm::vec3(0, -1, 0), glm::vec3(0, 1, 0)},
		{glm::vec3(1, -1, 0), glm::vec3(0, 1, 0)},
		{glm::vec3(1, 0, 0), glm::vec3(0, 1, 0)},

		{glm::vec3(1, 0, 0), glm::vec3(1, 1, 1)},
		{glm::vec3(1, 1, 0), glm::vec3(1, 1, 1)},
		{glm::vec3(0, 1, 0), glm::vec3(1, 1, 1)},

		{glm::vec3(0, 1, 0), glm::vec3(0, 0, 1)},
		{glm::vec3(-1, 1, 0), glm::vec3(0, 0, 1)},
		{glm::vec3(-1, 0, 0), glm::vec3(0, 0, 1)},
	};
	
	//vert_num = sizeof(vert) / sizeof(Vertex);
	vert_num = 12;

	vert[12] = { glm::vec3(0, 0, 0), glm::vec3(0, 0, 0) };
	float r = 0.2;
	for (int i = 0; i <= N; ++i)
	{
		float alpha = 2 * M_PI * i / N;
		vert[12+1+i] = { glm::vec3(r*cos(alpha), r*sin(alpha), 0), glm::vec3(0, 0, 0) };
	}

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
		(GLuint)0,				// a VB-ben található adatok közül a 0. "indexû" attribútumait állítjuk be
		3,				// komponens szam
		GL_FLOAT,		// adatok tipusa
		GL_FALSE,		// normalizalt legyen-e
		sizeof(Vertex),	// stride (0=egymas utan)
		0				// a 0. indexû attribútum hol kezdõdik a sizeof(Vertex)-nyi területen belül
	); 

	// a második attribútumhoz pedig a VBO-ban sizeof(Vertex) ugrás után sizeof(glm::vec3)-nyit menve újabb 3 float adatot találunk (szín)
	glEnableVertexAttribArray(1); // ez lesz majd a szín
	glVertexAttribPointer(
		(GLuint)1,
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

	// attributomok osszerendelese a VAO es shader kozt
	glBindAttribLocation( m_programID, 0, "vs_in_pos");
	glBindAttribLocation( m_programID, 1, "vs_in_col");

	// illesszük össze a shadereket (kimenõ-bemenõ változók összerendelése stb.)
	glLinkProgram(m_programID);

	// linkeles ellenorzese
	GLint infoLogLength = 0, result = 0;

	glGetProgramiv(m_programID, GL_LINK_STATUS, &result);
	glGetProgramiv(m_programID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (GL_FALSE == result )
	{
		char* error = new char[infoLogLength];
		glGetProgramInfoLog(m_programID, infoLogLength, NULL, error);
		std::cerr << "[app.Init()] Probléma a shader létrehozásakor: " << error << std::endl;
		delete[] error;
	}

	// mar nincs ezekre szukseg
	glDeleteShader( vs_ID );
	glDeleteShader( fs_ID );

	loc_scale = glGetUniformLocation(m_programID, "scale");

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
	float time = SDL_GetTicks() / 1000.0 ;
	scale = sin(time * 2 * M_PI)+1;
}


void CMyApp::Render()
{
	// töröljük a frampuffert (GL_COLOR_BUFFER_BIT) és a mélységi Z puffert (GL_DEPTH_BUFFER_BIT)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// shader bekapcsolasa
	glUseProgram( m_programID );

	// kapcsoljuk be a VAO-t (a VBO jön vele együtt)
	glBindVertexArray(m_vaoID);

	// kirajzolás
	glUniform1f(loc_scale,1.0f);
	glDrawArrays(GL_TRIANGLES, 0, vert_num);

	glUniform1f(loc_scale, scale);
	glDrawArrays(GL_TRIANGLE_FAN, 12, N+2);

	// VAO kikapcsolasa
	glBindVertexArray(0);

	// shader kikapcsolasa
	glUseProgram( 0 );
}

void CMyApp::KeyboardDown(SDL_KeyboardEvent& key)
{
	switch (key.keysym.sym)
	{
	case SDLK_0: vert_num = 0 * 3; break;
	case SDLK_1: vert_num = 1 * 3; break;
	case SDLK_2: vert_num = 2 * 3; break;
	case SDLK_3: vert_num = 3 * 3; break;
	case SDLK_4: vert_num = 4 * 3; break;
	case SDLK_KP_PLUS:
		vert_num < 12 ? vert_num += 3 : vert_num = 12;
		break;
	case SDLK_KP_MINUS:
		vert_num > 0 ? vert_num -= 3 : vert_num = 0;
		break;
	case SDLK_KP_0: vert_num = 0 * 3; break;
	case SDLK_KP_1: vert_num = 1 * 3; break;
	case SDLK_KP_2: vert_num = 2 * 3; break;
	case SDLK_KP_3: vert_num = 3 * 3; break;
	case SDLK_KP_4: vert_num = 4 * 3; break;
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
	glViewport(0, 0, _w, _h );
}