#include "MyApp.h"
#include "GLUtils.hpp"

#include <math.h>

CMyApp::CMyApp(void)
{
	m_vaoID = 0;
	m_vboID = 0;
	m_ibID = 0;

	m_programID = 0;
}


CMyApp::~CMyApp(void)
{
}

//
// egy parametrikus fel�let (u,v) param�ter�rt�kekhez tartoz� pontj�nak
// kisz�m�t�s�t v�gz� f�ggv�ny
//
glm::vec3	CMyApp::GetPos(float u, float v)
{
	// orig� k�z�ppont�, egys�gsugar� g�mb parametrikus alakja: http://hu.wikipedia.org/wiki/G%C3%B6mb#Egyenletek 
	// figyelj�nk:	matematik�ban sokszor a Z tengely mutat felfel�, de n�lunk az Y, teh�t a legt�bb k�plethez k�pest n�lunk
	//				az Y �s Z koordin�t�k felcser�lve szerepelnek
	u *= 2*3.1415f;
	v *= 2*3.1415f;
	float cu = cosf(u), su = sinf(u), cv = cosf(v), sv = sinf(v);
	float r = 2;
	float R = 4;

	return glm::vec3( (R+r*cu)*cv, r*su, (R+r*cu)*sv );
}

void CMyApp::InitTorusz()
{
	//
	// geometria letrehozasa
	//

	// NxM darab n�gysz�ggel k�zel�tj�k a parametrikus fel�let�nket => (N+1)x(M+1) pontban kell ki�rt�kelni
	Vertex vert[(N + 1) * (M + 1)];
	for (int i = 0; i <= N; ++i)
		for (int j = 0; j <= M; ++j)
		{
			float u = i / (float)N;
			float v = j / (float)M;

			vert[i + j * (N + 1)].p = GetPos(u, v);
			vert[i + j * (N + 1)].c = glm::normalize(vert[i + j * (N + 1)].p);
		}

	// indexpuffer adatai: NxM n�gysz�g = 2xNxM h�romsz�g = h�romsz�glista eset�n 3x2xNxM index
	GLushort indices[3 * 2 * (N) * (M)];
	for (int i = 0; i < N; ++i)
		for (int j = 0; j < M; ++j)
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
			indices[6 * i + j * 3 * 2 * (N)+0] = (i)+(j) * (N + 1);
			indices[6 * i + j * 3 * 2 * (N)+1] = (i + 1) + (j) * (N + 1);
			indices[6 * i + j * 3 * 2 * (N)+2] = (i)+(j + 1) * (N + 1);
			indices[6 * i + j * 3 * 2 * (N)+3] = (i + 1) + (j) * (N + 1);
			indices[6 * i + j * 3 * 2 * (N)+4] = (i + 1) + (j + 1) * (N + 1);
			indices[6 * i + j * 3 * 2 * (N)+5] = (i)+(j + 1) * (N + 1);
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

	// index puffer l�trehoz�sa
	glGenBuffers(1, &m_ibID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindVertexArray(0); // felt�lt�tt�k a VAO-t, kapcsoljuk le
	glBindBuffer(GL_ARRAY_BUFFER, 0); // felt�lt�tt�k a VBO-t is, ezt is vegy�k le
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // felt�lt�tt�k a VBO-t is, ezt is vegy�k le
}

void CMyApp::InitGula()
{
	//
	// geometria letrehozasa
	//

	// NxM darab n�gysz�ggel k�zel�tj�k a parametrikus fel�let�nket => (N+1)x(M+1) pontban kell ki�rt�kelni
	Vertex vert[gulaN + 2];
	vert[0] = { glm::vec3(0,1,0),glm::vec3(atan(0) / M_PI + 0.5,atan(1) / M_PI + 0.5,atan(0) / M_PI + 0.5) };
	for (int i = 0; i < gulaN; ++i)
	{
		float alpha = 2 * M_PI * i / gulaN;
		vert[i + 1] = { glm::vec3(cos(alpha),0,sin(alpha)),glm::vec3(atan(cos(alpha)) / M_PI + 0.5,atan(0) / M_PI + 0.5,atan(sin(alpha)) / M_PI + 0.5) };
	}
	vert[gulaN + 1] = { glm::vec3(0,0,0),glm::vec3(atan(0) / M_PI + 0.5,atan(0) / M_PI + 0.5,atan(0) / M_PI + 0.5) };

	// indexpuffer adatai: NxM n�gysz�g = 2xNxM h�romsz�g = h�romsz�glista eset�n 3x2xNxM index
	GLushort indices[3 * 2 * gulaN];
	for (int i = 0; i < gulaN; ++i)
	{
		indices[3 * i + 0] = 0;
		indices[3 * i + 1] = (i+1)%gulaN+1;
		indices[3 * i + 2] = i+1;

		indices[3 * gulaN + 3 * i + 0] = gulaN+1;
		indices[3 * gulaN + 3 * i + 1] = i + 1;
		indices[3 * gulaN + 3 * i + 2] = (i + 1) % gulaN + 1;
	}

	// 1 db VAO foglalasa
	glGenVertexArrays(1, &m_gula_vaoID);
	// a frissen gener�lt VAO beallitasa akt�vnak
	glBindVertexArray(m_gula_vaoID);

	// hozzunk l�tre egy �j VBO er�forr�s nevet
	glGenBuffers(1, &m_gula_vboID);
	glBindBuffer(GL_ARRAY_BUFFER, m_gula_vboID); // tegy�k "akt�vv�" a l�trehozott VBO-t
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

	// index puffer l�trehoz�sa
	glGenBuffers(1, &m_gula_ibID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_gula_ibID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindVertexArray(0); // felt�lt�tt�k a VAO-t, kapcsoljuk le
	glBindBuffer(GL_ARRAY_BUFFER, 0); // felt�lt�tt�k a VBO-t is, ezt is vegy�k le
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // felt�lt�tt�k a VBO-t is, ezt is vegy�k le
}

void CMyApp::InitHasab()
{
	//
	// geometria letrehozasa
	//

	// NxM darab n�gysz�ggel k�zel�tj�k a parametrikus fel�let�nket => (N+1)x(M+1) pontban kell ki�rt�kelni
	Vertex vert[2*hasabN+2+2*(hasabN+2)];
	for (int i = 0; i <= hasabN; ++i)
	{
		float alpha = 2 * M_PI * i / hasabN;
		vert[2*i + 0] = { glm::vec3(cos(alpha),0,sin(alpha)),glm::vec3(atan(cos(alpha)) / M_PI + 0.5,atan(0) / M_PI + 0.5,atan(sin(alpha)) / M_PI + 0.5) };
		vert[2*i + 1] = { glm::vec3(cos(alpha),3,sin(alpha)),glm::vec3(atan(cos(alpha)) / M_PI + 0.5,atan(3) / M_PI + 0.5,atan(sin(alpha)) / M_PI + 0.5) };
	}


	vert[2 * hasabN + 2] = { glm::vec3(0,3,0),glm::vec3(atan(0) / M_PI + 0.5,atan(3) / M_PI + 0.5,atan(0) / M_PI + 0.5) };
	vert[2 * hasabN + 2+hasabN+2] = { glm::vec3(0,0,0),glm::vec3(atan(0) / M_PI + 0.5,atan(0) / M_PI + 0.5,atan(0) / M_PI + 0.5) };

	for (int i = 0; i <= hasabN; ++i)
	{
		float alpha = -2 * M_PI * i / hasabN;
		vert[2 * hasabN + 2+1+i] = { glm::vec3(cos(alpha),3,sin(alpha)),glm::vec3(atan(cos(alpha)) / M_PI + 0.5,atan(3) / M_PI + 0.5,atan(sin(alpha)) / M_PI + 0.5) };
		alpha *= -1;
		vert[2 * hasabN + 2 + hasabN + 2+1+i] = { glm::vec3(cos(alpha),0,sin(alpha)),glm::vec3(atan(cos(alpha)) / M_PI + 0.5,atan(0) / M_PI + 0.5,atan(sin(alpha)) / M_PI + 0.5) };
	}

	// 1 db VAO foglalasa
	glGenVertexArrays(1, &m_hasab_vaoID);
	// a frissen gener�lt VAO beallitasa akt�vnak
	glBindVertexArray(m_hasab_vaoID);

	// hozzunk l�tre egy �j VBO er�forr�s nevet
	glGenBuffers(1, &m_hasab_vboID);
	glBindBuffer(GL_ARRAY_BUFFER, m_hasab_vboID); // tegy�k "akt�vv�" a l�trehozott VBO-t
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


	glBindVertexArray(0); // felt�lt�tt�k a VAO-t, kapcsoljuk le
	glBindBuffer(GL_ARRAY_BUFFER, 0); // felt�lt�tt�k a VBO-t is, ezt is vegy�k le
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // felt�lt�tt�k a VBO-t is, ezt is vegy�k le
}

bool CMyApp::Init()
{
	// t�rl�si sz�n legyen k�kes
	glClearColor(0.125f, 0.25f, 0.5f, 1.0f);

	glEnable(GL_CULL_FACE); // kapcsoljuk be a hatrafele nezo lapok eldobasat
	glEnable(GL_DEPTH_TEST); // m�lys�gi teszt bekapcsol�sa (takar�s)
	glCullFace(GL_BACK); // GL_BACK: a kamer�t�l "elfel�" n�z� lapok, GL_FRONT: a kamera fel� n�z� lapok

	InitTorusz();
	InitGula();
	InitHasab();

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
	if (GL_FALSE == result || infoLogLength != 0)
	{
		std::vector<char> error_massage(infoLogLength);
		glGetProgramInfoLog(m_programID, infoLogLength, NULL, error_massage.data());

		std::cout << "Hiba a shader letrehozasakor: " << error_massage.data() << std::endl;
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
	m_matView = glm::lookAt(glm::vec3( 15,  10,  15),		// honnan n�zz�k a sz�nteret
							glm::vec3( 0,  0,  0),		// a sz�nt�r melyik pontj�t n�zz�k
							glm::vec3( 0,  1,  0));		// felfel� mutat� ir�ny a vil�gban
}

void CMyApp::DrawTorusz(glm::mat4 m_matWorld)
{
	// kapcsoljuk be a VAO-t (a VBO j�n vele egy�tt)
	glBindVertexArray(m_vaoID);

	glm::mat4 mvp = m_matProj * m_matView * m_matWorld;

	// majd k�ldj�k �t a megfelel� m�trixot!
	// Uniform v�ltoz� bindol�sa csak a program bindol�sa ut�n lehets�ges! (glUseProgram() )
	glUniformMatrix4fv(m_loc_mvp,// erre a helyre t�lts�nk �t adatot
		1,			// egy darab m�trixot
		GL_FALSE,	// NEM transzpon�lva
		&(mvp[0][0])); // innen olvasva a 16 x sizeof(float)-nyi adatot

	// kirajzol�s
	//A draw h�v�sokhoz a VAO �s a program bindolva kell legyenek (glUseProgram() �s glBindVertexArray())

	glDrawElements(GL_TRIANGLES,		// primit�v t�pus
		3 * 2 * (N) * (M),		// hany csucspontot hasznalunk a kirajzolashoz
		GL_UNSIGNED_SHORT,	// indexek tipusa
		0);					// indexek cime


	// VAO kikapcsolasa
	glBindVertexArray(0);

}

void CMyApp::DrawGula(glm::mat4 m_matWorld)
{
	// kapcsoljuk be a VAO-t (a VBO j�n vele egy�tt)
	glBindVertexArray(m_gula_vaoID);

	glm::mat4 mvp = m_matProj * m_matView * m_matWorld;

	glUniformMatrix4fv(m_loc_mvp,// erre a helyre t�lts�nk �t adatot
		1,			// egy darab m�trixot
		GL_FALSE,	// NEM transzpon�lva
		&(mvp[0][0])); // innen olvasva a 16 x sizeof(float)-nyi adatot

	glDrawElements(GL_TRIANGLES,		// primit�v t�pus
		3 * 2*(gulaN),		// hany csucspontot hasznalunk a kirajzolashoz
		GL_UNSIGNED_SHORT,	// indexek tipusa
		0);					// indexek cime
	glBindVertexArray(0);
}


void CMyApp::DrawHasab(glm::mat4 m_matWorld)
{
	// kapcsoljuk be a VAO-t (a VBO j�n vele egy�tt)
	glBindVertexArray(m_hasab_vaoID);

	glm::mat4 mvp = m_matProj * m_matView * m_matWorld;

	glUniformMatrix4fv(m_loc_mvp,// erre a helyre t�lts�nk �t adatot
		1,			// egy darab m�trixot
		GL_FALSE,	// NEM transzpon�lva
		&(mvp[0][0])); // innen olvasva a 16 x sizeof(float)-nyi adatot

	glDrawArrays(GL_TRIANGLE_STRIP,0,2*hasabN+2);
	glDrawArrays(GL_TRIANGLE_FAN, 2 * hasabN + 2,hasabN+2);
	glDrawArrays(GL_TRIANGLE_FAN, 2 * hasabN + 2+hasabN+2,hasabN+2);
	glBindVertexArray(0);
}

void CMyApp::Render()
{
	// t�r�lj�k a frampuffert (GL_COLOR_BUFFER_BIT) �s a m�lys�gi Z puffert (GL_DEPTH_BUFFER_BIT)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// shader bekapcsolasa, ebben a projektben a teljes programot jel�li, hisz nem v�ltunk a shaderek k�z�tt
	glUseProgram( m_programID );

	// shader parameterek be�ll�t�sa
	/*

	GLM transzform�ci�s m�trixokra p�ld�k:
		glm::rotate<float>( sz�g, glm::vec3(tengely_x, tengely_y, tengely_z) ) <- tengely_{xyz} k�r�li elforgat�s
		glm::translate<float>( glm::vec3(eltol_x, eltol_y, eltol_z) ) <- eltol�s
		glm::scale<float>( glm::vec3(s_x, s_y, s_z) ) <- l�pt�kez�s

	*/
	m_matWorld = glm::mat4(1.0f);

	DrawTorusz(m_matWorld);

	float t = SDL_GetTicks() / 1000.0;

	for (int i = 0; i < num; ++i)
	{
		float alpha = 2 * M_PI * i / num;
		m_matWorld = glm::rotate<float>(t / 4+alpha, glm::vec3(0, 1, 0))
			* glm::translate(glm::vec3(4, 2.6, 0))
			* glm::rotate<float>(M_PI / 2, glm::vec3(0, 0, 1))
			* glm::rotate<float>(t, glm::vec3(0, 1, 0))
			* glm::scale<float>(glm::vec3(0.1));
		DrawTorusz(m_matWorld);
	}

	float x = 5 * sin(2 * M_PI * t / 10);
	float y = 0.1 * x * x * x  +0.8 * x + 0.2;

	m_matWorld = glm::translate(glm::vec3(x, y, 0)) * glm::rotate<float>(t, glm::vec3(1, 0, 0));

	DrawGula(m_matWorld);

	m_matWorld = glm::translate(glm::vec3(0,7, 0)) * glm::rotate<float>(t, glm::vec3(1, 0, 0));

	DrawHasab(m_matWorld);

	// shader kikapcsolasa
	glUseProgram( 0 );
}

void CMyApp::KeyboardDown(SDL_KeyboardEvent& key)
{
	switch (key.keysym.sym)
	{
	case SDLK_KP_PLUS:
		num++;
		break;
		//Ide a - os r�sz, 0 al� ne lehessen menni.
	default:
		break;
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

// a k�t param�terbe az �j ablakm�ret sz�less�ge (_w) �s magass�ga (_h) tal�lhat�
void CMyApp::Resize(int _w, int _h)
{
	glViewport(0, 0, _w, _h);

	m_matProj = glm::perspective(  45.0f,		// 90 fokos nyilasszog
									_w/(float)_h,	// ablakmereteknek megfelelo nezeti arany
									0.01f,			// kozeli vagosik
									100.0f);		// tavoli vagosik
}