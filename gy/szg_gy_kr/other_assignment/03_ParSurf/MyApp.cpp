#include "MyApp.h"
#include "GLUtils.hpp"

#include <math.h>

CMyApp::CMyApp(void)
{
	m_vaoID = 0;
	m_vboID = 0;
	m_ibID = 0;

	m_programID = 0;

	speedUp = false;
	slowDown = false;
}

glm::vec3 CMyApp::GetConePos(float u, float v)
{
	float h = 2;
	float r = 1;
	
	u *= h;
	v *= 2 * 3.1415f;
	
	float mul = (h - u) / h;
	return glm::vec3(u, mul*r*cosf(v), mul*r*sinf(v));
}

glm::vec3 CMyApp::GetCylinderPos(float u, float v)
{
	float h = 2;
	float r = 1;
	
	u *= h;
	v *= 2 * 3.1415f;

	return glm::vec3(r*cosf(v), u, r*sinf(v));
}



CMyApp::~CMyApp(void)
{
}

glm::vec3 CMyApp::GetPlanePos(float u, float v)
{
	return glm::vec3(8*u, 0,-8 * v);
}

glm::vec3 CMyApp::GetTorusPos(float u, float v)
{
	u *= 2 * 3.1415f;
	v *= 2 * 3.1415f;
	float cu = cosf(u), su = sinf(u), cv = cosf(v), sv = sinf(v);
	float r = 0.5;
	float R = 1.5;
	return glm::vec3((R + r * cu)*cv, r*su, (R + r * cu)*sv);
}

glm::vec3 CMyApp::GetDiscPos(float u, float v)
{
	float r = 1;
	v *= 2 * 3.1415f;
	
	return glm::vec3(r*u*cosf(v), 0, r*u*sinf(v));
}

glm::vec3	CMyApp::GetUV(float u, float v)
{
	// orig� k�z�ppont�, egys�gsugar� g�mb parametrikus alakja: http://hu.wikipedia.org/wiki/G%C3%B6mb#Egyenletek 
	// figyelj�nk:	matematik�ban sokszor a Z tengely mutat felfel�, de n�lunk az Y, teh�t a legt�bb k�plethez k�pest n�lunk
	//				az Y �s Z koordin�t�k felcser�lve szerepelnek
	
	// G�mb 
	/*
	u *= 2*3.1415f;
	v *= 3.1415f;
	float cu = cosf(u), su = sinf(u), cv = cosf(v), sv = sinf(v);
	float r = 2;
	return glm::vec3( r*cu*sv, r*cv, r*su*sv );
	*/
	// N�gyzet 
	return glm::vec3(u, 0, v);

	//K�r
	/*
	u *= 2 * 3.14159265359f;
	v *= 3.14159265359f;
	float cu = cosf(u), su = sinf(u), cv = cosf(v), sv = sinf(v);
	return glm::vec3(cu*sv, 0, su*sv);
	*/
	

	// Paraboloid 
	/*
	float x = 2 * u - 1;
	float y = 2 * v - 1;
	return glm::vec3(x, x*x+y*y, y);
	*/
	// T�rusz
	/*
	u *= 2*3.1415f;
	v *= 2*3.1415f;
	float cu = cosf(u), su = sinf(u), cv = cosf(v), sv = sinf(v);
	float r = 0.5;
	float R = 1.5;
	return glm::vec3( (R+r*cu)*cv, r*su, (R + r*cu)*sv);
	*/
	//hiperbolikus paraboloid
	/*
	u *= 1.5;
	v *= 1.5;
	return (glm::vec3(0.8*u, 0.3*u*v, 0.5*v));
	*/
	
	
	//hengerpal�st
	/*
	float r = 1;
	float h = 2;
	u *= 2 * 3.1415f;
	v *= h;
	return glm::vec3(r*cosf(u), v, r*sinf(u));
	*/
	
}

bool CMyApp::Init()
{
	// t�rl�si sz�n legyen k�kes
	glClearColor(0.125f, 0.25f, 0.5f, 1.0f);

	glEnable(GL_CULL_FACE); // kapcsoljuk be a hatrafele nezo lapok eldobasat
	glEnable(GL_DEPTH_TEST); // m�lys�gi teszt bekapcsol�sa (takar�s)
	//glCullFace(GL_BACK); // GL_BACK: a kamer�t�l "elfel�" n�z� lapok, GL_FRONT: a kamera fel� n�z� lapok

	//
	// geometria letrehozasa
	//

	// NxM darab n�gysz�ggel k�zel�tj�k a parametrikus fel�let�nket => (N+1)x(M+1) pontban kell ki�rt�kelni
	
	Vertex vert[(N+1)*(M+1)];
	for (int i=0; i<=N; ++i)
		for (int j=0; j<=M; ++j)
		{
			float u = i/(float)N;
			float v = j/(float)M;

			//vert[i + j * (N + 1)].p = GetConePos(u, v);
			vert[i + j * (N + 1)].p = GetDiscPos(u, v);
			//vert[i + j * (N + 1)].p = GetCylinderPos(u, v);


			//vert[i + j*(N + 1)].c = glm::normalize(GetConePos(u,v));
			vert[i + j * (N + 1)].c = glm::normalize(GetDiscPos(u, v));
			//vert[i + j * (N + 1)].c = glm::normalize(GetCylinderPos(u, v));

		
		}

	// indexpuffer adatai: NxM n�gysz�g = 2xNxM h�romsz�g = h�romsz�glista eset�n 3x2xNxM index
    GLushort indices[3*2*(N)*(M)];
	for (int i=0; i<N; ++i)
		for (int j=0; j<M; ++j)
		{
			indices[6*i + j*3*2*(N) + 0] = (i)		+ (j)*	(N+1);
			indices[6*i + j*3*2*(N) + 1] = (i+1)	+ (j)*	(N+1);
			indices[6*i + j*3*2*(N) + 2] = (i)		+ (j+1)*(N+1);
			indices[6*i + j*3*2*(N) + 3] = (i+1)	+ (j)*	(N+1);
			indices[6*i + j*3*2*(N) + 4] = (i+1)	+ (j+1)*(N+1);
			indices[6*i + j*3*2*(N) + 5] = (i)		+ (j+1)*(N+1);
		}


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
		(void*)(sizeof(glm::vec3)) );

	// index puffer l�trehoz�sa
	glGenBuffers(1, &m_ibID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindVertexArray(0); // felt�lt�tt�k a VAO-t, kapcsoljuk le
	glBindBuffer(GL_ARRAY_BUFFER, 0); // felt�lt�tt�k a VBO-t is, ezt is vegy�k le
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // felt�lt�tt�k a VBO-t is, ezt is vegy�k le

	//
	// shaderek bet�lt�se
	//
	GLuint vs_ID = loadShader(GL_VERTEX_SHADER,		"myVert.vert");
	GLuint fs_ID = loadShader(GL_FRAGMENT_SHADER,	"myFrag.frag");

	// a shadereket t�rol� program l�trehoz�sa
	m_programID = glCreateProgram();

	// adjuk hozz� a programhoz a shadereket
	glAttachShader(m_programID, vs_ID);
	glAttachShader(m_programID, fs_ID);

	// VAO-beli attrib�tumok hozz�rendel�se a shader v�ltoz�khoz
	// FONTOS: linkel�s el�tt kell ezt megtenni!
	glBindAttribLocation(	m_programID,	// shader azonos�t�ja, amib�l egy v�ltoz�hoz szeretn�nk hozz�rendel�st csin�lni
							0,				// a VAO-beli azonos�t� index
							"vs_in_pos");	// a shader-beli v�ltoz�n�v
	glBindAttribLocation( m_programID, 1, "vs_in_col");

	// illessz�k �ssze a shadereket (kimen�-bemen� v�ltoz�k �sszerendel�se stb.)
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

		std::cout << "[app.Init()] S�der Huba panasza: " << aSzoveg << std::endl;

		delete aSzoveg;
	}

	// mar nincs ezekre szukseg
	glDeleteShader( vs_ID );
	glDeleteShader( fs_ID );

	//
	// egy�b inicializ�l�s
	//

	// vet�t�si m�trix l�trehoz�sa
	m_matProj = glm::perspective( 45.0f, 640/480.0f, 1.0f, 1000.0f );

	// shader-beli transzform�ci�s m�trixok c�m�nek lek�rdez�se
	m_loc_mvp = glGetUniformLocation( m_programID, "MVP");

	return true;
}

void CMyApp::Clean()
{
	glDeleteBuffers(1, &m_vboID);
	glDeleteBuffers(1, &m_ibID);
	glDeleteVertexArrays(1, &m_vaoID);

	glDeleteProgram( m_programID );
}

void CMyApp::Update()
{
	// n�zeti transzform�ci� be�ll�t�sa
	float t = SDL_GetTicks()/1000.0f;
	/* Forgat * /
	m_matView = glm::lookAt(glm::vec3( 5*cosf(t),  5,  5*sinf(t)),		// honnan n�zz�k a sz�nteret
							glm::vec3( 0,  0,  0),		// a sz�nt�r melyik pontj�t n�zz�k
							glm::vec3( 0,  1,  0));		// felfel� mutat� ir�ny a vil�gban
	/* �ll */

	m_matView = glm::lookAt(glm::vec3(6, 6, 12),		// honnan n�zz�k a sz�nteret
		glm::vec3(0, 0, 0),		// a sz�nt�r melyik pontj�t n�zz�k
		glm::vec3(0, 1, 0));		// felfel� mutat� ir�ny a vil�gban
}


void CMyApp::Render()
{
	// t�r�lj�k a frampuffert (GL_COLOR_BUFFER_BIT) �s a m�lys�gi Z puffert (GL_DEPTH_BUFFER_BIT)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// shader bekapcsolasa
	glUseProgram( m_programID );

	float t = SDL_GetTicks() / 1000.0;
	float v;
	if (speedUp) v = 4;
	else if (slowDown) v = 0.3;
	else v = 1;


	m_matWorld =
					glm::rotate<float>(t * 2*M_PI, glm::vec3(sin(t/3),cos(t/2),0));

	glm::mat4 mvp = m_matProj * m_matView * m_matWorld;

	// majd k�ldj�k �t a megfelel� m�trixot!
	glUniformMatrix4fv( m_loc_mvp,1,GL_FALSE,&(mvp[0][0]) ); 

	// kapcsoljuk be a VAO-t (a VBO j�n vele egy�tt)
	glBindVertexArray(m_vaoID);

	// kirajzol�s
	glDrawElements(	GL_TRIANGLES,3*2*(N)*(M),GL_UNSIGNED_SHORT,	0);					

	// VAO kikapcsolasa
	glBindVertexArray(0);

	// shader kikapcsolasa
	glUseProgram( 0 );
}

void CMyApp::KeyboardDown(SDL_KeyboardEvent& key)
{
	if (key.keysym.sym == SDLK_0) speedUp = true;
	if (key.keysym.sym == SDLK_1) slowDown = true;
}

void CMyApp::KeyboardUp(SDL_KeyboardEvent& key)
{
	if (key.keysym.sym == SDLK_0) speedUp = false;
	if (key.keysym.sym == SDLK_1) slowDown = false;
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

	m_matProj = glm::perspective(  45.0f,		// 90 fokos nyilasszog
									_w/(float)_h,	// ablakmereteknek megfelelo nezeti arany
									0.01f,			// kozeli vagosik
									100.0f);		// tavoli vagosik
}