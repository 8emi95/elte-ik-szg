#include "MyApp.h"
#include "GLUtils.hpp"

#include <GL/GLU.h>
#include <math.h>

#include <vector>

CMyApp::CMyApp(void)
{
	m_vaoID = 0;
	m_vboID = 0;
	m_programID = 0;
	m_atomicTextureID = 0;
	m_donotTextureID = 0;

}


CMyApp::~CMyApp(void)
{
}

/*
GLuint CMyApp::GenTexture()
{
    unsigned char tex[256][256][3];
 
    for (int i=0; i<256; ++i)
        for (int j=0; j<256; ++j)
        {
			tex[i][j][0] = rand()%256;
			tex[i][j][1] = rand()%256;
			tex[i][j][2] = rand()%256;
        }
 
	GLuint tmpID;

	// gener�ljunk egy text�ra er�forr�s nevet
    glGenTextures(1, &tmpID);
	// aktiv�ljuk a most gener�lt nev� text�r�t
    glBindTexture(GL_TEXTURE_2D, tmpID);
	// t�lts�k fel adatokkal az...
    gluBuild2DMipmaps(  GL_TEXTURE_2D,	// akt�v 2D text�r�t
						GL_RGB8,		// a v�r�s, z�ld �s k�k csatorn�kat 8-8 biten t�rolja a text�ra
						256, 256,		// 256x256 m�ret� legyen
						GL_RGB,				// a text�ra forr�sa RGB �rt�keket t�rol, ilyen sorrendben
						GL_UNSIGNED_BYTE,	// egy-egy sz�nkopmonenst egy unsigned byte-r�l kell olvasni
						tex);				// �s a text�ra adatait a rendszermem�ria ezen szeglet�b�l t�lts�k fel
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// biline�ris sz�r�s kicsiny�t�skor
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	// �s nagy�t�skor is
	glBindTexture(GL_TEXTURE_2D, 0);

	return tmpID;
}
*/

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

	/*
	Vertex vert[] =
	{ 
		//          x,  y, z             R, G, B			 s, t
		{glm::vec3(-1, -1, 0), glm::vec3(1, 0, 0), glm::vec2(0, 0)},
		{glm::vec3( 1, -1, 0), glm::vec3(0, 1, 0), glm::vec2(1, 0)},
		{glm::vec3(-1,  1, 0), glm::vec3(0, 0, 1), glm::vec2(0, 1)},
		{glm::vec3( 1,  1, 0), glm::vec3(1, 1, 1), glm::vec2(1, 1)},
	};
	*/

	// defini�ljuk a k�rlap cs�csait
	std::vector<Vertex> vertex_vector;
	
	// egy Vertex ker�l k�z�pre (vegyy�k �szre, hogy a text�ra k�zepe a (0.5, 0.5) koordin�ta!)
	//									X  Y  Z				R  G  B				S    T
	vertex_vector.push_back({ glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec2(0.5, 0.5) });
	// Megjegyz�s: a modell RGB sz�n�t nem haszn�ljuk (l�sd fragment shader k�dja), csak a text�r�b�l
	// sz�rmaz� sz�nt jelen�tj�k meg. Val�j�ban ki is t�r�lhetn�nk a Vertexek sz�n-attrib�tum�t.
	

	// Lerakunk N darab Vertexet egy 2 sugar�, orig� k�z�ppont� k�rvonal ment�n.
	// A text�ra-koordin�t�k egy (0.5, 0.5) k�z�ppont�, 0.5 sugar� k�rlapot k�vetnek! (A text�ra k�zep�b�l kiv�gunk egy maxim�lis sugar� k�rlapot.)
	// Teh�t (s,t) text�ra-koordin�ta egy Vertex eset�n s = 0.5*cosf(a) + 0.5, �s t = 0.5*sinf(a) + 0.5.
	// Csakhogy a text�ra-koordinarendszerben az orig� a bal als� sarok! Viszont a k�pet beolvas�skor fel�lr�l lefel� olvassuk.
	// Vagyis az y tengely t�kr�z�tt a modell-koordin�tarendszerhez k�pest.
	// Azaz alapb�l fejjel lefel� jelenne meg a text�ra.
	// Ez�rt a Vertexek (s,t) koordin�t�k helyett (s, 1-t) koordin�t�kat haszn�lunk.
	for (int i = 0; i < N; ++i){
		float a = i * 2 * 3.14152f / N; // sz�g egy adott vertexhez (radi�nban)

		//									X		   Y		  Z				R  G  B				S					 T
		vertex_vector.push_back({ glm::vec3(2*cosf(a), 2*sinf(a), 0), glm::vec3(0, 0, 0), glm::vec2(0.5 * cosf(a) + 0.5, 1 - (0.5 * sinf(a) + 0.5)) });
	}

	// indexpuffer adatai
	std::vector<GLushort> index_vector;
	
	// Kirajzol�shoz GL_TRIANGLES-t haszn�lunk.
	// Ez�rt az index pufferbe beleker�l az els� N-1 h�romsz�g... (k�r�lj�r�si ir�nyra �gyelj�nk!)
	for (int i = 0; i < N-1; ++i){
		index_vector.push_back(0);
		index_vector.push_back(i+1);
		index_vector.push_back(i+2);
	}
	// ...�s v�g�l az utols� h�romsz�g, ahol k�rbe�r�nk az indexekben
	index_vector.push_back(0);
	index_vector.push_back(N);
	index_vector.push_back(1);
	// Megjegyz�s: egyszer�bb lenne az index_vector, ha nem GL_TRIANGLES-t, hanem GL_TRIANGLE_FAN-t haszn�ln�nk.
	// Gyakorl�sk�pp m�dos�tsuk a programunkat ennek megfelel�en, hogy a kirajzolt eredm�ny ugyanaz maradjon!

	/*
	// indexpuffer adatai
    GLushort indices[]=
    {
		// 1. h�romsz�g
        0,1,2,
		// 2. h�romsz�g
        2,1,3,
    };
	*/

	// 1 db VAO foglalasa
	glGenVertexArrays(1, &m_vaoID);
	// a frissen gener�lt VAO beallitasa akt�vnak
	glBindVertexArray(m_vaoID);
	
	// hozzunk l�tre egy �j VBO er�forr�s nevet
	glGenBuffers(1, &m_vboID); 
	glBindBuffer(GL_ARRAY_BUFFER, m_vboID); // tegy�k "akt�vv�" a l�trehozott VBO-t
	// t�lts�k fel adatokkal az akt�v VBO-t
	glBufferData( GL_ARRAY_BUFFER,	// az akt�v VBO-ba t�lts�nk adatokat
				  vertex_vector.size() * sizeof(Vertex),		// ennyi b�jt nagys�gban
				  &vertex_vector[0],	// err�l a rendszermem�riabeli c�mr�l olvasva
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
		(void*)(2*sizeof(glm::vec3)) );

	// index puffer l�trehoz�sa
	glGenBuffers(1, &m_ibID);
	// a VAO �szreveszi, hogy bind-olunk egy index puffert �s feljegyzi, hogy melyik volt ez!
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_vector.size() * sizeof(GLushort), &index_vector[0], GL_STATIC_DRAW);

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
	
	m_loc_texture1 = glGetUniformLocation( m_programID, "texImage1" );
	m_loc_texture2 = glGetUniformLocation( m_programID, "texImage2" );
	m_loc_t =		 glGetUniformLocation( m_programID, "t" );

	//
	// egy�b er�forr�sok bet�lt�se
	//

	// text�ra bet�lt�se
	m_atomicTextureID = TextureFromFile("atomic.bmp");
	m_donotTextureID  = TextureFromFile("donot.bmp");

	return true;
}

void CMyApp::Clean()
{
	glDeleteTextures(1, &m_atomicTextureID);
	glDeleteTextures(1, &m_donotTextureID);

	glDeleteBuffers(1, &m_vboID);
	glDeleteVertexArrays(1, &m_vaoID);

	glDeleteProgram( m_programID );
}

void CMyApp::Update()
{
	// n�zeti transzform�ci� be�ll�t�sa
	m_matView = glm::lookAt(glm::vec3( 0,  0,  5),		// honnan n�zz�k a sz�nteret
							glm::vec3( 0,  0,  0),		// a sz�nt�r melyik pontj�t n�zz�k
							glm::vec3( 0,  1,  0));		// felfel� mutat� ir�ny a vil�gban
}


void CMyApp::Render()
{

	if (draw_lines){
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	if (draw_back_faces){
		glEnable(GL_CULL_FACE);
	}
	else {
		glDisable(GL_CULL_FACE);
	}

	// t�r�lj�k a frampuffert (GL_COLOR_BUFFER_BIT) �s a m�lys�gi Z puffert (GL_DEPTH_BUFFER_BIT)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// shader bekapcsolasa
	glUseProgram( m_programID );

	// shader parameterek be�ll�t�sa
	/*

	GLM transzform�ci�s m�trixokra p�ld�k:
		glm::rotate<float>( sz�g, tengely_x, tengely_y, tengely_z) <- tengely_{xyz} k�r�li elforgat�s
		glm::translate<float>( eltol_x, eltol_y, eltol_z) <- eltol�s
		glm::scale<float>( s_x, s_y, s_z ) <- l�pt�kez�s

	*/

	// rot sz�ggel forgatjuk el az objektumot
	// 360										-> egy teljes k�r (fokban)
	// 360 * SDL_GetTicks()						-> mp-k�nt 1000-szer forog k�rbe (mp-�nt �rj�k el 360 eg�sz t�bbsz�r�seit)
	// 360 * SDL_GetTicks() / 1000.0f			-> mp-k�nt 1-szer forog k�rbe
	// 360 * SDL_GetTicks() / 1000.0f / 10.0f	-> mp-k�nt 1/10-szer forog k�rbe, vagyis 10 mp-k�nt forog k�rbe teljesen
	float rot = 360 * SDL_GetTicks() / 1000.0f / 10.0f;

	m_matWorld = glm::rotate<float>(rot,glm::vec3( 0, 1, 0));

	glm::mat4 mvp = m_matProj * m_matView * m_matWorld;

	// majd k�ldj�k �t a megfelel� m�trixot!
	glUniformMatrix4fv( m_loc_mvp,// erre a helyre t�lts�nk �t adatot
						1,			// egy darab m�trixot
						GL_FALSE,	// NEM transzpon�lva
						&(mvp[0][0]) ); // innen olvasva a 16 x sizeof(float)-nyi adatot
	
	// aktiv�ljuk a 0-�s text�ra mintav�telez� egys�get
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_atomicTextureID);
	// aktiv�ljuk a gener�lt text�r�nkat 
	// text�ra mintav�telez� �s shader-beli sampler2D �sszerendel�se
	glUniform1i(	m_loc_texture1,	// ezen azonos�t�j� sampler 2D
					0);				// olvassa az ezen index� mintav�telez�t

	// az 1-es text�ra mintav�telez� egys�ghez m�sik text�ra tartozik
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_donotTextureID);
	glUniform1i(m_loc_texture2, 1);

	// t �rt�ke -1 �s +1 k�z�tt cikliz�l, ha sinf-be rakunk egy id�ben v�ltoz� �rt�ket!
	// Ne felejts�k el, hogy a sinf() radi�nt v�r!
	// 2 * 3.14152f										-> egy teljes peri�dus (radi�nban 2*PI)
	// 2 * 3.14152f * SDL_GetTicks()					-> mp-k�nt 1000 peri�dus
	// 2 * 3.14152f * SDL_GetTicks() / 1000.0f			-> mp-k�nt 1 peri�dus
	// 2 * 3.14152f * SDL_GetTicks() / 1000.0f / 6.0f	-> mp-k�nt 1/6 peri�dus, azaz 6 mp-k�nt 1 peri�dus
	float t = sinf(2 * 3.14152f * SDL_GetTicks() / 1000.0f / 4.0f);
	
	// t �rt�k�t sk�l�znunk kell, hogy ne a [-1; +1], hanem a [0; +1] tartom�nyban cikliz�ljon
	t = (t + 1) / 2.0f;

	std::cout << "t erteke: " << t << "\n";
	glUniform1f(m_loc_t, t);

	// kapcsoljuk be a VAO-t (a VBO j�n vele egy�tt)
	glBindVertexArray(m_vaoID);

	// kirajzol�s
	glDrawElements(	GL_TRIANGLES,		// primit�v t�pus
					3*N,				// hany csucspontot hasznalunk a kirajzolashoz
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
	if (mouse.button == SDL_BUTTON_RIGHT){
		draw_lines = !draw_lines;
	}
	if (mouse.button == SDL_BUTTON_LEFT){
		draw_back_faces = !draw_back_faces;
	}
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