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
// egy parametrikus felület (u,v) paraméterértékekhez tartozó pontjának
// kiszámítását végzõ függvény
//
glm::vec3	CMyApp::GetPos(float u, float v)
{
	// origó középpontú, egységsugarú gömb parametrikus alakja: http://hu.wikipedia.org/wiki/G%C3%B6mb#Egyenletek 
	// figyeljünk:	matematikában sokszor a Z tengely mutat felfelé, de nálunk az Y, tehát a legtöbb képlethez képest nálunk
	//				az Y és Z koordináták felcserélve szerepelnek
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

	// NxM darab négyszöggel közelítjük a parametrikus felületünket => (N+1)x(M+1) pontban kell kiértékelni
	Vertex vert[(N + 1) * (M + 1)];
	for (int i = 0; i <= N; ++i)
		for (int j = 0; j <= M; ++j)
		{
			float u = i / (float)N;
			float v = j / (float)M;

			vert[i + j * (N + 1)].p = GetPos(u, v);
			vert[i + j * (N + 1)].c = glm::normalize(vert[i + j * (N + 1)].p);
		}

	// indexpuffer adatai: NxM négyszög = 2xNxM háromszög = háromszöglista esetén 3x2xNxM index
	GLushort indices[3 * 2 * (N) * (M)];
	for (int i = 0; i < N; ++i)
		for (int j = 0; j < M; ++j)
		{
			// minden négyszögre csináljunk kettõ háromszöget, amelyek a következõ 
			// (i,j) indexeknél született (u_i, v_i) paraméterértékekhez tartozó
			// pontokat kötik össze:
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
			// - az (i,j)-hez tartózó 1D-s index a VBO-ban: i+j*(N+1)
			// - az (i,j)-hez tartózó 1D-s index az IB-ben: i*6+j*6*(N+1) 
			//		(mert minden négyszöghöz 2db háromszög = 6 index tartozik)
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
	// a frissen generált VAO beallitasa aktívnak
	glBindVertexArray(m_vaoID);

	// hozzunk létre egy új VBO erõforrás nevet
	glGenBuffers(1, &m_vboID);
	glBindBuffer(GL_ARRAY_BUFFER, m_vboID); // tegyük "aktívvá" a létrehozott VBO-t
	// töltsük fel adatokkal az aktív VBO-t
	glBufferData(GL_ARRAY_BUFFER,	// az aktív VBO-ba töltsünk adatokat
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
		(void*)(sizeof(glm::vec3)));

	// index puffer létrehozása
	glGenBuffers(1, &m_ibID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindVertexArray(0); // feltöltüttük a VAO-t, kapcsoljuk le
	glBindBuffer(GL_ARRAY_BUFFER, 0); // feltöltöttük a VBO-t is, ezt is vegyük le
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // feltöltöttük a VBO-t is, ezt is vegyük le
}

void CMyApp::InitGula()
{
	//
	// geometria letrehozasa
	//

	// NxM darab négyszöggel közelítjük a parametrikus felületünket => (N+1)x(M+1) pontban kell kiértékelni
	Vertex vert[gulaN + 2];
	vert[0] = { glm::vec3(0,1,0),glm::vec3(atan(0) / M_PI + 0.5,atan(1) / M_PI + 0.5,atan(0) / M_PI + 0.5) };
	for (int i = 0; i < gulaN; ++i)
	{
		float alpha = 2 * M_PI * i / gulaN;
		vert[i + 1] = { glm::vec3(cos(alpha),0,sin(alpha)),glm::vec3(atan(cos(alpha)) / M_PI + 0.5,atan(0) / M_PI + 0.5,atan(sin(alpha)) / M_PI + 0.5) };
	}
	vert[gulaN + 1] = { glm::vec3(0,0,0),glm::vec3(atan(0) / M_PI + 0.5,atan(0) / M_PI + 0.5,atan(0) / M_PI + 0.5) };

	// indexpuffer adatai: NxM négyszög = 2xNxM háromszög = háromszöglista esetén 3x2xNxM index
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
	// a frissen generált VAO beallitasa aktívnak
	glBindVertexArray(m_gula_vaoID);

	// hozzunk létre egy új VBO erõforrás nevet
	glGenBuffers(1, &m_gula_vboID);
	glBindBuffer(GL_ARRAY_BUFFER, m_gula_vboID); // tegyük "aktívvá" a létrehozott VBO-t
	// töltsük fel adatokkal az aktív VBO-t
	glBufferData(GL_ARRAY_BUFFER,	// az aktív VBO-ba töltsünk adatokat
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
		(void*)(sizeof(glm::vec3)));

	// index puffer létrehozása
	glGenBuffers(1, &m_gula_ibID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_gula_ibID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindVertexArray(0); // feltöltüttük a VAO-t, kapcsoljuk le
	glBindBuffer(GL_ARRAY_BUFFER, 0); // feltöltöttük a VBO-t is, ezt is vegyük le
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // feltöltöttük a VBO-t is, ezt is vegyük le
}

void CMyApp::InitHasab()
{
	//
	// geometria letrehozasa
	//

	// NxM darab négyszöggel közelítjük a parametrikus felületünket => (N+1)x(M+1) pontban kell kiértékelni
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
	// a frissen generált VAO beallitasa aktívnak
	glBindVertexArray(m_hasab_vaoID);

	// hozzunk létre egy új VBO erõforrás nevet
	glGenBuffers(1, &m_hasab_vboID);
	glBindBuffer(GL_ARRAY_BUFFER, m_hasab_vboID); // tegyük "aktívvá" a létrehozott VBO-t
	// töltsük fel adatokkal az aktív VBO-t
	glBufferData(GL_ARRAY_BUFFER,	// az aktív VBO-ba töltsünk adatokat
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
		(void*)(sizeof(glm::vec3)));


	glBindVertexArray(0); // feltöltüttük a VAO-t, kapcsoljuk le
	glBindBuffer(GL_ARRAY_BUFFER, 0); // feltöltöttük a VBO-t is, ezt is vegyük le
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // feltöltöttük a VBO-t is, ezt is vegyük le
}

bool CMyApp::Init()
{
	// törlési szín legyen kékes
	glClearColor(0.125f, 0.25f, 0.5f, 1.0f);

	glEnable(GL_CULL_FACE); // kapcsoljuk be a hatrafele nezo lapok eldobasat
	glEnable(GL_DEPTH_TEST); // mélységi teszt bekapcsolása (takarás)
	glCullFace(GL_BACK); // GL_BACK: a kamerától "elfelé" nézõ lapok, GL_FRONT: a kamera felé nézõ lapok

	InitTorusz();
	InitGula();
	InitHasab();

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

	// VAO-beli attribútumok hozzárendelése a shader változókhoz
	// FONTOS: linkelés elõtt kell ezt megtenni!
	glBindAttribLocation(	m_programID,	// shader azonosítója, amibõl egy változóhoz szeretnénk hozzárendelést csinálni
							0,				// a VAO-beli azonosító index
							"vs_in_pos");	// a shader-beli változónév
	glBindAttribLocation( m_programID, 1, "vs_in_col");

	// illesszük össze a shadereket (kimenõ-bemenõ változók összerendelése stb.)
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
	// egyéb inicializálás
	//

	// vetítési mátrix létrehozása
	m_matProj = glm::perspective( 45.0f, 640/480.0f, 1.0f, 1000.0f );

	// shader-beli transzformációs mátrixok címének lekérdezése
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
	// nézeti transzformáció beállítása
	float t = SDL_GetTicks()/1000.0f;
	m_matView = glm::lookAt(glm::vec3( 15,  10,  15),		// honnan nézzük a színteret
							glm::vec3( 0,  0,  0),		// a színtér melyik pontját nézzük
							glm::vec3( 0,  1,  0));		// felfelé mutató irány a világban
}

void CMyApp::DrawTorusz(glm::mat4 m_matWorld)
{
	// kapcsoljuk be a VAO-t (a VBO jön vele együtt)
	glBindVertexArray(m_vaoID);

	glm::mat4 mvp = m_matProj * m_matView * m_matWorld;

	// majd küldjük át a megfelelõ mátrixot!
	// Uniform változó bindolása csak a program bindolása után lehetséges! (glUseProgram() )
	glUniformMatrix4fv(m_loc_mvp,// erre a helyre töltsünk át adatot
		1,			// egy darab mátrixot
		GL_FALSE,	// NEM transzponálva
		&(mvp[0][0])); // innen olvasva a 16 x sizeof(float)-nyi adatot

	// kirajzolás
	//A draw hívásokhoz a VAO és a program bindolva kell legyenek (glUseProgram() és glBindVertexArray())

	glDrawElements(GL_TRIANGLES,		// primitív típus
		3 * 2 * (N) * (M),		// hany csucspontot hasznalunk a kirajzolashoz
		GL_UNSIGNED_SHORT,	// indexek tipusa
		0);					// indexek cime


	// VAO kikapcsolasa
	glBindVertexArray(0);

}

void CMyApp::DrawGula(glm::mat4 m_matWorld)
{
	// kapcsoljuk be a VAO-t (a VBO jön vele együtt)
	glBindVertexArray(m_gula_vaoID);

	glm::mat4 mvp = m_matProj * m_matView * m_matWorld;

	glUniformMatrix4fv(m_loc_mvp,// erre a helyre töltsünk át adatot
		1,			// egy darab mátrixot
		GL_FALSE,	// NEM transzponálva
		&(mvp[0][0])); // innen olvasva a 16 x sizeof(float)-nyi adatot

	glDrawElements(GL_TRIANGLES,		// primitív típus
		3 * 2*(gulaN),		// hany csucspontot hasznalunk a kirajzolashoz
		GL_UNSIGNED_SHORT,	// indexek tipusa
		0);					// indexek cime
	glBindVertexArray(0);
}


void CMyApp::DrawHasab(glm::mat4 m_matWorld)
{
	// kapcsoljuk be a VAO-t (a VBO jön vele együtt)
	glBindVertexArray(m_hasab_vaoID);

	glm::mat4 mvp = m_matProj * m_matView * m_matWorld;

	glUniformMatrix4fv(m_loc_mvp,// erre a helyre töltsünk át adatot
		1,			// egy darab mátrixot
		GL_FALSE,	// NEM transzponálva
		&(mvp[0][0])); // innen olvasva a 16 x sizeof(float)-nyi adatot

	glDrawArrays(GL_TRIANGLE_STRIP,0,2*hasabN+2);
	glDrawArrays(GL_TRIANGLE_FAN, 2 * hasabN + 2,hasabN+2);
	glDrawArrays(GL_TRIANGLE_FAN, 2 * hasabN + 2+hasabN+2,hasabN+2);
	glBindVertexArray(0);
}

void CMyApp::Render()
{
	// töröljük a frampuffert (GL_COLOR_BUFFER_BIT) és a mélységi Z puffert (GL_DEPTH_BUFFER_BIT)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// shader bekapcsolasa, ebben a projektben a teljes programot jelöli, hisz nem váltunk a shaderek között
	glUseProgram( m_programID );

	// shader parameterek beállítása
	/*

	GLM transzformációs mátrixokra példák:
		glm::rotate<float>( szög, glm::vec3(tengely_x, tengely_y, tengely_z) ) <- tengely_{xyz} körüli elforgatás
		glm::translate<float>( glm::vec3(eltol_x, eltol_y, eltol_z) ) <- eltolás
		glm::scale<float>( glm::vec3(s_x, s_y, s_z) ) <- léptékezés

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
		//Ide a - os rész, 0 alá ne lehessen menni.
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

// a két paraméterbe az új ablakméret szélessége (_w) és magassága (_h) található
void CMyApp::Resize(int _w, int _h)
{
	glViewport(0, 0, _w, _h);

	m_matProj = glm::perspective(  45.0f,		// 90 fokos nyilasszog
									_w/(float)_h,	// ablakmereteknek megfelelo nezeti arany
									0.01f,			// kozeli vagosik
									100.0f);		// tavoli vagosik
}