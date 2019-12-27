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

bool CMyApp::Init()
{
	// törlési szín legyen kékes
	glClearColor(0.125f, 0.25f, 0.5f, 1.0f);

	glEnable(GL_CULL_FACE); // kapcsoljuk be a hatrafele nezo lapok eldobasat
	glEnable(GL_DEPTH_TEST); // mélységi teszt bekapcsolása (takarás)
	glCullFace(GL_BACK); // GL_BACK: a kamerától "elfelé" néző lapok, GL_FRONT: a kamera felé néző lapok

	//
	// geometria letrehozasa
	//

	float ran = ((float)rand() / (RAND_MAX));

	Vertex vert[] ={
	{
			//          x,  y, z             R, G, B


			glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)}, //0
		{glm::vec3( 0.25, 1, -0.25), glm::vec3(1, 0, 0)}, //1
		{glm::vec3(0.25, 1, 0.25), glm::vec3(0, 1, 0)}, //2
		{glm::vec3( -0.25, 1, 0.25), glm::vec3(0, 0, 1)}, //3
		{glm::vec3(-0.25, 1, -0.25), glm::vec3(1, 0.9, 1) },//4

		{ glm::vec3(-0.25, -1, 0.25), glm::vec3(0, 1, 1) },//5
		{ glm::vec3(-0.25,  -1, -0.25), glm::vec3(1, 0, 1) },//6
		{ glm::vec3(0.25, -1, -0.25), glm::vec3(1, 1, 0) },//7
		{ glm::vec3(0.25, -1, 0.25), glm::vec3(0.2, 0.3, 0.4) }//8


	};
	


	

	

	GLushort indices[] =
	{
		//felső 
		1,2,3,
		1,3,4,
		
		0,1,4,
		0,3,2,
		0,2,1,
		0,4,3,
		
		//alsó
		5,8,7,
		5,7,6,

		0,6,7,
		0,7,8,
		0,8,5,
		0,5,6


	};

	// 1 db VAO foglalasa
	glGenVertexArrays(1, &m_vaoID);
	// a frissen generált VAO beallitasa aktívnak
	glBindVertexArray(m_vaoID);
	
	// hozzunk létre egy új VBO erőforrás nevet
	glGenBuffers(1, &m_vboID); 
	glBindBuffer(GL_ARRAY_BUFFER, m_vboID); // tegyük "aktívvá" a létrehozott VBO-t
	// töltsük fel adatokkal az aktív VBO-t
	glBufferData( GL_ARRAY_BUFFER,	// az aktív VBO-ba töltsünk adatokat
				  sizeof(vert),		// ennyi bájt nagyságban
				  vert,	// erről a rendszermemóriabeli címről olvasva
				  GL_STATIC_DRAW);	// úgy, hogy a VBO-nkba nem tervezünk ezután írni és minden kirajzoláskor felhasnzáljuk a benne lévő adatokat
	

	// VAO-ban jegyezzük fel, hogy a VBO-ban az első 3 float sizeof(Vertex)-enként lesz az első attribútum (pozíció)
	glEnableVertexAttribArray(0); // ez lesz majd a pozíció
	glVertexAttribPointer(
		0,				// a VB-ben található adatok közül a 0. "indexű" attribútumait állítjuk be
		3,				// komponens szam
		GL_FLOAT,		// adatok tipusa
		GL_FALSE,		// normalizalt legyen-e
		sizeof(Vertex),	// stride (0=egymas utan)
		0				// a 0. indexű attribútum hol kezdődik a sizeof(Vertex)-nyi területen belül
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

	// index puffer létrehozása
	glGenBuffers(1, &m_ibID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindVertexArray(0); // feltöltüttük a VAO-t, kapcsoljuk le
	glBindBuffer(GL_ARRAY_BUFFER, 0); // feltöltöttük a VBO-t is, ezt is vegyük le
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // feltöltöttük a VBO-t is, ezt is vegyük le

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
	// FONTOS: linkelés előtt kell ezt megtenni!
	glBindAttribLocation(	m_programID,	// shader azonosítója, amiből egy változóhoz szeretnénk hozzárendelést csinálni
							0,				// a VAO-beli azonosító index
							"vs_in_pos");	// a shader-beli változónév
	glBindAttribLocation( m_programID, 1, "vs_in_col");

	// illesszük össze a shadereket (kimenő-bemenő változók összerendelése stb.)
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
	m_loc_color = glGetUniformLocation(m_programID, "color");

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
	m_matView = glm::lookAt(glm::vec3( 10,  0,  25),		// honnan nézzük a színteret
							glm::vec3( 0,  0,  0),		// a színtér melyik pontját nézzük
							glm::vec3( 0,  1,  0));		// felfelé mutató irány a világban
}


void CMyApp::Render()
{
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// shader bekapcsolasa
	glUseProgram(m_programID);



	// kapcsoljuk be a VAO-t (a VBO jön vele együtt)
	glBindVertexArray(m_vaoID);

	float bigCircleAlpha = SDL_GetTicks() / 7000.0f *6.2831f;
	float bigCircleRadius = 6.0f;
	float bigCircleY = bigCircleRadius * cosf(bigCircleAlpha);
	float bigCircleZ = bigCircleRadius * sinf(bigCircleAlpha);
	glm::mat4 bigCircleTransform = glm::translate(glm::vec3(0,bigCircleY, bigCircleZ));

	for (int i = 0; i < 6; i++)
	{
		float alpha = (6.2831 / 6.0f)* i;
		float radius = 5.0f;

		//1. megoldás: kitoljuk x=5-be, utána forgatjuk
		glm::mat4 translation = glm::translate(glm::vec3(0, 0, radius));
		glm::mat4 rotation = glm::rotate(alpha, glm::vec3(1, 0, 0));
		m_matWorld = bigCircleTransform * rotation * translation;

		//2. megoldás: polárkoordinátákkal
		/* float y = radius * cosf(alpha);
		float z = radius * sinf(alpha);
		glm::mat4 polarTranslation = glm::translate(glm::vec3( 0,y, z));
		m_matWorld = bigCircleTransform * polarTranslation; */

		glm::mat4 mvp = m_matProj * m_matView * m_matWorld;

		
		glUniformMatrix4fv(m_loc_mvp,
			1,			
			GL_FALSE,	
			&(mvp[0][0])); 

						   
		glDrawElements(GL_TRIANGLES,		
			36,					// hany csucspontot hasznalunk a kirajzolashoz
			GL_UNSIGNED_SHORT,	// indexek tipusa
			0);					// indexek cime
	}

	// VAO kikapcsolasa
	glBindVertexArray(0);

	// shader kikapcsolasa
	glUseProgram(0);


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