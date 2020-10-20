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

	// generáljunk egy textúra erõforrás nevet
	glGenTextures(1, &tmpID);
	// aktiváljuk a most generált nevû textúrát
	glBindTexture(GL_TEXTURE_2D, tmpID);
	// töltsük fel adatokkal
	glTexImage2D(GL_TEXTURE_2D,		// melyik binding point-on van a textúra erõforrás, amihez tárolást rendelünk
		0,					// melyik részletességi szint adatait határozzuk meg
		GL_RGB,				// textúra belsõ tárolási formátuma (GPU-n)
		256, 256,			// szélesség, magasság
		0,					// nulla kell, hogy legyen ( https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glTexImage2D.xhtml )
		GL_RGB,				// forrás (=CPU-n) formátuma
		GL_UNSIGNED_BYTE,	// forrás egy pixelének egy csatornáját hogyan tároljuk
		tex);				// forráshoz pointer

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
	// Képlettel
	/*u *= 2*3.1415f;
	v *= 3.1415f/2;
	float cu = cosf(u), su = sinf(u), cv = cosf(v), sv = sinf(v);
	return glm::vec3(cu*sv, 0, su*sv);*/
	
	// Numerikusan (nem kell ismerni a képletet, elég a pozícióét)
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
	// Képlettel
	/*u *= 2*3.1415f;
	v *= 3.1415f/2;
	float cu = cosf(u), su = sinf(u), cv = cosf(v), sv = sinf(v);
	return glm::vec3(cu*sv, 0, su*sv);*/

	// Numerikusan (nem kell ismerni a képletet, elég a pozícióét)
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
	// Képlettel
	/*u *= 2*3.1415f;
	v *= 3.1415f/2;
	float cu = cosf(u), su = sinf(u), cv = cosf(v), sv = sinf(v);
	return glm::vec3(cu*sv, 0, su*sv);*/

	// Numerikusan (nem kell ismerni a képletet, elég a pozícióét)
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
	// törlési szín legyen kékes
	glClearColor(0.125f, 0.25f, 0.5f, 1.0f);

	//glEnable(GL_CULL_FACE); // kapcsoljuk be a hatrafele nezo lapok eldobasat
	glEnable(GL_DEPTH_TEST); // mélységi teszt bekapcsolása (takarás)
	glCullFace(GL_BACK); // GL_BACK: a kamerától "elfelé" nézõ lapok, GL_FRONT: a kamera felé nézõ lapok
	glPolygonMode(GL_BACK,GL_LINE);

	//
	// geometria letrehozasa
	//

	// NxM darab négyszöggel közelítjük a parametrikus felületünket => (N+1)x(M+1) pontban kell kiértékelni
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

	// indexpuffer adatai: NxM négyszög = 2xNxM háromszög = háromszöglista esetén 3x2xNxM index
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
		(void*)(sizeof(glm::vec3)) );

	// textúrakoordináták bekapcsolása a 2-es azonosítójú attribútom csatornán
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(
		2,
		2,
		GL_FLOAT,
		GL_FALSE,
		sizeof(Vertex),
		(void*)(2 * sizeof(glm::vec3)));

	// index puffer létrehozása
	glGenBuffers(1, &m_ibID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindVertexArray(0); // feltöltüttük a VAO-t, kapcsoljuk le
	glBindBuffer(GL_ARRAY_BUFFER, 0); // feltöltöttük a VBO-t is, ezt is vegyük le
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // feltöltöttük a VBO-t is, ezt is vegyük le

	





	//FELGÖMB*****************************************************

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

	// indexpuffer adatai: NxM négyszög = 2xNxM háromszög = háromszöglista esetén 3x2xNxM index
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
	// a frissen generált VAO beallitasa aktívnak
	glBindVertexArray(m_vaoFelgombID);

	// hozzunk létre egy új VBO erõforrás nevet
	glGenBuffers(1, &m_vboFelgombID);
	glBindBuffer(GL_ARRAY_BUFFER, m_vboFelgombID); // tegyük "aktívvá" a létrehozott VBO-t
											// töltsük fel adatokkal az aktív VBO-t
	glBufferData(GL_ARRAY_BUFFER,	// az aktív VBO-ba töltsünk adatokat
		sizeof(vertFelgomb),		// ennyi bájt nagyságban
		vertFelgomb,	// errõl a rendszermemóriabeli címrõl olvasva
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

	// textúrakoordináták bekapcsolása a 2-es azonosítójú attribútom csatornán
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(
		2,
		2,
		GL_FLOAT,
		GL_FALSE,
		sizeof(Vertex),
		(void*)(2 * sizeof(glm::vec3)));

	// index puffer létrehozása
	glGenBuffers(1, &m_ibFelgombID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibFelgombID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesFelgomb), indicesFelgomb, GL_STATIC_DRAW);

	glBindVertexArray(0); // feltöltüttük a VAO-t, kapcsoljuk le
	glBindBuffer(GL_ARRAY_BUFFER, 0); // feltöltöttük a VBO-t is, ezt is vegyük le
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // feltöltöttük a VBO-t is, ezt is vegyük le



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

	// indexpuffer adatai: NxM négyszög = 2xNxM háromszög = háromszöglista esetén 3x2xNxM index
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
	// a frissen generált VAO beallitasa aktívnak
	glBindVertexArray(m_vaoHengerID);

	// hozzunk létre egy új VBO erõforrás nevet
	glGenBuffers(1, &m_vboHengerID);
	glBindBuffer(GL_ARRAY_BUFFER, m_vboHengerID); // tegyük "aktívvá" a létrehozott VBO-t
												   // töltsük fel adatokkal az aktív VBO-t
	glBufferData(GL_ARRAY_BUFFER,	// az aktív VBO-ba töltsünk adatokat
		sizeof(vertHenger),		// ennyi bájt nagyságban
		vertHenger,	// errõl a rendszermemóriabeli címrõl olvasva
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

	// textúrakoordináták bekapcsolása a 2-es azonosítójú attribútom csatornán
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(
		2,
		2,
		GL_FLOAT,
		GL_FALSE,
		sizeof(Vertex),
		(void*)(2 * sizeof(glm::vec3)));

	// index puffer létrehozása
	glGenBuffers(1, &m_ibHengerID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibHengerID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesHenger), indicesHenger, GL_STATIC_DRAW);

	glBindVertexArray(0); // feltöltüttük a VAO-t, kapcsoljuk le
	glBindBuffer(GL_ARRAY_BUFFER, 0); // feltöltöttük a VBO-t is, ezt is vegyük le
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // feltöltöttük a VBO-t is, ezt is vegyük le



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

	// indexpuffer adatai: NxM négyszög = 2xNxM háromszög = háromszöglista esetén 3x2xNxM index
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
	// a frissen generált VAO beallitasa aktívnak
	glBindVertexArray(m_vaoHengerFejID);

	// hozzunk létre egy új VBO erõforrás nevet
	glGenBuffers(1, &m_vboHengerFejID);
	glBindBuffer(GL_ARRAY_BUFFER, m_vboHengerFejID); // tegyük "aktívvá" a létrehozott VBO-t
												  // töltsük fel adatokkal az aktív VBO-t
	glBufferData(GL_ARRAY_BUFFER,	// az aktív VBO-ba töltsünk adatokat
		sizeof(vertHengerFej),		// ennyi bájt nagyságban
		vertHengerFej,	// errõl a rendszermemóriabeli címrõl olvasva
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

	// textúrakoordináták bekapcsolása a 2-es azonosítójú attribútom csatornán
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(
		2,
		2,
		GL_FLOAT,
		GL_FALSE,
		sizeof(Vertex),
		(void*)(2 * sizeof(glm::vec3)));

	// index puffer létrehozása
	glGenBuffers(1, &m_ibHengerFejID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibHengerFejID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesHengerFej), indicesHengerFej, GL_STATIC_DRAW);

	glBindVertexArray(0); // feltöltüttük a VAO-t, kapcsoljuk le
	glBindBuffer(GL_ARRAY_BUFFER, 0); // feltöltöttük a VBO-t is, ezt is vegyük le
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // feltöltöttük a VBO-t is, ezt is vegyük le










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

	// indexpuffer adatai: NxM négyszög = 2xNxM háromszög = háromszöglista esetén 3x2xNxM index
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
	// a frissen generált VAO beallitasa aktívnak
	glBindVertexArray(m_vaoKiskorID);

	// hozzunk létre egy új VBO erõforrás nevet
	glGenBuffers(1, &m_vboKiskorID);
	glBindBuffer(GL_ARRAY_BUFFER, m_vboKiskorID); // tegyük "aktívvá" a létrehozott VBO-t
												  // töltsük fel adatokkal az aktív VBO-t
	glBufferData(GL_ARRAY_BUFFER,	// az aktív VBO-ba töltsünk adatokat
		sizeof(vertKiskor),		// ennyi bájt nagyságban
		vertKiskor,	// errõl a rendszermemóriabeli címrõl olvasva
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

	// textúrakoordináták bekapcsolása a 2-es azonosítójú attribútom csatornán
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(
		2,
		2,
		GL_FLOAT,
		GL_FALSE,
		sizeof(Vertex),
		(void*)(2 * sizeof(glm::vec3)));

	// index puffer létrehozása
	glGenBuffers(1, &m_ibKiskorID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibKiskorID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesKiskor), indicesKiskor, GL_STATIC_DRAW);

	glBindVertexArray(0); // feltöltüttük a VAO-t, kapcsoljuk le
	glBindBuffer(GL_ARRAY_BUFFER, 0); // feltöltöttük a VBO-t is, ezt is vegyük le
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // feltöltöttük a VBO-t is, ezt is vegyük le




	// shaderek betöltése
	//
	GLuint vs_ID = loadShader(GL_VERTEX_SHADER,		"myVert.vert");
	GLuint fs_ID = loadShader(GL_FRAGMENT_SHADER,	"myFrag.frag");

	// a shadereket tároló program létrehozása
	m_programID = glCreateProgram();

	// adjuk hozzá a programhoz a shadereket
	glAttachShader(m_programID, vs_ID);
	glAttachShader(m_programID, fs_ID);

	// VAO-beli attribútumok hozzárendelése a shader változókhoz
	// FONTOS: linkelés elõtt kell ezt megtenni!
	glBindAttribLocation(	m_programID,	// shader azonosítója, amibõl egy változóhoz szeretnénk hozzárendelést csinálni
							0,				// a VAO-beli azonosító index
							"vs_in_pos");	// a shader-beli változónév
	glBindAttribLocation( m_programID, 1, "vs_in_col");
	glBindAttribLocation( m_programID, 2, "vs_in_tex0");

	// illesszük össze a shadereket (kimenõ-bemenõ változók összerendelése stb.)
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

		std::cout << "[app.Init()] Sáder Huba panasza: " << aSzoveg << std::endl;

		delete aSzoveg;
	}

	// mar nincs ezekre szukseg
	glDeleteShader( vs_ID );
	glDeleteShader( fs_ID );

	//
	// egyéb inicializálás
	//

	// vetítési mátrix létrehozása
	m_matProj = glm::perspective( 45.0f, 640/480.0f, 1.0f, 1000.0f );

	// shader-beli transzformációs mátrixok címének lekérdezése
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
	// nézeti transzformáció beállítása
	float t = SDL_GetTicks()/1000.0f;
	m_matView = glm::lookAt(glm::vec3( 5*cosf(t), 5 * sinf(t),  5*sinf(t)),		// honnan nézzük a színteret
	//m_matView = glm::lookAt(glm::vec3(1, -2, 6),		// honnan nézzük a színteret
							glm::vec3( 0,  0,  0),		// a színtér melyik pontját nézzük
							glm::vec3( 0,  1,  0));		// felfelé mutató irány a világban
}


void CMyApp::Render()
{
	// töröljük a frampuffert (GL_COLOR_BUFFER_BIT) és a mélységi Z puffert (GL_DEPTH_BUFFER_BIT)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// shader bekapcsolasa
	glUseProgram( m_programID );

	// shader parameterek beállítása
	/*

	GLM transzformációs mátrixokra példák:
		glm::rotate<float>( szög, glm::vec3(tengely_x, tengely_y, tengely_z) ) <- tengely_{xyz} körüli elforgatás
		glm::translate<float>( glm::vec3(eltol_x, eltol_y, eltol_z) ) <- eltolás
		glm::scale<float>( glm::vec3(s_x, s_y, s_z) ) <- léptékezés

	*/


	//----------------------------------------------------földlap----------------------
	glBindVertexArray(m_vaoID);

	glBindTexture(GL_TEXTURE_2D, m_loadedTextureID);

	m_matWorld = glm::translate<float>(glm::vec3(0, 3.1415f/1.55, 0));

	glm::mat4 mvp = m_matProj * m_matView * m_matWorld;

	// majd küldjük át a megfelelõ mátrixot!
	glUniformMatrix4fv( m_loc_mvp,// erre a helyre töltsünk át adatot
						1,			// egy darab mátrixot
						GL_FALSE,	// NEM transzponálva
						&(mvp[0][0]) ); // innen olvasva a 16 x sizeof(float)-nyi adatot

	glm::mat4 WorldIT = glm::inverse(glm::transpose(m_matWorld));

	glUniformMatrix4fv(m_loc_wit, 1, GL_FALSE, &(WorldIT[0][0]));
	glUniformMatrix4fv(m_loc_w, 1, GL_FALSE, &(m_matWorld[0][0]));

	glActiveTexture(GL_TEXTURE0 + 0);

	// 2 másodpercenkénti váltás a generált és a betöltött textúra között
	// Feladat: ehelyett shaderben fokozatos váltás a két textúra között
	//if (SDL_GetTicks() / 1000 % 2 == 0)
		
	//else
	//	glBindTexture(GL_TEXTURE_2D, m_generatedTextureID);

	glBindSampler(0, m_samplerID);

	// aktiváljuk a generált textúránkat 
	// textúra mintavételezõ és shader-beli sampler2D összerendelése
	glUniform1i(m_loc_tex,	// ezen azonosítójú sampler 2D
		0 );	// olvassa az ezen indexû mintavételezõt

	// kapcsoljuk be a VAO-t (a VBO jön vele együtt)

	

	// kirajzolás
		glDrawElements(	GL_TRIANGLES,		// primitív típus
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

	// majd küldjük át a megfelelõ mátrixot!
	glUniformMatrix4fv(m_loc_mvp,// erre a helyre töltsünk át adatot
		1,			// egy darab mátrixot
		GL_FALSE,	// NEM transzponálva
		&(mvp[0][0])); // innen olvasva a 16 x sizeof(float)-nyi adatot

	WorldIT = glm::inverse(glm::transpose(m_matWorld));

	glUniformMatrix4fv(m_loc_wit, 1, GL_FALSE, &(WorldIT[0][0]));
	glUniformMatrix4fv(m_loc_w, 1, GL_FALSE, &(m_matWorld[0][0]));

	glActiveTexture(GL_TEXTURE0 + 0 );


	glBindSampler(0 , m_samplerID);

	// aktiváljuk a generált textúránkat 
	// textúra mintavételezõ és shader-beli sampler2D összerendelése
	glUniform1i(m_loc_tex,	// ezen azonosítójú sampler 2D
		0 );	// olvassa az ezen indexû mintavételezõt

						
	// kirajzolás
	glDrawElements(GL_TRIANGLES,		// primitív típus
		3 * 2 * (N)*(M),		// hany csucspontot hasznalunk a kirajzolashoz
		GL_UNSIGNED_SHORT,	// indexek tipusa
		0);					// indexek cime

							// VAO kikapcsolasa
	glBindVertexArray(0);
	


	//FELGÖMB -----------------------------------------------------------------------------
	glBindTexture(GL_TEXTURE_2D, m_shellTextureID);

	glBindVertexArray(m_vaoFelgombID);


	m_matWorld = glm::translate<float>(glm::vec3(0, 0, 0));

	mvp = m_matProj * m_matView * m_matWorld;

	
	glUniformMatrix4fv(m_loc_mvp,// erre a helyre töltsünk át adatot
		1,			// egy darab mátrixot
		GL_FALSE,	// NEM transzponálva
		&(mvp[0][0])); // innen olvasva a 16 x sizeof(float)-nyi adatot

	WorldIT = glm::inverse(glm::transpose(m_matWorld));

	glUniformMatrix4fv(m_loc_wit, 1, GL_FALSE, &(WorldIT[0][0]));
	glUniformMatrix4fv(m_loc_w, 1, GL_FALSE, &(m_matWorld[0][0]));

	glActiveTexture(GL_TEXTURE0 + 0 /*hw samplerindex*/);


	glBindSampler(0 , m_samplerID);


	glUniform1i(m_loc_tex,	// ezen azonosítójú sampler 2D
		0 /*hw samplerindex*/);	// olvassa az ezen indexû mintavételezõt


	// kirajzolás
	glDrawElements(GL_TRIANGLES,		// primitív típus
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


	glUniformMatrix4fv(m_loc_mvp,// erre a helyre töltsünk át adatot
		1,			// egy darab mátrixot
		GL_FALSE,	// NEM transzponálva
		&(mvp[0][0])); // innen olvasva a 16 x sizeof(float)-nyi adatot

	WorldIT = glm::inverse(glm::transpose(m_matWorld));

	glUniformMatrix4fv(m_loc_wit, 1, GL_FALSE, &(WorldIT[0][0]));
	glUniformMatrix4fv(m_loc_w, 1, GL_FALSE, &(m_matWorld[0][0]));

	glActiveTexture(GL_TEXTURE0 + 0 /*hw samplerindex*/);


	glBindSampler(0, m_samplerID);


	glUniform1i(m_loc_tex,	// ezen azonosítójú sampler 2D
		0 /*hw samplerindex*/);	// olvassa az ezen indexû mintavételezõt


								// kirajzolás
	glDrawElements(GL_TRIANGLES,		// primitív típus
		3 * 2 * (N)*(M),		// hany csucspontot hasznalunk a kirajzolashoz
		GL_UNSIGNED_SHORT,	// indexek tipusa
		0);					// indexek cime


	//-------------- HENGER 2- MÁSIK LÁBA (2.)


	m_matWorld = glm::scale<float>(glm::vec3(1, 1, 1))*glm::translate<float>(glm::vec3(-1.5, -1, 0.8));

	mvp = m_matProj * m_matView * m_matWorld;


	glUniformMatrix4fv(m_loc_mvp,// erre a helyre töltsünk át adatot
		1,			// egy darab mátrixot
		GL_FALSE,	// NEM transzponálva
		&(mvp[0][0])); // innen olvasva a 16 x sizeof(float)-nyi adatot

	WorldIT = glm::inverse(glm::transpose(m_matWorld));

	glUniformMatrix4fv(m_loc_wit, 1, GL_FALSE, &(WorldIT[0][0]));
	glUniformMatrix4fv(m_loc_w, 1, GL_FALSE, &(m_matWorld[0][0]));

	glActiveTexture(GL_TEXTURE0 + 0 /*hw samplerindex*/);


	glBindSampler(0, m_samplerID);


	glUniform1i(m_loc_tex,	// ezen azonosítójú sampler 2D
		0 /*hw samplerindex*/);	// olvassa az ezen indexû mintavételezõt


								// kirajzolás
	glDrawElements(GL_TRIANGLES,		// primitív típus
		3 * 2 * (N)*(M),		// hany csucspontot hasznalunk a kirajzolashoz
		GL_UNSIGNED_SHORT,	// indexek tipusa
		0);					// indexek cime






							//-------------- HENGER 2- HARMADIK LÁB


	m_matWorld = glm::scale<float>(glm::vec3(1, 1, 1))*glm::translate<float>(glm::vec3(-1.5, -1, -0.8));

	mvp = m_matProj * m_matView * m_matWorld;


	glUniformMatrix4fv(m_loc_mvp,// erre a helyre töltsünk át adatot
		1,			// egy darab mátrixot
		GL_FALSE,	// NEM transzponálva
		&(mvp[0][0])); // innen olvasva a 16 x sizeof(float)-nyi adatot

	WorldIT = glm::inverse(glm::transpose(m_matWorld));

	glUniformMatrix4fv(m_loc_wit, 1, GL_FALSE, &(WorldIT[0][0]));
	glUniformMatrix4fv(m_loc_w, 1, GL_FALSE, &(m_matWorld[0][0]));

	glActiveTexture(GL_TEXTURE0 + 0 /*hw samplerindex*/);


	glBindSampler(0, m_samplerID);


	glUniform1i(m_loc_tex,	// ezen azonosítójú sampler 2D
		0 /*hw samplerindex*/);	// olvassa az ezen indexû mintavételezõt


								// kirajzolás
	glDrawElements(GL_TRIANGLES,		// primitív típus
		3 * 2 * (N)*(M),		// hany csucspontot hasznalunk a kirajzolashoz
		GL_UNSIGNED_SHORT,	// indexek tipusa
		0);					// indexek cime








							//-------------- HENGER 2- NEGYEDIK LÁB


	m_matWorld = glm::scale<float>(glm::vec3(1, 1, 1))*glm::translate<float>(glm::vec3(1.5, -1,-0.8));

	mvp = m_matProj * m_matView * m_matWorld;


	glUniformMatrix4fv(m_loc_mvp,// erre a helyre töltsünk át adatot
		1,			// egy darab mátrixot
		GL_FALSE,	// NEM transzponálva
		&(mvp[0][0])); // innen olvasva a 16 x sizeof(float)-nyi adatot

	WorldIT = glm::inverse(glm::transpose(m_matWorld));

	glUniformMatrix4fv(m_loc_wit, 1, GL_FALSE, &(WorldIT[0][0]));
	glUniformMatrix4fv(m_loc_w, 1, GL_FALSE, &(m_matWorld[0][0]));

	glActiveTexture(GL_TEXTURE0 + 0 /*hw samplerindex*/);


	glBindSampler(0, m_samplerID);


	glUniform1i(m_loc_tex,	// ezen azonosítójú sampler 2D
		0 /*hw samplerindex*/);	// olvassa az ezen indexû mintavételezõt


								// kirajzolás
	glDrawElements(GL_TRIANGLES,		// primitív típus
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


	glUniformMatrix4fv(m_loc_mvp,// erre a helyre töltsünk át adatot
		1,			// egy darab mátrixot
		GL_FALSE,	// NEM transzponálva
		&(mvp[0][0])); // innen olvasva a 16 x sizeof(float)-nyi adatot

	WorldIT = glm::inverse(glm::transpose(m_matWorld));

	glUniformMatrix4fv(m_loc_wit, 1, GL_FALSE, &(WorldIT[0][0]));
	glUniformMatrix4fv(m_loc_w, 1, GL_FALSE, &(m_matWorld[0][0]));

	glActiveTexture(GL_TEXTURE0 + 0 /*hw samplerindex*/);


	glBindSampler(0, m_samplerID);


	glUniform1i(m_loc_tex,	// ezen azonosítójú sampler 2D
		0 /*hw samplerindex*/);	// olvassa az ezen indexû mintavételezõt


								// kirajzolás
	glDrawElements(GL_TRIANGLES,		// primitív típus
		3 * 2 * (N)*(M),		// hany csucspontot hasznalunk a kirajzolashoz
		GL_UNSIGNED_SHORT,	// indexek tipusa
		0);					// indexek cime


							//-------------- HENGER 2- MÁSIK LÁBA (2.)

	m_matWorld = glm::rotate<float>(M_PI, glm::vec3(1, 0, 1));
	m_matWorld *= glm::scale<float>(glm::vec3(1, 1, 1))*glm::translate<float>(glm::vec3(-0.8, 1, 1.5));

	mvp = m_matProj * m_matView * m_matWorld;


	glUniformMatrix4fv(m_loc_mvp,// erre a helyre töltsünk át adatot
		1,			// egy darab mátrixot
		GL_FALSE,	// NEM transzponálva
		&(mvp[0][0])); // innen olvasva a 16 x sizeof(float)-nyi adatot

	WorldIT = glm::inverse(glm::transpose(m_matWorld));

	glUniformMatrix4fv(m_loc_wit, 1, GL_FALSE, &(WorldIT[0][0]));
	glUniformMatrix4fv(m_loc_w, 1, GL_FALSE, &(m_matWorld[0][0]));

	glActiveTexture(GL_TEXTURE0 + 0 /*hw samplerindex*/);


	glBindSampler(0, m_samplerID);


	glUniform1i(m_loc_tex,	// ezen azonosítójú sampler 2D
		0 /*hw samplerindex*/);	// olvassa az ezen indexû mintavételezõt


								// kirajzolás
	glDrawElements(GL_TRIANGLES,		// primitív típus
		3 * 2 * (N)*(M),		// hany csucspontot hasznalunk a kirajzolashoz
		GL_UNSIGNED_SHORT,	// indexek tipusa
		0);					// indexek cime






							//-------------- HENGER 2- HARMADIK LÁB
	//m_matWorld = glm::scale<float>(glm::vec3(1, 1, 1))*glm::translate<float>(glm::vec3(-1.5, -1, -0.8));

	m_matWorld = glm::rotate<float>(M_PI, glm::vec3(1, 0, 1));
	m_matWorld *= glm::scale<float>(glm::vec3(1, 1, 1))*glm::translate<float>(glm::vec3(-0.8, 1, -1.5));

	mvp = m_matProj * m_matView * m_matWorld;


	glUniformMatrix4fv(m_loc_mvp,// erre a helyre töltsünk át adatot
		1,			// egy darab mátrixot
		GL_FALSE,	// NEM transzponálva
		&(mvp[0][0])); // innen olvasva a 16 x sizeof(float)-nyi adatot

	WorldIT = glm::inverse(glm::transpose(m_matWorld));

	glUniformMatrix4fv(m_loc_wit, 1, GL_FALSE, &(WorldIT[0][0]));
	glUniformMatrix4fv(m_loc_w, 1, GL_FALSE, &(m_matWorld[0][0]));

	glActiveTexture(GL_TEXTURE0 + 0 /*hw samplerindex*/);


	glBindSampler(0, m_samplerID);


	glUniform1i(m_loc_tex,	// ezen azonosítójú sampler 2D
		0 /*hw samplerindex*/);	// olvassa az ezen indexû mintavételezõt


								// kirajzolás
	glDrawElements(GL_TRIANGLES,		// primitív típus
		3 * 2 * (N)*(M),		// hany csucspontot hasznalunk a kirajzolashoz
		GL_UNSIGNED_SHORT,	// indexek tipusa
		0);					// indexek cime








							//-------------- HENGER 2- NEGYEDIK LÁB

	//m_matWorld = glm::scale<float>(glm::vec3(1, 1, 1))*glm::translate<float>(glm::vec3(1.5, -1,-0.8));

	m_matWorld = glm::rotate<float>(M_PI, glm::vec3(1, 0, 1));
	m_matWorld *= glm::scale<float>(glm::vec3(1, 1, 1))*glm::translate<float>(glm::vec3(0.8, 1, -1.5));

	mvp = m_matProj * m_matView * m_matWorld;


	glUniformMatrix4fv(m_loc_mvp,// erre a helyre töltsünk át adatot
		1,			// egy darab mátrixot
		GL_FALSE,	// NEM transzponálva
		&(mvp[0][0])); // innen olvasva a 16 x sizeof(float)-nyi adatot

	WorldIT = glm::inverse(glm::transpose(m_matWorld));

	glUniformMatrix4fv(m_loc_wit, 1, GL_FALSE, &(WorldIT[0][0]));
	glUniformMatrix4fv(m_loc_w, 1, GL_FALSE, &(m_matWorld[0][0]));

	glActiveTexture(GL_TEXTURE0 + 0 /*hw samplerindex*/);


	glBindSampler(0, m_samplerID);


	glUniform1i(m_loc_tex,	// ezen azonosítójú sampler 2D
		0 /*hw samplerindex*/);	// olvassa az ezen indexû mintavételezõt


								// kirajzolás
	glDrawElements(GL_TRIANGLES,		// primitív típus
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


	glUniformMatrix4fv(m_loc_mvp,// erre a helyre töltsünk át adatot
		1,			// egy darab mátrixot
		GL_FALSE,	// NEM transzponálva
		&(mvp[0][0])); // innen olvasva a 16 x sizeof(float)-nyi adatot

	WorldIT = glm::inverse(glm::transpose(m_matWorld));

	glUniformMatrix4fv(m_loc_wit, 1, GL_FALSE, &(WorldIT[0][0]));
	glUniformMatrix4fv(m_loc_w, 1, GL_FALSE, &(m_matWorld[0][0]));

	glActiveTexture(GL_TEXTURE0 + 0 /*hw samplerindex*/);


	glBindSampler(0, m_samplerID);


	glUniform1i(m_loc_tex,	// ezen azonosítójú sampler 2D
		0 /*hw samplerindex*/);	// olvassa az ezen indexû mintavételezõt


								// kirajzolás
	glDrawElements(GL_TRIANGLES,		// primitív típus
		3 * 2 * (N)*(M),		// hany csucspontot hasznalunk a kirajzolashoz
		GL_UNSIGNED_SHORT,	// indexek tipusa
		0);					// indexek cime


	glBindVertexArray(0);




	//****************************************HENGER-FAROK*********************************


	glBindVertexArray(m_vaoHengerID);


	m_matWorld = glm::scale<float>(glm::vec3(2, 1, 1))*glm::rotate<float>(M_PI / 2, glm::vec3(0, 0, 1))
		*glm::translate<float>(glm::vec3(0.2, -1.5, 0));

	mvp = m_matProj * m_matView * m_matWorld;


	glUniformMatrix4fv(m_loc_mvp,// erre a helyre töltsünk át adatot
		1,			// egy darab mátrixot
		GL_FALSE,	// NEM transzponálva
		&(mvp[0][0])); // innen olvasva a 16 x sizeof(float)-nyi adatot

	WorldIT = glm::inverse(glm::transpose(m_matWorld));

	glUniformMatrix4fv(m_loc_wit, 1, GL_FALSE, &(WorldIT[0][0]));
	glUniformMatrix4fv(m_loc_w, 1, GL_FALSE, &(m_matWorld[0][0]));

	glActiveTexture(GL_TEXTURE0 + 0 /*hw samplerindex*/);


	glBindSampler(0, m_samplerID);


	glUniform1i(m_loc_tex,	// ezen azonosítójú sampler 2D
		0 /*hw samplerindex*/);	// olvassa az ezen indexû mintavételezõt


								// kirajzolás
	glDrawElements(GL_TRIANGLES,		// primitív típus
		3 * 2 * (N)*(M),		// hany csucspontot hasznalunk a kirajzolashoz
		GL_UNSIGNED_SHORT,	// indexek tipusa
		0);					// indexek cime

	glBindVertexArray(0);

	//********************************HENGER FAROK ZÁRÁS, KÖR

	glBindVertexArray(m_vaoKiskorID);

	
	/* m_matWorld = 
		glm::translate<float>(glm::vec3(1, -1, 0));*/

	m_matWorld = glm::rotate<float>(M_PI, glm::vec3(1, 0, 1));
	m_matWorld *= glm::scale<float>(glm::vec3(2, 1, 1))/**glm::rotate<float>(M_PI / 2, glm::vec3(0, 0, 1))*/
		*glm::translate<float>(glm::vec3(1, 2, 0));

	mvp = m_matProj * m_matView * m_matWorld;


	glUniformMatrix4fv(m_loc_mvp,// erre a helyre töltsünk át adatot
		1,			// egy darab mátrixot
		GL_FALSE,	// NEM transzponálva
		&(mvp[0][0])); // innen olvasva a 16 x sizeof(float)-nyi adatot

	WorldIT = glm::inverse(glm::transpose(m_matWorld));

	glUniformMatrix4fv(m_loc_wit, 1, GL_FALSE, &(WorldIT[0][0]));
	glUniformMatrix4fv(m_loc_w, 1, GL_FALSE, &(m_matWorld[0][0]));

	glActiveTexture(GL_TEXTURE0 + 0 /*hw samplerindex*/);


	glBindSampler(0, m_samplerID);


	glUniform1i(m_loc_tex,	// ezen azonosítójú sampler 2D
		0 /*hw samplerindex*/);	// olvassa az ezen indexû mintavételezõt


								// kirajzolás
	glDrawElements(GL_TRIANGLES,		// primitív típus
		3 * 2 * (N)*(M),		// hany csucspontot hasznalunk a kirajzolashoz
		GL_UNSIGNED_SHORT,	// indexek tipusa
		0);					// indexek cime


	glBindVertexArray(0);

	/*

	GLM transzformációs mátrixokra példák:
	glm::rotate<float>( szög, glm::vec3(tengely_x, tengely_y, tengely_z) ) <- tengely_{xyz} körüli elforgatás
	glm::translate<float>( glm::vec3(eltol_x, eltol_y, eltol_z) ) <- eltolás
	glm::scale<float>( glm::vec3(s_x, s_y, s_z) ) <- léptékezés

	*/








	// textúra kikapcsolása
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

// a két paraméterbe az új ablakméret szélessége (_w) és magassága (_h) található
void CMyApp::Resize(int _w, int _h)
{
	glViewport(0, 0, _w, _h);

	m_matProj = glm::perspective(  45.0f,		// 90 fokos nyilasszog
									_w/(float)_h,	// ablakmereteknek megfelelo nezeti arany
									0.01f,			// kozeli vagosik
									100.0f);		// tavoli vagosik
}
