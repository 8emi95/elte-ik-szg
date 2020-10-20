#include "MyApp.h"
#include "GLUtils.hpp"

#include <math.h>

CMyApp::CMyApp(void)
{
	m_vaoID = 0;
	m_vboID = 0;
	m_ibID = 0;

	m_vaoFelgombID=0;
	m_vboFelgombID=0;
	m_ibFelgombID=0;

	m_vaoHengerID=0;
	m_vboHengerID=0;
	m_ibHengerID=0;


	m_vaoKiskorID=0; 
	m_vboKiskorID=0; 
	m_ibKiskorID=0;



	m_vaoHengerFejID=0;
	m_vboHengerFejID=0;
	m_ibHengerFejID=0;

	m_vaoHengerFarokID=0;
	m_vboHengerFarokID=0;
	m_ibHengerFarokID=0;

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

glm::vec3 CMyApp::GetPos(float u, float v)
{
	u *= 2 * 3.1415f;
	v *= 3.1415f/2;
	float cu = cosf(u), su = sinf(u), cv = cosf(v), sv = sinf(v);
	float r = 2;

	return glm::vec3(r*cu*sv, 0, r*su*sv);
}

glm::vec3 CMyApp::GetNorm(float u, float v)
{
	// K�plettel
	/*u *= 2*3.1415f;
	v *= 3.1415f/2;
	float cu = cosf(u), su = sinf(u), cv = cosf(v), sv = sinf(v);
	return glm::vec3(cu*sv, 0, su*sv);*/
	
	// Numerikusan (nem kell ismerni a k�pletet, el�g a poz�ci��t)
	glm::vec3 uv = GetPos(u, v);
	glm::vec3 du = GetPos(u+0.01, v)-GetPos(u-0.01, v);
	glm::vec3 dv = GetPos(u, v+0.01)-GetPos(u, v-0.01);

	return (glm::cross(du, dv));
}

glm::vec2 CMyApp::GetTex(float u, float v)
{
	return glm::vec2( u, v);
}


//-----------------------------------FELGOMB-----------------------------------

glm::vec3 CMyApp::GetPosFelgomb(float u, float v)
{
	u *= 2 * 3.1415f;
	v *= 3.1415f/2;
	float cu = cosf(u), su = sinf(u), cv = cosf(v), sv = sinf(v);
	float r = 2;

	return glm::vec3(r*sv, r*cv, r*su*sv);
}

glm::vec3 CMyApp::GetNormFelgomb(float u, float v)
{
	// K�plettel
	/*u *= 2*3.1415f;
	v *= 3.1415f/2;
	float cu = cosf(u), su = sinf(u), cv = cosf(v), sv = sinf(v);
	return glm::vec3(cu*sv, 0, su*sv);*/

	// Numerikusan (nem kell ismerni a k�pletet, el�g a poz�ci��t)
	glm::vec3 uv = GetPos(u, v);
	glm::vec3 du = GetPos(u + 0.01, v) - GetPos(u - 0.01, v);
	glm::vec3 dv = GetPos(u, v + 0.01) - GetPos(u, v - 0.01);

	return (glm::cross(du, dv));
}

glm::vec2 CMyApp::GetTexFelgomb(float u, float v)
{
	return glm::vec2(u, 1 - v);
}


//******************************************Henger***************************


glm::vec3 CMyApp::GetPosHenger(float u, float v)
{
	float h = u;
	float angle = v * 2 * M_PI;
	float r = 0.25;

	return glm::vec3(r*cosf(angle), h, r*sinf(angle));
}

glm::vec3 CMyApp::GetNormHenger(float u, float v)
{
	glm::vec3 uv = GetPos(u, v);
	glm::vec3 du = GetPos(u + 0.01, v) - GetPos(u - 0.01, v);
	glm::vec3 dv = GetPos(u, v + 0.01) - GetPos(u, v - 0.01);

	return (glm::cross(du, dv));
}

glm::vec2 CMyApp::GetTexHenger(float u, float v)
{
	return glm::vec2(u, 1 - v);
}

//*****************************************************HENGER FEJ

glm::vec3 CMyApp::GetPosHengerFej(float u, float v)
{
	float h = u;
	float angle = v * 2 * M_PI;
	float r = 0.5;

	return glm::vec3(r*cosf(angle), h, r*sinf(angle));
}

glm::vec3 CMyApp::GetNormHengerFej(float u, float v)
{
	glm::vec3 uv = GetPos(u, v);
	glm::vec3 du = GetPos(u + 0.01, v) - GetPos(u - 0.01, v);
	glm::vec3 dv = GetPos(u, v + 0.01) - GetPos(u, v - 0.01);

	return (glm::cross(du, dv));
}

glm::vec2 CMyApp::GetTexHengerFej(float u, float v)
{
	return glm::vec2(u, 1 - v);
}






//**************************************KISKOR


glm::vec3 CMyApp::GetPosKiskor(float u, float v)
{
	u *= 2 * 3.1415f;
	v *= 3.1415f / 2;
	float cu = cosf(u), su = sinf(u), cv = cosf(v), sv = sinf(v);
	float r = 0.25;

	return glm::vec3(r*cu*sv, 0, r*su*sv);
}

glm::vec3 CMyApp::GetNormKiskor(float u, float v)
{
	// K�plettel
	/*u *= 2*3.1415f;
	v *= 3.1415f/2;
	float cu = cosf(u), su = sinf(u), cv = cosf(v), sv = sinf(v);
	return glm::vec3(cu*sv, 0, su*sv);*/

	// Numerikusan (nem kell ismerni a k�pletet, el�g a poz�ci��t)
	glm::vec3 uv = GetPos(u, v);
	glm::vec3 du = GetPos(u + 0.01, v) - GetPos(u - 0.01, v);
	glm::vec3 dv = GetPos(u, v + 0.01) - GetPos(u, v - 0.01);

	return (glm::cross(du, dv));
}

glm::vec2 CMyApp::GetTexKiskor(float u, float v)
{
	return glm::vec2(u, 1 - v);
}


bool CMyApp::Init()
{
	// t�rl�si sz�n legyen k�kes
	glClearColor(0.125f, 0.25f, 0.5f, 1.0f);

	//glEnable(GL_CULL_FACE); // kapcsoljuk be a hatrafele nezo lapok eldobasat
	glEnable(GL_DEPTH_TEST); // m�lys�gi teszt bekapcsol�sa (takar�s)
	glCullFace(GL_BACK); // GL_BACK: a kamer�t�l "elfel�" n�z� lapok, GL_FRONT: a kamera fel� n�z� lapok
	glPolygonMode(GL_BACK,GL_LINE);

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

	





	//FELG�MB*****************************************************

	Vertex vertFelgomb[(N + 1)*(M + 1)];
	for (int i = 0; i <= N; ++i)
		for (int j = 0; j <= M; ++j)
		{
			float u = i / (float)N;
			float v = j / (float)M;

			vertFelgomb[i + j * (N + 1)].p = GetPosFelgomb(u, v);
			vertFelgomb[i + j * (N + 1)].n = GetNormFelgomb(u, v);
			vertFelgomb[i + j * (N + 1)].t = GetTexFelgomb(u, v);
		}

	// indexpuffer adatai: NxM n�gysz�g = 2xNxM h�romsz�g = h�romsz�glista eset�n 3x2xNxM index
	GLushort indicesFelgomb[3 * 2 * (N)*(M)];
	for (int i = 0; i<N; ++i)
		for (int j = 0; j<M; ++j)
		{

			indicesFelgomb[6 * i + j * 3 * 2 * (N)+0] = (i)+(j)*	(N + 1);
			indicesFelgomb[6 * i + j * 3 * 2 * (N)+1] = (i + 1) + (j)*	(N + 1);
			indicesFelgomb[6 * i + j * 3 * 2 * (N)+2] = (i)+(j + 1)*(N + 1);
			indicesFelgomb[6 * i + j * 3 * 2 * (N)+3] = (i + 1) + (j)*	(N + 1);
			indicesFelgomb[6 * i + j * 3 * 2 * (N)+4] = (i + 1) + (j + 1)*(N + 1);
			indicesFelgomb[6 * i + j * 3 * 2 * (N)+5] = (i)+(j + 1)*(N + 1);
		}


	// 1 db VAO foglalasa
	glGenVertexArrays(1, &m_vaoFelgombID);
	// a frissen gener�lt VAO beallitasa akt�vnak
	glBindVertexArray(m_vaoFelgombID);

	// hozzunk l�tre egy �j VBO er�forr�s nevet
	glGenBuffers(1, &m_vboFelgombID);
	glBindBuffer(GL_ARRAY_BUFFER, m_vboFelgombID); // tegy�k "akt�vv�" a l�trehozott VBO-t
											// t�lts�k fel adatokkal az akt�v VBO-t
	glBufferData(GL_ARRAY_BUFFER,	// az akt�v VBO-ba t�lts�nk adatokat
		sizeof(vertFelgomb),		// ennyi b�jt nagys�gban
		vertFelgomb,	// err�l a rendszermem�riabeli c�mr�l olvasva
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
	glGenBuffers(1, &m_ibFelgombID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibFelgombID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesFelgomb), indicesFelgomb, GL_STATIC_DRAW);

	glBindVertexArray(0); // felt�lt�tt�k a VAO-t, kapcsoljuk le
	glBindBuffer(GL_ARRAY_BUFFER, 0); // felt�lt�tt�k a VBO-t is, ezt is vegy�k le
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // felt�lt�tt�k a VBO-t is, ezt is vegy�k le



	//**********************************************HENGER*****************************************


	Vertex vertHenger[(N + 1)*(M + 1)];
	for (int i = 0; i <= N; ++i)
		for (int j = 0; j <= M; ++j)
		{
			float u = i / (float)N;
			float v = j / (float)M;

			vertHenger[i + j * (N + 1)].p = GetPosHenger(u, v);
			vertHenger[i + j * (N + 1)].n = GetNormHenger(u, v);
			vertHenger[i + j * (N + 1)].t = GetTexHenger(u, v);
		}

	// indexpuffer adatai: NxM n�gysz�g = 2xNxM h�romsz�g = h�romsz�glista eset�n 3x2xNxM index
	GLushort indicesHenger[3 * 2 * (N)*(M)];
	for (int i = 0; i<N; ++i)
		for (int j = 0; j<M; ++j)
		{

			indicesHenger[6 * i + j * 3 * 2 * (N)+0] = (i)+(j)*	(N + 1);
			indicesHenger[6 * i + j * 3 * 2 * (N)+1] = (i + 1) + (j)*	(N + 1);
			indicesHenger[6 * i + j * 3 * 2 * (N)+2] = (i)+(j + 1)*(N + 1);
			indicesHenger[6 * i + j * 3 * 2 * (N)+3] = (i + 1) + (j)*	(N + 1);
			indicesHenger[6 * i + j * 3 * 2 * (N)+4] = (i + 1) + (j + 1)*(N + 1);
			indicesHenger[6 * i + j * 3 * 2 * (N)+5] = (i)+(j + 1)*(N + 1);
		}


	// 1 db VAO foglalasa
	glGenVertexArrays(1, &m_vaoHengerID);
	// a frissen gener�lt VAO beallitasa akt�vnak
	glBindVertexArray(m_vaoHengerID);

	// hozzunk l�tre egy �j VBO er�forr�s nevet
	glGenBuffers(1, &m_vboHengerID);
	glBindBuffer(GL_ARRAY_BUFFER, m_vboHengerID); // tegy�k "akt�vv�" a l�trehozott VBO-t
												   // t�lts�k fel adatokkal az akt�v VBO-t
	glBufferData(GL_ARRAY_BUFFER,	// az akt�v VBO-ba t�lts�nk adatokat
		sizeof(vertHenger),		// ennyi b�jt nagys�gban
		vertHenger,	// err�l a rendszermem�riabeli c�mr�l olvasva
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
	glGenBuffers(1, &m_ibHengerID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibHengerID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesHenger), indicesHenger, GL_STATIC_DRAW);

	glBindVertexArray(0); // felt�lt�tt�k a VAO-t, kapcsoljuk le
	glBindBuffer(GL_ARRAY_BUFFER, 0); // felt�lt�tt�k a VBO-t is, ezt is vegy�k le
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // felt�lt�tt�k a VBO-t is, ezt is vegy�k le



	//***************************************HENGER-FEJ***************************************


	//**********************************************HENGER*****************************************


	Vertex vertHengerFej[(N + 1)*(M + 1)];
	for (int i = 0; i <= N; ++i)
		for (int j = 0; j <= M; ++j)
		{
			float u = i / (float)N;
			float v = j / (float)M;

			vertHengerFej[i + j * (N + 1)].p = GetPosHengerFej(u, v);
			vertHengerFej[i + j * (N + 1)].n = GetNormHengerFej(u, v);
			vertHengerFej[i + j * (N + 1)].t = GetTexHengerFej(u, v);
		}

	// indexpuffer adatai: NxM n�gysz�g = 2xNxM h�romsz�g = h�romsz�glista eset�n 3x2xNxM index
	GLushort indicesHengerFej[3 * 2 * (N)*(M)];
	for (int i = 0; i<N; ++i)
		for (int j = 0; j<M; ++j)
		{

			indicesHengerFej[6 * i + j * 3 * 2 * (N)+0] = (i)+(j)*	(N + 1);
			indicesHengerFej[6 * i + j * 3 * 2 * (N)+1] = (i + 1) + (j)*	(N + 1);
			indicesHengerFej[6 * i + j * 3 * 2 * (N)+2] = (i)+(j + 1)*(N + 1);
			indicesHengerFej[6 * i + j * 3 * 2 * (N)+3] = (i + 1) + (j)*	(N + 1);
			indicesHengerFej[6 * i + j * 3 * 2 * (N)+4] = (i + 1) + (j + 1)*(N + 1);
			indicesHengerFej[6 * i + j * 3 * 2 * (N)+5] = (i)+(j + 1)*(N + 1);
		}


	// 1 db VAO foglalasa
	glGenVertexArrays(1, &m_vaoHengerFejID);
	// a frissen gener�lt VAO beallitasa akt�vnak
	glBindVertexArray(m_vaoHengerFejID);

	// hozzunk l�tre egy �j VBO er�forr�s nevet
	glGenBuffers(1, &m_vboHengerFejID);
	glBindBuffer(GL_ARRAY_BUFFER, m_vboHengerFejID); // tegy�k "akt�vv�" a l�trehozott VBO-t
												  // t�lts�k fel adatokkal az akt�v VBO-t
	glBufferData(GL_ARRAY_BUFFER,	// az akt�v VBO-ba t�lts�nk adatokat
		sizeof(vertHengerFej),		// ennyi b�jt nagys�gban
		vertHengerFej,	// err�l a rendszermem�riabeli c�mr�l olvasva
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
	glGenBuffers(1, &m_ibHengerFejID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibHengerFejID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesHengerFej), indicesHengerFej, GL_STATIC_DRAW);

	glBindVertexArray(0); // felt�lt�tt�k a VAO-t, kapcsoljuk le
	glBindBuffer(GL_ARRAY_BUFFER, 0); // felt�lt�tt�k a VBO-t is, ezt is vegy�k le
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // felt�lt�tt�k a VBO-t is, ezt is vegy�k le










	//****************************************KISKOR*********************************

	Vertex vertKiskor[(N + 1)*(M + 1)];
	for (int i = 0; i <= N; ++i)
		for (int j = 0; j <= M; ++j)
		{
			float u = i / (float)N;
			float v = j / (float)M;

			vertKiskor[i + j * (N + 1)].p = GetPosKiskor(u, v);
			vertKiskor[i + j * (N + 1)].n = GetNormKiskor(u, v);
			vertKiskor[i + j * (N + 1)].t = GetTexKiskor(u, v);
		}

	// indexpuffer adatai: NxM n�gysz�g = 2xNxM h�romsz�g = h�romsz�glista eset�n 3x2xNxM index
	GLushort indicesKiskor[3 * 2 * (N)*(M)];
	for (int i = 0; i<N; ++i)
		for (int j = 0; j<M; ++j)
		{

			indicesKiskor[6 * i + j * 3 * 2 * (N)+0] = (i)+(j)*	(N + 1);
			indicesKiskor[6 * i + j * 3 * 2 * (N)+1] = (i + 1) + (j)*	(N + 1);
			indicesKiskor[6 * i + j * 3 * 2 * (N)+2] = (i)+(j + 1)*(N + 1);
			indicesKiskor[6 * i + j * 3 * 2 * (N)+3] = (i + 1) + (j)*	(N + 1);
			indicesKiskor[6 * i + j * 3 * 2 * (N)+4] = (i + 1) + (j + 1)*(N + 1);
			indicesKiskor[6 * i + j * 3 * 2 * (N)+5] = (i)+(j + 1)*(N + 1);
		}


	// 1 db VAO foglalasa
	glGenVertexArrays(1, &m_vaoKiskorID);
	// a frissen gener�lt VAO beallitasa akt�vnak
	glBindVertexArray(m_vaoKiskorID);

	// hozzunk l�tre egy �j VBO er�forr�s nevet
	glGenBuffers(1, &m_vboKiskorID);
	glBindBuffer(GL_ARRAY_BUFFER, m_vboKiskorID); // tegy�k "akt�vv�" a l�trehozott VBO-t
												  // t�lts�k fel adatokkal az akt�v VBO-t
	glBufferData(GL_ARRAY_BUFFER,	// az akt�v VBO-ba t�lts�nk adatokat
		sizeof(vertKiskor),		// ennyi b�jt nagys�gban
		vertKiskor,	// err�l a rendszermem�riabeli c�mr�l olvasva
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
	glGenBuffers(1, &m_ibKiskorID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibKiskorID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesKiskor), indicesKiskor, GL_STATIC_DRAW);

	glBindVertexArray(0); // felt�lt�tt�k a VAO-t, kapcsoljuk le
	glBindBuffer(GL_ARRAY_BUFFER, 0); // felt�lt�tt�k a VBO-t is, ezt is vegy�k le
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // felt�lt�tt�k a VBO-t is, ezt is vegy�k le




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

	//m_loadedTextureID : earth.png


	m_loadedTextureID = TextureFromFile("earth.png");
	m_shellTextureID = TextureFromFile("shell.jpg");
	m_scalesTextureID = TextureFromFile("scales.jpg");

	m_generatedTextureID = GenTexture();
	m_samplerID = genSampler();

	return true;
}

void CMyApp::Clean()
{
	glDeleteTextures(1, &m_loadedTextureID);
	glDeleteTextures(1, &m_shellTextureID);
	glDeleteTextures(1, &m_scalesTextureID);
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
	m_matView = glm::lookAt(glm::vec3( 5*cosf(t), 5 * sinf(t),  5*sinf(t)),		// honnan n�zz�k a sz�nteret
	//m_matView = glm::lookAt(glm::vec3(1, -2, 6),		// honnan n�zz�k a sz�nteret
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


	//----------------------------------------------------f�ldlap----------------------
	glBindVertexArray(m_vaoID);

	glBindTexture(GL_TEXTURE_2D, m_loadedTextureID);

	m_matWorld = glm::translate<float>(glm::vec3(0, 3.1415f/1.55, 0));

	glm::mat4 mvp = m_matProj * m_matView * m_matWorld;

	// majd k�ldj�k �t a megfelel� m�trixot!
	glUniformMatrix4fv( m_loc_mvp,// erre a helyre t�lts�nk �t adatot
						1,			// egy darab m�trixot
						GL_FALSE,	// NEM transzpon�lva
						&(mvp[0][0]) ); // innen olvasva a 16 x sizeof(float)-nyi adatot

	glm::mat4 WorldIT = glm::inverse(glm::transpose(m_matWorld));

	glUniformMatrix4fv(m_loc_wit, 1, GL_FALSE, &(WorldIT[0][0]));
	glUniformMatrix4fv(m_loc_w, 1, GL_FALSE, &(m_matWorld[0][0]));

	glActiveTexture(GL_TEXTURE0 + 0);

	// 2 m�sodpercenk�nti v�lt�s a gener�lt �s a bet�lt�tt text�ra k�z�tt
	// Feladat: ehelyett shaderben fokozatos v�lt�s a k�t text�ra k�z�tt
	//if (SDL_GetTicks() / 1000 % 2 == 0)
		
	//else
	//	glBindTexture(GL_TEXTURE_2D, m_generatedTextureID);

	glBindSampler(0, m_samplerID);

	// aktiv�ljuk a gener�lt text�r�nkat 
	// text�ra mintav�telez� �s shader-beli sampler2D �sszerendel�se
	glUniform1i(m_loc_tex,	// ezen azonos�t�j� sampler 2D
		0 );	// olvassa az ezen index� mintav�telez�t

	// kapcsoljuk be a VAO-t (a VBO j�n vele egy�tt)

	

	// kirajzol�s
		glDrawElements(	GL_TRIANGLES,		// primit�v t�pus
					3*2*(N)*(M),		// hany csucspontot hasznalunk a kirajzolashoz
					GL_UNSIGNED_SHORT,	// indexek tipusa
					0);					// indexek cime

	// VAO kikapcsolasa
	glBindVertexArray(0);


	//----------------------------------------------FELGOMB ALJA---------------------------------

	glBindVertexArray(m_vaoID);

	glBindTexture(GL_TEXTURE_2D, m_shellTextureID);
	m_matWorld = glm::rotate<float>(M_PI, glm::vec3(1, 0, 1));

	mvp = m_matProj * m_matView * m_matWorld;

	// majd k�ldj�k �t a megfelel� m�trixot!
	glUniformMatrix4fv(m_loc_mvp,// erre a helyre t�lts�nk �t adatot
		1,			// egy darab m�trixot
		GL_FALSE,	// NEM transzpon�lva
		&(mvp[0][0])); // innen olvasva a 16 x sizeof(float)-nyi adatot

	WorldIT = glm::inverse(glm::transpose(m_matWorld));

	glUniformMatrix4fv(m_loc_wit, 1, GL_FALSE, &(WorldIT[0][0]));
	glUniformMatrix4fv(m_loc_w, 1, GL_FALSE, &(m_matWorld[0][0]));

	glActiveTexture(GL_TEXTURE0 + 0 );


	glBindSampler(0 , m_samplerID);

	// aktiv�ljuk a gener�lt text�r�nkat 
	// text�ra mintav�telez� �s shader-beli sampler2D �sszerendel�se
	glUniform1i(m_loc_tex,	// ezen azonos�t�j� sampler 2D
		0 );	// olvassa az ezen index� mintav�telez�t

						
	// kirajzol�s
	glDrawElements(GL_TRIANGLES,		// primit�v t�pus
		3 * 2 * (N)*(M),		// hany csucspontot hasznalunk a kirajzolashoz
		GL_UNSIGNED_SHORT,	// indexek tipusa
		0);					// indexek cime

							// VAO kikapcsolasa
	glBindVertexArray(0);
	


	//FELG�MB -----------------------------------------------------------------------------
	glBindTexture(GL_TEXTURE_2D, m_shellTextureID);

	glBindVertexArray(m_vaoFelgombID);


	m_matWorld = glm::translate<float>(glm::vec3(0, 0, 0));

	mvp = m_matProj * m_matView * m_matWorld;

	
	glUniformMatrix4fv(m_loc_mvp,// erre a helyre t�lts�nk �t adatot
		1,			// egy darab m�trixot
		GL_FALSE,	// NEM transzpon�lva
		&(mvp[0][0])); // innen olvasva a 16 x sizeof(float)-nyi adatot

	WorldIT = glm::inverse(glm::transpose(m_matWorld));

	glUniformMatrix4fv(m_loc_wit, 1, GL_FALSE, &(WorldIT[0][0]));
	glUniformMatrix4fv(m_loc_w, 1, GL_FALSE, &(m_matWorld[0][0]));

	glActiveTexture(GL_TEXTURE0 + 0 /*hw samplerindex*/);


	glBindSampler(0 , m_samplerID);


	glUniform1i(m_loc_tex,	// ezen azonos�t�j� sampler 2D
		0 /*hw samplerindex*/);	// olvassa az ezen index� mintav�telez�t


	// kirajzol�s
	glDrawElements(GL_TRIANGLES,		// primit�v t�pus
		3 * 2 * (N)*(M),		// hany csucspontot hasznalunk a kirajzolashoz
		GL_UNSIGNED_SHORT,	// indexek tipusa
		0);					// indexek cime

							// VAO kikapcsolasa
	glBindVertexArray(0);




	// ----------------------------------------------------------HENGER-------------------
	glBindTexture(GL_TEXTURE_2D, m_scalesTextureID);

	glBindVertexArray(m_vaoHengerID);


	m_matWorld = glm::scale<float>(glm::vec3(1, 1, 1))*glm::translate<float>(glm::vec3(1.5, -1, 0.8));

	mvp = m_matProj * m_matView * m_matWorld;


	glUniformMatrix4fv(m_loc_mvp,// erre a helyre t�lts�nk �t adatot
		1,			// egy darab m�trixot
		GL_FALSE,	// NEM transzpon�lva
		&(mvp[0][0])); // innen olvasva a 16 x sizeof(float)-nyi adatot

	WorldIT = glm::inverse(glm::transpose(m_matWorld));

	glUniformMatrix4fv(m_loc_wit, 1, GL_FALSE, &(WorldIT[0][0]));
	glUniformMatrix4fv(m_loc_w, 1, GL_FALSE, &(m_matWorld[0][0]));

	glActiveTexture(GL_TEXTURE0 + 0 /*hw samplerindex*/);


	glBindSampler(0, m_samplerID);


	glUniform1i(m_loc_tex,	// ezen azonos�t�j� sampler 2D
		0 /*hw samplerindex*/);	// olvassa az ezen index� mintav�telez�t


								// kirajzol�s
	glDrawElements(GL_TRIANGLES,		// primit�v t�pus
		3 * 2 * (N)*(M),		// hany csucspontot hasznalunk a kirajzolashoz
		GL_UNSIGNED_SHORT,	// indexek tipusa
		0);					// indexek cime


	//-------------- HENGER 2- M�SIK L�BA (2.)


	m_matWorld = glm::scale<float>(glm::vec3(1, 1, 1))*glm::translate<float>(glm::vec3(-1.5, -1, 0.8));

	mvp = m_matProj * m_matView * m_matWorld;


	glUniformMatrix4fv(m_loc_mvp,// erre a helyre t�lts�nk �t adatot
		1,			// egy darab m�trixot
		GL_FALSE,	// NEM transzpon�lva
		&(mvp[0][0])); // innen olvasva a 16 x sizeof(float)-nyi adatot

	WorldIT = glm::inverse(glm::transpose(m_matWorld));

	glUniformMatrix4fv(m_loc_wit, 1, GL_FALSE, &(WorldIT[0][0]));
	glUniformMatrix4fv(m_loc_w, 1, GL_FALSE, &(m_matWorld[0][0]));

	glActiveTexture(GL_TEXTURE0 + 0 /*hw samplerindex*/);


	glBindSampler(0, m_samplerID);


	glUniform1i(m_loc_tex,	// ezen azonos�t�j� sampler 2D
		0 /*hw samplerindex*/);	// olvassa az ezen index� mintav�telez�t


								// kirajzol�s
	glDrawElements(GL_TRIANGLES,		// primit�v t�pus
		3 * 2 * (N)*(M),		// hany csucspontot hasznalunk a kirajzolashoz
		GL_UNSIGNED_SHORT,	// indexek tipusa
		0);					// indexek cime






							//-------------- HENGER 2- HARMADIK L�B


	m_matWorld = glm::scale<float>(glm::vec3(1, 1, 1))*glm::translate<float>(glm::vec3(-1.5, -1, -0.8));

	mvp = m_matProj * m_matView * m_matWorld;


	glUniformMatrix4fv(m_loc_mvp,// erre a helyre t�lts�nk �t adatot
		1,			// egy darab m�trixot
		GL_FALSE,	// NEM transzpon�lva
		&(mvp[0][0])); // innen olvasva a 16 x sizeof(float)-nyi adatot

	WorldIT = glm::inverse(glm::transpose(m_matWorld));

	glUniformMatrix4fv(m_loc_wit, 1, GL_FALSE, &(WorldIT[0][0]));
	glUniformMatrix4fv(m_loc_w, 1, GL_FALSE, &(m_matWorld[0][0]));

	glActiveTexture(GL_TEXTURE0 + 0 /*hw samplerindex*/);


	glBindSampler(0, m_samplerID);


	glUniform1i(m_loc_tex,	// ezen azonos�t�j� sampler 2D
		0 /*hw samplerindex*/);	// olvassa az ezen index� mintav�telez�t


								// kirajzol�s
	glDrawElements(GL_TRIANGLES,		// primit�v t�pus
		3 * 2 * (N)*(M),		// hany csucspontot hasznalunk a kirajzolashoz
		GL_UNSIGNED_SHORT,	// indexek tipusa
		0);					// indexek cime








							//-------------- HENGER 2- NEGYEDIK L�B


	m_matWorld = glm::scale<float>(glm::vec3(1, 1, 1))*glm::translate<float>(glm::vec3(1.5, -1,-0.8));

	mvp = m_matProj * m_matView * m_matWorld;


	glUniformMatrix4fv(m_loc_mvp,// erre a helyre t�lts�nk �t adatot
		1,			// egy darab m�trixot
		GL_FALSE,	// NEM transzpon�lva
		&(mvp[0][0])); // innen olvasva a 16 x sizeof(float)-nyi adatot

	WorldIT = glm::inverse(glm::transpose(m_matWorld));

	glUniformMatrix4fv(m_loc_wit, 1, GL_FALSE, &(WorldIT[0][0]));
	glUniformMatrix4fv(m_loc_w, 1, GL_FALSE, &(m_matWorld[0][0]));

	glActiveTexture(GL_TEXTURE0 + 0 /*hw samplerindex*/);


	glBindSampler(0, m_samplerID);


	glUniform1i(m_loc_tex,	// ezen azonos�t�j� sampler 2D
		0 /*hw samplerindex*/);	// olvassa az ezen index� mintav�telez�t


								// kirajzol�s
	glDrawElements(GL_TRIANGLES,		// primit�v t�pus
		3 * 2 * (N)*(M),		// hany csucspontot hasznalunk a kirajzolashoz
		GL_UNSIGNED_SHORT,	// indexek tipusa
		0);					// indexek cime

















							// ----------------------------------------------------------HENGER-ALJA-------------------
	glBindTexture(GL_TEXTURE_2D, m_scalesTextureID);

	glBindVertexArray(m_vaoKiskorID);

	//m_matWorld = glm::scale<float>(glm::vec3(1, 1, 1))*glm::translate<float>(glm::vec3(1.5, -1, 0.8));

	m_matWorld = glm::rotate<float>(M_PI, glm::vec3(1, 0, 1));
	m_matWorld *= glm::scale<float>(glm::vec3(1, 1, 1))*glm::translate<float>(glm::vec3(0.8, 1, 1.5));

	mvp = m_matProj * m_matView * m_matWorld;


	glUniformMatrix4fv(m_loc_mvp,// erre a helyre t�lts�nk �t adatot
		1,			// egy darab m�trixot
		GL_FALSE,	// NEM transzpon�lva
		&(mvp[0][0])); // innen olvasva a 16 x sizeof(float)-nyi adatot

	WorldIT = glm::inverse(glm::transpose(m_matWorld));

	glUniformMatrix4fv(m_loc_wit, 1, GL_FALSE, &(WorldIT[0][0]));
	glUniformMatrix4fv(m_loc_w, 1, GL_FALSE, &(m_matWorld[0][0]));

	glActiveTexture(GL_TEXTURE0 + 0 /*hw samplerindex*/);


	glBindSampler(0, m_samplerID);


	glUniform1i(m_loc_tex,	// ezen azonos�t�j� sampler 2D
		0 /*hw samplerindex*/);	// olvassa az ezen index� mintav�telez�t


								// kirajzol�s
	glDrawElements(GL_TRIANGLES,		// primit�v t�pus
		3 * 2 * (N)*(M),		// hany csucspontot hasznalunk a kirajzolashoz
		GL_UNSIGNED_SHORT,	// indexek tipusa
		0);					// indexek cime


							//-------------- HENGER 2- M�SIK L�BA (2.)

	m_matWorld = glm::rotate<float>(M_PI, glm::vec3(1, 0, 1));
	m_matWorld *= glm::scale<float>(glm::vec3(1, 1, 1))*glm::translate<float>(glm::vec3(-0.8, 1, 1.5));

	mvp = m_matProj * m_matView * m_matWorld;


	glUniformMatrix4fv(m_loc_mvp,// erre a helyre t�lts�nk �t adatot
		1,			// egy darab m�trixot
		GL_FALSE,	// NEM transzpon�lva
		&(mvp[0][0])); // innen olvasva a 16 x sizeof(float)-nyi adatot

	WorldIT = glm::inverse(glm::transpose(m_matWorld));

	glUniformMatrix4fv(m_loc_wit, 1, GL_FALSE, &(WorldIT[0][0]));
	glUniformMatrix4fv(m_loc_w, 1, GL_FALSE, &(m_matWorld[0][0]));

	glActiveTexture(GL_TEXTURE0 + 0 /*hw samplerindex*/);


	glBindSampler(0, m_samplerID);


	glUniform1i(m_loc_tex,	// ezen azonos�t�j� sampler 2D
		0 /*hw samplerindex*/);	// olvassa az ezen index� mintav�telez�t


								// kirajzol�s
	glDrawElements(GL_TRIANGLES,		// primit�v t�pus
		3 * 2 * (N)*(M),		// hany csucspontot hasznalunk a kirajzolashoz
		GL_UNSIGNED_SHORT,	// indexek tipusa
		0);					// indexek cime






							//-------------- HENGER 2- HARMADIK L�B
	//m_matWorld = glm::scale<float>(glm::vec3(1, 1, 1))*glm::translate<float>(glm::vec3(-1.5, -1, -0.8));

	m_matWorld = glm::rotate<float>(M_PI, glm::vec3(1, 0, 1));
	m_matWorld *= glm::scale<float>(glm::vec3(1, 1, 1))*glm::translate<float>(glm::vec3(-0.8, 1, -1.5));

	mvp = m_matProj * m_matView * m_matWorld;


	glUniformMatrix4fv(m_loc_mvp,// erre a helyre t�lts�nk �t adatot
		1,			// egy darab m�trixot
		GL_FALSE,	// NEM transzpon�lva
		&(mvp[0][0])); // innen olvasva a 16 x sizeof(float)-nyi adatot

	WorldIT = glm::inverse(glm::transpose(m_matWorld));

	glUniformMatrix4fv(m_loc_wit, 1, GL_FALSE, &(WorldIT[0][0]));
	glUniformMatrix4fv(m_loc_w, 1, GL_FALSE, &(m_matWorld[0][0]));

	glActiveTexture(GL_TEXTURE0 + 0 /*hw samplerindex*/);


	glBindSampler(0, m_samplerID);


	glUniform1i(m_loc_tex,	// ezen azonos�t�j� sampler 2D
		0 /*hw samplerindex*/);	// olvassa az ezen index� mintav�telez�t


								// kirajzol�s
	glDrawElements(GL_TRIANGLES,		// primit�v t�pus
		3 * 2 * (N)*(M),		// hany csucspontot hasznalunk a kirajzolashoz
		GL_UNSIGNED_SHORT,	// indexek tipusa
		0);					// indexek cime








							//-------------- HENGER 2- NEGYEDIK L�B

	//m_matWorld = glm::scale<float>(glm::vec3(1, 1, 1))*glm::translate<float>(glm::vec3(1.5, -1,-0.8));

	m_matWorld = glm::rotate<float>(M_PI, glm::vec3(1, 0, 1));
	m_matWorld *= glm::scale<float>(glm::vec3(1, 1, 1))*glm::translate<float>(glm::vec3(0.8, 1, -1.5));

	mvp = m_matProj * m_matView * m_matWorld;


	glUniformMatrix4fv(m_loc_mvp,// erre a helyre t�lts�nk �t adatot
		1,			// egy darab m�trixot
		GL_FALSE,	// NEM transzpon�lva
		&(mvp[0][0])); // innen olvasva a 16 x sizeof(float)-nyi adatot

	WorldIT = glm::inverse(glm::transpose(m_matWorld));

	glUniformMatrix4fv(m_loc_wit, 1, GL_FALSE, &(WorldIT[0][0]));
	glUniformMatrix4fv(m_loc_w, 1, GL_FALSE, &(m_matWorld[0][0]));

	glActiveTexture(GL_TEXTURE0 + 0 /*hw samplerindex*/);


	glBindSampler(0, m_samplerID);


	glUniform1i(m_loc_tex,	// ezen azonos�t�j� sampler 2D
		0 /*hw samplerindex*/);	// olvassa az ezen index� mintav�telez�t


								// kirajzol�s
	glDrawElements(GL_TRIANGLES,		// primit�v t�pus
		3 * 2 * (N)*(M),		// hany csucspontot hasznalunk a kirajzolashoz
		GL_UNSIGNED_SHORT,	// indexek tipusa
		0);					// indexek cime


	glBindVertexArray(0);












	//**************************************HENGER-FEJ*******************************


	glBindVertexArray(m_vaoHengerFejID);

	//m_matWorld = glm::rotate<float>(M_PI, glm::vec3(1, 0, 1));
	//m_matWorld *= glm::scale<float>(glm::vec3(1, 1, 1))*glm::translate<float>(glm::vec3(0.8, 1, -1.5));

	//m_matWorld = glm::mat4(1.0f);
	m_matWorld = glm::scale<float>(glm::vec3(2, 1, 1))*glm::rotate<float>(M_PI/2, glm::vec3(0, 0, 1))
		*glm::translate<float>(glm::vec3(0.49, 0.5, 0));


	mvp = m_matProj * m_matView * m_matWorld;


	glUniformMatrix4fv(m_loc_mvp,// erre a helyre t�lts�nk �t adatot
		1,			// egy darab m�trixot
		GL_FALSE,	// NEM transzpon�lva
		&(mvp[0][0])); // innen olvasva a 16 x sizeof(float)-nyi adatot

	WorldIT = glm::inverse(glm::transpose(m_matWorld));

	glUniformMatrix4fv(m_loc_wit, 1, GL_FALSE, &(WorldIT[0][0]));
	glUniformMatrix4fv(m_loc_w, 1, GL_FALSE, &(m_matWorld[0][0]));

	glActiveTexture(GL_TEXTURE0 + 0 /*hw samplerindex*/);


	glBindSampler(0, m_samplerID);


	glUniform1i(m_loc_tex,	// ezen azonos�t�j� sampler 2D
		0 /*hw samplerindex*/);	// olvassa az ezen index� mintav�telez�t


								// kirajzol�s
	glDrawElements(GL_TRIANGLES,		// primit�v t�pus
		3 * 2 * (N)*(M),		// hany csucspontot hasznalunk a kirajzolashoz
		GL_UNSIGNED_SHORT,	// indexek tipusa
		0);					// indexek cime


	glBindVertexArray(0);




	//****************************************HENGER-FAROK*********************************


	glBindVertexArray(m_vaoHengerID);


	m_matWorld = glm::scale<float>(glm::vec3(2, 1, 1))*glm::rotate<float>(M_PI / 2, glm::vec3(0, 0, 1))
		*glm::translate<float>(glm::vec3(0.2, -1.5, 0));

	mvp = m_matProj * m_matView * m_matWorld;


	glUniformMatrix4fv(m_loc_mvp,// erre a helyre t�lts�nk �t adatot
		1,			// egy darab m�trixot
		GL_FALSE,	// NEM transzpon�lva
		&(mvp[0][0])); // innen olvasva a 16 x sizeof(float)-nyi adatot

	WorldIT = glm::inverse(glm::transpose(m_matWorld));

	glUniformMatrix4fv(m_loc_wit, 1, GL_FALSE, &(WorldIT[0][0]));
	glUniformMatrix4fv(m_loc_w, 1, GL_FALSE, &(m_matWorld[0][0]));

	glActiveTexture(GL_TEXTURE0 + 0 /*hw samplerindex*/);


	glBindSampler(0, m_samplerID);


	glUniform1i(m_loc_tex,	// ezen azonos�t�j� sampler 2D
		0 /*hw samplerindex*/);	// olvassa az ezen index� mintav�telez�t


								// kirajzol�s
	glDrawElements(GL_TRIANGLES,		// primit�v t�pus
		3 * 2 * (N)*(M),		// hany csucspontot hasznalunk a kirajzolashoz
		GL_UNSIGNED_SHORT,	// indexek tipusa
		0);					// indexek cime

	glBindVertexArray(0);

	//********************************HENGER FAROK Z�R�S, K�R

	glBindVertexArray(m_vaoKiskorID);

	
	/* m_matWorld = 
		glm::translate<float>(glm::vec3(1, -1, 0));*/

	m_matWorld = glm::rotate<float>(M_PI, glm::vec3(1, 0, 1));
	m_matWorld *= glm::scale<float>(glm::vec3(2, 1, 1))/**glm::rotate<float>(M_PI / 2, glm::vec3(0, 0, 1))*/
		*glm::translate<float>(glm::vec3(1, 2, 0));

	mvp = m_matProj * m_matView * m_matWorld;


	glUniformMatrix4fv(m_loc_mvp,// erre a helyre t�lts�nk �t adatot
		1,			// egy darab m�trixot
		GL_FALSE,	// NEM transzpon�lva
		&(mvp[0][0])); // innen olvasva a 16 x sizeof(float)-nyi adatot

	WorldIT = glm::inverse(glm::transpose(m_matWorld));

	glUniformMatrix4fv(m_loc_wit, 1, GL_FALSE, &(WorldIT[0][0]));
	glUniformMatrix4fv(m_loc_w, 1, GL_FALSE, &(m_matWorld[0][0]));

	glActiveTexture(GL_TEXTURE0 + 0 /*hw samplerindex*/);


	glBindSampler(0, m_samplerID);


	glUniform1i(m_loc_tex,	// ezen azonos�t�j� sampler 2D
		0 /*hw samplerindex*/);	// olvassa az ezen index� mintav�telez�t


								// kirajzol�s
	glDrawElements(GL_TRIANGLES,		// primit�v t�pus
		3 * 2 * (N)*(M),		// hany csucspontot hasznalunk a kirajzolashoz
		GL_UNSIGNED_SHORT,	// indexek tipusa
		0);					// indexek cime


	glBindVertexArray(0);

	/*

	GLM transzform�ci�s m�trixokra p�ld�k:
	glm::rotate<float>( sz�g, glm::vec3(tengely_x, tengely_y, tengely_z) ) <- tengely_{xyz} k�r�li elforgat�s
	glm::translate<float>( glm::vec3(eltol_x, eltol_y, eltol_z) ) <- eltol�s
	glm::scale<float>( glm::vec3(s_x, s_y, s_z) ) <- l�pt�kez�s

	*/








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
