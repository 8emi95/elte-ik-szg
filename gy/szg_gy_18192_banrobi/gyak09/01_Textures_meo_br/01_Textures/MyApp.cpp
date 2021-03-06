#include "MyApp.h"
#include "GLUtils.hpp"

#include <math.h>

CMyApp::CMyApp(void)
{
	m_vaoID = 0;
	m_vboID = 0;
	m_ibID = 0;

	m_programID = 0;

	m_generatedTextureID = 0;
	m_loadedTextureID = 0;
	m_samplerID = 0;
}


CMyApp::~CMyApp(void)
{
}

GLuint CMyApp::GenTexture()
{
	unsigned char tex[256][256][3];

	for (int i = 0; i<256; ++i)
		for (int j = 0; j<256; ++j)
		{
			tex[i][j][0] = rand() % 256;
			tex[i][j][1] = rand() % 256;
			tex[i][j][2] = rand() % 256;
		}

	GLuint tmpID;

	// gener�ljunk egy text�ra er�forr�s nevet
	glGenTextures(1, &tmpID);
	// aktiv�ljuk a most gener�lt nev� text�r�t
	glBindTexture(GL_TEXTURE_2D, tmpID);
	// t�lts�k fel adatokkal
	glTexImage2D(GL_TEXTURE_2D,		// melyik binding point-on van a text�ra er�forr�s, amihez t�rol�st rendel�nk
		0,					// melyik r�szletess�gi szint adatait hat�rozzuk meg
		GL_RGB,				// text�ra bels� t�rol�si form�tuma (GPU-n)
		256, 256,			// sz�less�g, magass�g
		0,					// nulla kell, hogy legyen ( https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glTexImage2D.xhtml )
		GL_RGB,				// forr�s (=CPU-n) form�tuma
		GL_UNSIGNED_BYTE,	// forr�s egy pixel�nek egy csatorn�j�t hogyan t�roljuk
		tex);				// forr�shoz pointer

	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);

	return tmpID;
}

//
// egy parametrikus fel�let (u,v) param�ter�rt�kekhez tartoz� pontj�nak
// kisz�m�t�s�t v�gz� f�ggv�ny
//
glm::vec3 CMyApp::GetPos(float u, float v)
{
	// orig� k�z�ppont�, egys�gsugar� g�mb parametrikus alakja: http://hu.wikipedia.org/wiki/G%C3%B6mb#Egyenletek 
	// figyelj�nk:	matematik�ban sokszor a Z tengely mutat felfel�, de n�lunk az Y, teh�t a legt�bb k�plethez k�pest n�lunk
	//				az Y �s Z koordin�t�k felcser�lve szerepelnek
	u *= 2 * 3.1415f;
	v *= 3.1415f;
	float cu = cosf(u), su = sinf(u), cv = cosf(v), sv = sinf(v);
	float r = 2;

	return glm::vec3(r*cu*sv, r*cv, r*su*sv);
}
//
// egy parametrikus fel�let (u,v) param�ter�rt�kekhez tartoz� norm�lvektor�nak
// kisz�m�t�s�t v�gz� f�ggv�ny
//
glm::vec3 CMyApp::GetNorm(float u, float v)
{
	// K�plettel
	u *= 2*3.1415f;
	v *= 3.1415f;
	float cu = cosf(u), su = sinf(u), cv = cosf(v), sv = sinf(v);
	return glm::vec3(cu*sv, cv, su*sv);
	
	// Numerikusan (nem kell ismerni a k�pletet, el�g a poz�ci��t)
	/*glm::vec3 uv = GetPos(u, v);
	glm::vec3 du = GetPos(u+0.01, v)-GetPos(u-0.01, v);
	glm::vec3 dv = GetPos(u, v+0.01)-GetPos(u, v-0.01);

	return (glm::cross(du, dv));*/
}

glm::vec2 CMyApp::GetTex(float u, float v)
{
	return glm::vec2(u, 1 - v);
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

	// NxM darab n�gysz�ggel k�zel�tj�k a parametrikus fel�let�nket => (N+1)x(M+1) pontban kell ki�rt�kelni
	Vertex vert[(N+1)*(M+1)];
	for (int i=0; i<=N; ++i)
		for (int j = 0; j <= M; ++j)
		{
			float u = i / (float)N;
			float v = j / (float)M;

			vert[i + j * (N + 1)].p = GetPos(u, v);
			vert[i + j * (N + 1)].n = GetNorm(u, v);
			vert[i + j * (N + 1)].t = GetTex(u, v);
		}

	// indexpuffer adatai: NxM n�gysz�g = 2xNxM h�romsz�g = h�romsz�glista eset�n 3x2xNxM index
    GLushort indices[3*2*(N)*(M)];
	for (int i=0; i<N; ++i)
		for (int j=0; j<M; ++j)
		{
			// minden n�gysz�gre csin�ljunk kett� h�romsz�get, amelyek a k�vetkez� 
			// (i,j) indexekn�l sz�letett (u_i, v_i) param�ter�rt�kekhez tartoz�
			// pontokat k�tik �ssze:
			//
			//		(i,j+1)
			//		  o-----o(i+1,j+1)
			//		  |\    |			a = p(u_i, v_i)
			//		  | \   |			b = p(u_{i+1}, v_i)
			//		  |  \  |			c = p(u_i, v_{i+1})
			//		  |   \ |			d = p(u_{i+1}, v_{i+1})
			//		  |	   \|
			//	(i,j) o-----o(i+1, j)
			//
			// - az (i,j)-hez tart�z� 1D-s index a VBO-ban: i+j*(N+1)
			// - az (i,j)-hez tart�z� 1D-s index az IB-ben: i*6+j*6*(N+1) 
			//		(mert minden n�gysz�gh�z 2db h�romsz�g = 6 index tartozik)
			//
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

	// text�rakoordin�t�k bekapcsol�sa a 2-es azonos�t�j� attrib�tom csatorn�n
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(
		2,
		2,
		GL_FLOAT,
		GL_FALSE,
		sizeof(Vertex),
		(void*)(2 * sizeof(glm::vec3)));

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
	glBindAttribLocation( m_programID, 2, "vs_in_tex0");

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
	m_loc_w = glGetUniformLocation( m_programID, "world" );
	m_loc_wit = glGetUniformLocation( m_programID, "WorldIT" );
	m_loc_tex = glGetUniformLocation(m_programID, "texImage");

	// MEO ------------------------------------------------------------------------------------------------------------------------

	m_loc_tex2 = glGetUniformLocation(m_programID, "texImage2");
	m_loc_ice = glGetUniformLocation(m_programID, "ice_level");
	m_loc_mix = glGetUniformLocation(m_programID, "mix_texture");

	// MEO ------------------------------------------------------------------------------------------------------------------------

	m_loadedTextureID = TextureFromFile("earth.png");
	m_generatedTextureID = GenTexture();
	m_samplerID = genSampler();
	m_samplerID2 = genSampler(); // MEO ------------------------------------------------------------------------------------------------------------------------

	return true;
}

void CMyApp::Clean()
{
	glDeleteTextures(1, &m_loadedTextureID);
	glDeleteTextures(1, &m_generatedTextureID);

	glDeleteSamplers(1, &m_samplerID);

	glDeleteBuffers(1, &m_vboID);
	glDeleteBuffers(1, &m_ibID);
	glDeleteVertexArrays(1, &m_vaoID);

	glDeleteProgram( m_programID );
}

void CMyApp::Update()
{
	// n�zeti transzform�ci� be�ll�t�sa
	float t = SDL_GetTicks()/1000.0f;
	m_matView = glm::lookAt(glm::vec3( 5*cosf(t),  5,  5*sinf(t)),		// honnan n�zz�k a sz�nteret
							glm::vec3( 0,  0,  0),		// a sz�nt�r melyik pontj�t n�zz�k
							glm::vec3( 0,  1,  0));		// felfel� mutat� ir�ny a vil�gban
}


void CMyApp::Render()
{
	// t�r�lj�k a frampuffert (GL_COLOR_BUFFER_BIT) �s a m�lys�gi Z puffert (GL_DEPTH_BUFFER_BIT)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// shader bekapcsolasa
	glUseProgram( m_programID );

	// shader parameterek be�ll�t�sa
	/*

	GLM transzform�ci�s m�trixokra p�ld�k:
		glm::rotate<float>( sz�g, glm::vec3(tengely_x, tengely_y, tengely_z) ) <- tengely_{xyz} k�r�li elforgat�s
		glm::translate<float>( glm::vec3(eltol_x, eltol_y, eltol_z) ) <- eltol�s
		glm::scale<float>( glm::vec3(s_x, s_y, s_z) ) <- l�pt�kez�s

	*/
	m_matWorld = glm::mat4(1.0f);

	glm::mat4 mvp = m_matProj * m_matView * m_matWorld;

	// majd k�ldj�k �t a megfelel� m�trixot!
	glUniformMatrix4fv( m_loc_mvp,// erre a helyre t�lts�nk �t adatot
						1,			// egy darab m�trixot
						GL_FALSE,	// NEM transzpon�lva
						&(mvp[0][0]) ); // innen olvasva a 16 x sizeof(float)-nyi adatot

	glm::mat4 WorldIT = glm::inverse(glm::transpose(m_matWorld));

	glUniformMatrix4fv(m_loc_wit, 1, GL_FALSE, &(WorldIT[0][0]));
	glUniformMatrix4fv(m_loc_w, 1, GL_FALSE, &(m_matWorld[0][0]));

	// 1. text�ra // MEO ---------------------------------------------------------------------------------------- csak komment
	glActiveTexture(GL_TEXTURE0 + 0 /*hw samplerindex*/);

	// 2 m�sodpercenk�nti v�lt�s a gener�lt �s a bet�lt�tt text�ra k�z�tt
	// Feladat: ehelyett shaderben fokozatos v�lt�s a k�t text�ra k�z�tt
	/*
	if (SDL_GetTicks() / 1000 % 2 == 0)
		glBindTexture(GL_TEXTURE_2D, m_loadedTextureID);
	else
		glBindTexture(GL_TEXTURE_2D, m_generatedTextureID);
	*/
	glBindTexture(GL_TEXTURE_2D, m_loadedTextureID); // MEO ------------------------------------------------------------------------------------------------------------------------

	glBindSampler(0 /*hw samplerindex*/, m_samplerID);

	// aktiv�ljuk a gener�lt text�r�nkat 
	// text�ra mintav�telez� �s shader-beli sampler2D �sszerendel�se
	glUniform1i(m_loc_tex,	// ezen azonos�t�j� sampler 2D
		0 /*hw samplerindex*/);	// olvassa az ezen index� mintav�telez�t

	
	// MEO ------------------------------------------------------------------------------------------------------------------------

	// 2. text�ra
	glActiveTexture(GL_TEXTURE0 + 1 /*hw samplerindex*/);
	glBindTexture(GL_TEXTURE_2D, m_generatedTextureID);
	glBindSampler(1 /*hw samplerindex*/, m_samplerID2);
	glUniform1i(m_loc_tex2, 1 /*hw samplerindex*/);

	// kapcsoljuk be a VAO-t (a VBO j�n vele egy�tt)
	glBindVertexArray(m_vaoID);

	float ice = sinf(SDL_GetTicks() / 1000.0f * 2 * float(M_PI) / 5.f) / 8.0f + 7.f / 8.f;
	//float ice = sin(SDL_GetTicks() / 1000.0 * 2 * M_PI / 5) / 8 + 7.0 / 8;
	glUniform1f(m_loc_ice, ice);

	float mix_texture = sin(SDL_GetTicks() / 1000.f * 2 * M_PI) / 2 + 0.5;
	glUniform1f(m_loc_mix, mix_texture);

	// MEO ------------------------------------------------------------------------------------------------------------------------

	// kirajzol�s
	glDrawElements(	GL_TRIANGLES,		// primit�v t�pus
					3*2*(N)*(M),		// hany csucspontot hasznalunk a kirajzolashoz
					GL_UNSIGNED_SHORT,	// indexek tipusa
					0);					// indexek cime

	// VAO kikapcsolasa
	glBindVertexArray(0);

	// text�ra kikapcsol�sa
	glBindTexture(GL_TEXTURE_2D, 0);

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

// a k�t param�terbe az �j ablakm�ret sz�less�ge (_w) �s magass�ga (_h) tal�lhat�
void CMyApp::Resize(int _w, int _h)
{
	glViewport(0, 0, _w, _h);

	m_matProj = glm::perspective(  45.0f,		// 90 fokos nyilasszog
									_w/(float)_h,	// ablakmereteknek megfelelo nezeti arany
									0.01f,			// kozeli vagosik
									100.0f);		// tavoli vagosik
}
