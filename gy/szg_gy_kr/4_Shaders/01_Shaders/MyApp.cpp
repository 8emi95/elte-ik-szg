#include "MyApp.h"
#include "GLUtils.hpp"

#include <math.h>

CMyApp::CMyApp(void)
{
}


CMyApp::~CMyApp(void)
{
}

bool CMyApp::Init()
{
	// törlési szín legyen kékes
	//glClearColor(0.125f, 0.25f, 0.5f, 1.0f);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	glEnable(GL_CULL_FACE); // hatrafele nezo lapok eldobasa
	glEnable(GL_DEPTH_TEST); // mélységi teszt (takarás)

	//
	// shaderek betöltése
	//milyen shader, milyen neven
	//"shaderek forditasa targykodda"
	GLuint vs_ID = loadShader(GL_VERTEX_SHADER,		"myVert.vert");
	GLuint fs_ID = loadShader(GL_FRAGMENT_SHADER,	"myFrag.frag");

	// a shadereket tároló program létrehozása
	m_programID = glCreateProgram();

	// adjuk hozzá a programhoz a shadereket
	glAttachShader(m_programID, vs_ID);
	glAttachShader(m_programID, fs_ID);

	// illesszük össze a shadereket (kimenõ-bemenõ változók összerendelése stb.)
	glLinkProgram(m_programID);

	// linkeles ellenorzese
	GLint infoLogLength = 0, result = 0;

	glGetProgramiv(m_programID, GL_LINK_STATUS, &result);
	glGetProgramiv(m_programID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if ( GL_FALSE == result )
	{
		std::vector<char> VertexShaderErrorMessage(infoLogLength);
		glGetProgramInfoLog(m_programID, infoLogLength, NULL, &VertexShaderErrorMessage[0]);

		std::cerr << &VertexShaderErrorMessage[0];
	}

	// már nincs ezekre szükség
	glDeleteShader( vs_ID );
	glDeleteShader( fs_ID );

	return true;
}

void CMyApp::Clean()
{
	glDeleteProgram( m_programID );
}

void CMyApp::Update()
{
}


void CMyApp::Render()
{
	// töröljük a frampuffert (GL_COLOR_BUFFER_BIT) és a mélységi Z puffert (GL_DEPTH_BUFFER_BIT)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// shader bekapcsolasa --> a kirajzolast az altalunk keszitett programmal (a shader programmal) szeretnenk vegrehajtani, tehat a modositott pipeline-unkon menjen vegig az adat
	glUseProgram( m_programID );

	glDrawArrays(	GL_TRIANGLE_STRIP,0,4);			
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
	glViewport(0, 0, _w, _h );
}
