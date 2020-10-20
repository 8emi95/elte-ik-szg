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
	glCullFace(GL_BACK); // GL_BACK: a kamerától "elfelé" nézõ lapok, GL_FRONT: a kamera felé nézõ lapok

	//
	// geometria letrehozasa
	//

	Vertex vert[] =
	{
		//közepe
		//			x , y,z
		//eleje
		{ glm::vec3(-1,	1,	1),	glm::vec3(1,	1,	1) },
		{ glm::vec3(-1,	0,	1),	glm::vec3(1,	1,	1) },
		{ glm::vec3(0,	0,	1),	glm::vec3(1,	1,	1) },

		{ glm::vec3(0,	0,	1),	glm::vec3(0,	1,	1) },
		{ glm::vec3(0,	1,	1),	glm::vec3(1,	1,	1) },
		{ glm::vec3(-1,	1,	1),	glm::vec3(1,	1,	1) },

		//hátulja
		{ glm::vec3(0,	0,	0),	glm::vec3(1,	0,	1) },
		{ glm::vec3(-1,	0,	0),	glm::vec3(1,	1,	1) },
		{ glm::vec3(-1,	1,	0),	glm::vec3(1,	1,	1) },

		{ glm::vec3(-1,	1,	0),	glm::vec3(1,	1,	0) },
		{ glm::vec3(0,	1,	0),	glm::vec3(1,	1,	1) },
		{ glm::vec3(0,	0,	0),	glm::vec3(1,	1,	1) },

		//jobboldal
		{ glm::vec3(0,	0,	1),	glm::vec3(1,	1,	1) },
		{ glm::vec3(0,	0,	0),	glm::vec3(0,	1,	1) },
		{ glm::vec3(0,	1,	0),	glm::vec3(1,	1,	1) },


		{ glm::vec3(0,	0,	1),	glm::vec3(1,	1,	1) },
		{ glm::vec3(0,	1,	0),	glm::vec3(1,	0,	1) },
		{ glm::vec3(0,	1,	1),	glm::vec3(1,	1,	1) },

		//baloldal
		{ glm::vec3(-1,	1,	0),	glm::vec3(1,	1,	1) },
		{ glm::vec3(-1,	0,	0),	glm::vec3(1,	1,	0) },
		{ glm::vec3(-1,	0,	1),	glm::vec3(1,	1,	1) },


		{ glm::vec3(-1,	1,	1),	glm::vec3(1,	1,	1) },
		{ glm::vec3(-1,	1,	0),	glm::vec3(1,	1,	1) },
		{ glm::vec3(-1,	0,	1),	glm::vec3(0,	1,	1) },

		//teteje
		{ glm::vec3(-1,	1,	1),	glm::vec3(1,	1,	1) },
		{ glm::vec3(0,	1,	1),	glm::vec3(1,	1,	1) },
		{ glm::vec3(0,	1,	0),	glm::vec3(1,	0,	1) },

		{ glm::vec3(-1,	1,	0),	glm::vec3(1,	1,	1) },
		{ glm::vec3(-1,	1,	1),	glm::vec3(1,	1,	1) },
		{ glm::vec3(0,	1,	0),	glm::vec3(1,	1,	0) },

		//alja
		{ glm::vec3(-1,	0,	0),	glm::vec3(0,	0,	1) },
		{ glm::vec3(0,	0,	0),	glm::vec3(1,	1,	1) },
		{ glm::vec3(0,	0,	1),	glm::vec3(1,	1,	1) },

		{ glm::vec3(0,	0,	1),	glm::vec3(0,	1,	0) },
		{ glm::vec3(-1,	0,	1),	glm::vec3(1,	1,	1) },
		{ glm::vec3(-1,	0,	0),	glm::vec3(1,	1,	1) },



		//baloldala
		//			x , y,z
		//eleje
		{ glm::vec3(-2,	1,	1),	glm::vec3(0,	1,	1) },
		{ glm::vec3(-2,	0,	1),	glm::vec3(0,	1,	1) },
		{ glm::vec3(-1,	0,	1),	glm::vec3(1,	1,	1) },

		{ glm::vec3(-1,	0,	1),	glm::vec3(0,	1,	1) },
		{ glm::vec3(-1,	1,	1),	glm::vec3(1,	0,	1) },
		{ glm::vec3(-2,	1,	1),	glm::vec3(1,	1,	1) },

		//hátulja
		{ glm::vec3(-1,	0,	0),	glm::vec3(0,	1,	1) },
		{ glm::vec3(-2,	0,	0),	glm::vec3(1,	1,	0) },
		{ glm::vec3(-2,	1,	0),	glm::vec3(1,	1,	1) },

		{ glm::vec3(-2,	1,	0),	glm::vec3(0,	1,	1) },
		{ glm::vec3(-1,	1,	0),	glm::vec3(1,	1,	1) },
		{ glm::vec3(-1,	0,	0),	glm::vec3(0,	1,	1) },

		//jobboldal
		{ glm::vec3(-1,	0,	1),	glm::vec3(0,	1,	1) },
		{ glm::vec3(-1,	0,	0),	glm::vec3(1,	1,	1) },
		{ glm::vec3(-1,	1,	0),	glm::vec3(1,	0,	1) },


		{ glm::vec3(-1,	0,	1),	glm::vec3(0,	1,	1) },
		{ glm::vec3(-1,	1,	0),	glm::vec3(1,	1,	1) },
		{ glm::vec3(-1,	1,	1),	glm::vec3(1,	1,	0) },

		//baloldal
		{ glm::vec3(-2,	1,	0),	glm::vec3(0,	0,	0) },
		{ glm::vec3(-2,	0,	0),	glm::vec3(1,	1,	1) },
		{ glm::vec3(-2,	0,	1),	glm::vec3(1,	1,	1) },


		{ glm::vec3(-2,	1,	1),	glm::vec3(0,	0,	1) },
		{ glm::vec3(-2,	1,	0),	glm::vec3(0,	1,	1) },
		{ glm::vec3(-2,	0,	1),	glm::vec3(1,	1,	1) },

		//teteje
		{ glm::vec3(-2,	1,	1),	glm::vec3(0,	0,	1) },
		{ glm::vec3(-1,	1,	1),	glm::vec3(1,	0,	1) },
		{ glm::vec3(-1,	1,	0),	glm::vec3(1,	1,	1) },

		{ glm::vec3(-2,	1,	0),	glm::vec3(0,	0,	1) },
		{ glm::vec3(-2,	1,	1),	glm::vec3(1,	1,	0) },
		{ glm::vec3(-1,	1,	0),	glm::vec3(1,	1,	1) },

		//alja
		{ glm::vec3(-2,	0,	0),	glm::vec3(0,	0,	1) },
		{ glm::vec3(-1,	0,	0),	glm::vec3(1,	1,	1) },
		{ glm::vec3(-1,	0,	1),	glm::vec3(0,	1,	1) },

		{ glm::vec3(-1,	0,	1),	glm::vec3(0,	0,	1) },
		{ glm::vec3(-2,	0,	1),	glm::vec3(1,	1,	1) },
		{ glm::vec3(-2,	0,	0),	glm::vec3(1,	0,	1) },



		//jobb oldala
		//			x , y,z
		//eleje
		{ glm::vec3(0,	1,	1),	glm::vec3(0,	0,	1) },
		{ glm::vec3(0,	0,	1),	glm::vec3(1,	1,	1) },
		{ glm::vec3(1,	0,	1),	glm::vec3(1,	1,	0) },

		{ glm::vec3(1,	0,	1),	glm::vec3(0,	0,	0) },
		{ glm::vec3(1,	1,	1),	glm::vec3(0,	1,	1) },
		{ glm::vec3(0,	1,	1),	glm::vec3(1,	1,	1) },

		//hátulja
		{ glm::vec3(1,	0,	0),	glm::vec3(0,	0,	0) },
		{ glm::vec3(0,	0,	0),	glm::vec3(1,	0,	1) },
		{ glm::vec3(0,	1,	0),	glm::vec3(1,	1,	1) },

		{ glm::vec3(0,	1,	0),	glm::vec3(0,	0,	0) },
		{ glm::vec3(1,	1,	0),	glm::vec3(1,	1,	0) },
		{ glm::vec3(1,	0,	0),	glm::vec3(1,	1,	1) },

		//jobboldal
		{ glm::vec3(1,	0,	1),	glm::vec3(0,	0,	0) },
		{ glm::vec3(1,	0,	0),	glm::vec3(1,	1,	1) },
		{ glm::vec3(1,	1,	0),	glm::vec3(0,	1,	1) },


		{ glm::vec3(1,	0,	1),	glm::vec3(0,	0,	0) },
		{ glm::vec3(1,	1,	0),	glm::vec3(1,	1,	1) },
		{ glm::vec3(1,	1,	1),	glm::vec3(1,	0,	1) },

		//baloldal
		{ glm::vec3(0,	1,	0),	glm::vec3(0,	0,	0) },
		{ glm::vec3(0,	0,	0),	glm::vec3(1,	1,	1) },
		{ glm::vec3(0,	0,	1),	glm::vec3(1,	1,	0) },


		{ glm::vec3(0,	1,	1),	glm::vec3(0,	0,	0) },
		{ glm::vec3(0,	1,	0),	glm::vec3(0,	0,	1) },
		{ glm::vec3(0,	0,	1),	glm::vec3(1,	1,	1) },

		//teteje
		{ glm::vec3(0,	1,	1),	glm::vec3(0,	0,	0) },
		{ glm::vec3(1,	1,	1),	glm::vec3(0,	1,	0) },
		{ glm::vec3(1,	1,	0),	glm::vec3(1,	1,	1) },

		{ glm::vec3(0,	1,	0),	glm::vec3(0,	0,	0) },
		{ glm::vec3(0,	1,	1),	glm::vec3(0,	1,	1) },
		{ glm::vec3(1,	1,	0),	glm::vec3(0,	1,	1) },

		//alja
		{ glm::vec3(0,	0,	0),	glm::vec3(0,	0,	0) },
		{ glm::vec3(1,	0,	0),	glm::vec3(0,	1,	1) },
		{ glm::vec3(1,	0,	1),	glm::vec3(1,	0,	1) },

		{ glm::vec3(1,	0,	1),	glm::vec3(0,	0,	0) },
		{ glm::vec3(0,	0,	1),	glm::vec3(0,	1,	1) },
		{ glm::vec3(0,	0,	0),	glm::vec3(1,	1,	0) },



		//teteje
		//			x , y,z
		//eleje
		{ glm::vec3(-1,	2,	1),	glm::vec3(0,	0,	0) },
		{ glm::vec3(-1,	1,	1),	glm::vec3(0,	0,	0) },
		{ glm::vec3(0,	1,	1),	glm::vec3(1,	1,	1) },

		{ glm::vec3(0,	1,	1),	glm::vec3(0,	0,	0) },
		{ glm::vec3(0,	2,	1),	glm::vec3(0,	0,	1) },
		{ glm::vec3(-1,	2,	1),	glm::vec3(0,	1,	1) },

		//hátulja
		{ glm::vec3(0,	1,	0),	glm::vec3(0,	0,	0) },
		{ glm::vec3(-1,	1,	0),	glm::vec3(0,	0,	1) },
		{ glm::vec3(-1,	2,	0),	glm::vec3(1,	0,	1) },

		{ glm::vec3(-1,	2,	0),	glm::vec3(0,	0,	0) },
		{ glm::vec3(0,	2,	0),	glm::vec3(0,	0,	1) },
		{ glm::vec3(0,	1,	0),	glm::vec3(1,	1,	0) },

		//jobboldal
		{ glm::vec3(0,	1,	1),	glm::vec3(0,	0,	0) },
		{ glm::vec3(0,	1,	0),	glm::vec3(0,	0,	0) },
		{ glm::vec3(0,	2,	0),	glm::vec3(0,	1,	1) },


		{ glm::vec3(0,	1,	1),	glm::vec3(0,	0,	0) },
		{ glm::vec3(0,	2,	0),	glm::vec3(0,	0,	0) },
		{ glm::vec3(0,	2,	1),	glm::vec3(0,	0,	1) },

		//baloldal
		{ glm::vec3(-1,	2,	0),	glm::vec3(1,	1,	0) },
		{ glm::vec3(-1,	1,	0),	glm::vec3(1,	0,	0) },
		{ glm::vec3(-1,	1,	1),	glm::vec3(1,	0,	0) },


		{ glm::vec3(-1,	2,	1),	glm::vec3(1,	0,	1) },
		{ glm::vec3(-1,	2,	0),	glm::vec3(1,	0,	0) },
		{ glm::vec3(-1,	1,	1),	glm::vec3(1,	0,	0) },

		//teteje
		{ glm::vec3(-1,	2,	1),	glm::vec3(1,	0,	0) },
		{ glm::vec3(0,	2,	1),	glm::vec3(1,	1,	0) },
		{ glm::vec3(0,	2,	0),	glm::vec3(1,	0,	0) },

		{ glm::vec3(-1,	2,	0),	glm::vec3(1,	0,	0) },
		{ glm::vec3(-1,	2,	1),	glm::vec3(1,	0,	1) },
		{ glm::vec3(0,	2,	0),	glm::vec3(1,	0,	0) },

		//alja
		{ glm::vec3(-1,	1,	0),	glm::vec3(1,	0,	0) },
		{ glm::vec3(0,	1,	0),	glm::vec3(1,	0,	0) },
		{ glm::vec3(0,	1,	1),	glm::vec3(1,	1,	0) },

		{ glm::vec3(0,	1,	1),	glm::vec3(1,	0,	0) },
		{ glm::vec3(-1,	1,	1),	glm::vec3(1,	0,	0) },
		{ glm::vec3(-1,	1,	0),	glm::vec3(1,	0,	1) },


		//alja
		//			x , y,z
		//eleje
		{ glm::vec3(-1,	0,	1),	glm::vec3(1,	1,	1) },
		{ glm::vec3(-1,	-1,	1),	glm::vec3(1,	0,	0) },
		{ glm::vec3(0,	-1,	1),	glm::vec3(1,	0,	0) },

		{ glm::vec3(0,	-1,	1),	glm::vec3(1,	1,	0) },
		{ glm::vec3(0,	0,	1),	glm::vec3(1,	1,	0) },
		{ glm::vec3(-1,	0,	1),	glm::vec3(1,	0,	0) },

		//hátulja
		{ glm::vec3(0,	-1,	0),	glm::vec3(1,	1,	0) },
		{ glm::vec3(-1,	-1,	0),	glm::vec3(1,	0,	1) },
		{ glm::vec3(-1,	0,	0),	glm::vec3(1,	0,	0) },

		{ glm::vec3(-1,	0,	0),	glm::vec3(1,	1,	0) },
		{ glm::vec3(0,	0,	0),	glm::vec3(1,	0,	0) },
		{ glm::vec3(0,	-1,	0),	glm::vec3(1,	1,	0) },

		//jobboldal
		{ glm::vec3(0,	-1,	1),	glm::vec3(1,	1,	0) },
		{ glm::vec3(0,	-1,	0),	glm::vec3(1,	0,	0) },
		{ glm::vec3(0,	0,	0),	glm::vec3(1,	0,	1) },


		{ glm::vec3(0,	-1,	1),	glm::vec3(1,	1,	1) },
		{ glm::vec3(0,	0,	0),	glm::vec3(1,	1,	0) },
		{ glm::vec3(0,	0,	1),	glm::vec3(1,	0,	0) },

		//baloldal
		{ glm::vec3(-1,	0,	0),	glm::vec3(1,	1,	1) },
		{ glm::vec3(-1,	-1,	0),	glm::vec3(1,	0,	1) },
		{ glm::vec3(-1,	-1,	1),	glm::vec3(1,	0,	0) },


		{ glm::vec3(-1,	0,	1),	glm::vec3(1,	1,	1) },
		{ glm::vec3(-1,	0,	0),	glm::vec3(1,	0,	0) },
		{ glm::vec3(-1,	-1,	1),	glm::vec3(1,	1,	0) },

		//teteje
		{ glm::vec3(-1,	0,	1),	glm::vec3(1,	1,	1) },
		{ glm::vec3(0,	0,	1),	glm::vec3(1,	0,	0) },
		{ glm::vec3(0,	0,	0),	glm::vec3(1,	0,	1) },

		{ glm::vec3(-1,	0,	0),	glm::vec3(1,	1,	1) },
		{ glm::vec3(-1,	0,	1),	glm::vec3(1,	1,	1) },
		{ glm::vec3(0,	0,	0),	glm::vec3(1,	0,	0) },

		//alja
		{ glm::vec3(-1,	-1,	0),	glm::vec3(1,	1,	1) },
		{ glm::vec3(0,	-1,	0),	glm::vec3(1,	1,	0) },
		{ glm::vec3(0,	-1,	1),	glm::vec3(1,	1,	0) },

		{ glm::vec3(0,	-1,	1),	glm::vec3(1,	1,	0) },
		{ glm::vec3(-1,	-1,	1),	glm::vec3(1,	1,	0) },
		{ glm::vec3(-1,	-1,	0),	glm::vec3(1,	0,	1) },
	};

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
	m_loc_world = glGetUniformLocation( m_programID, "world");
	m_loc_view  = glGetUniformLocation( m_programID, "view" );
	m_loc_proj  = glGetUniformLocation( m_programID, "proj" );
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
	// nézeti transzformáció beállítása

	m_matView = glm::lookAt(glm::vec3(40, 40, 5),		// honnan nézzük a színteret
		glm::vec3(0, 0, 0),		// a színtér melyik pontját nézzük
		glm::vec3(0, 1, 0));		// felfelé mutató irány a világban
}


void CMyApp::Render()
{
	// töröljük a frampuffert (GL_COLOR_BUFFER_BIT) és a mélységi Z puffert (GL_DEPTH_BUFFER_BIT)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// shader bekapcsolasa
	glUseProgram(m_programID);

	// majd küldjük át a megfelelõ mátrixokat!
	glUniformMatrix4fv(m_loc_world,// erre a helyre töltsünk át adatot
		1,			// egy darab mátrixot
		GL_FALSE,	// NEM transzponálva
		&(m_matWorld[0][0]));
	glUniformMatrix4fv(m_loc_view, 1, GL_FALSE, &(m_matView[0][0]));
	glUniformMatrix4fv(m_loc_proj, 1, GL_FALSE, &(m_matProj[0][0]));


	if (szinvaltas != 0)
	{
		szinatmenet = sin(((SDL_GetTicks()-startTick) % 5000) / 1250.);
		GLuint szorzo_loc = glGetUniformLocation(m_programID, "szorzo");
		glUniform1f(szorzo_loc, szinatmenet);
		GLuint szin_loc = glGetUniformLocation(m_programID, "szinID");
		glUniform1i(szin_loc, szinvaltas);
		//std::cout << szinatmenet << std::endl;
		if (szinatmenet < 0) {
			szinatmenet = 0.0;
			szinvaltas = 0;
		}
	}


	// kapcsoljuk be a VAO-t (a VBO jön vele együtt)
	glBindVertexArray(m_vaoID);
	int N = 6;
	for (int i = 0; i < N; ++i) {
		
		// eltoljuk az yz tengely mentén 5 egységgel.
		glm::mat4 eltol = glm::translate<float>(0, 5, 5);

		// Ha egyenletesen akarjuk szétszórni a , akkor minden ciklus lefutásakor újabb 360/N fokokkal kell forgatnunk.
		glm::mat4 korbeszor = glm::rotate<float>(i * 360.0f / N, 0, 1, 0);

		glm::mat4 forogsugar = glm::translate<float>(10, 10, 0);
		glm::mat4 forog = glm::rotate<float>(SDL_GetTicks()/11000.0*360, 1, 1 , 0);
		

		// a szorzás sorrendje a logikai sorrendhez képest FORDÍTOTT!
		m_matWorld =
			forog *
			forogsugar *
			korbeszor *
			eltol
		 ;
		// Az újraszámolt világtranszformációt újra át kell adni a shadernek!

		glUniformMatrix4fv(m_loc_world, 1, GL_FALSE, &(m_matWorld[0][0]));
		
		glDrawArrays(GL_TRIANGLES, 0, 180);

	}

	// VAO kikapcsolasa
	glBindVertexArray(0);

	// shader kikapcsolasa
	glUseProgram(0);
}

void CMyApp::KeyboardDown(SDL_KeyboardEvent& key)
{

	if (szinvaltas != 0)
		return;
	if (key.keysym.sym == SDLK_1) {
		startTick = SDL_GetTicks();
		szinvaltas = 1;
	}
	else if (key.keysym.sym == SDLK_2) {
		startTick = SDL_GetTicks();
		szinvaltas = 2;
	}
	else if (key.keysym.sym == SDLK_3) {
		startTick = SDL_GetTicks();
		szinvaltas = 3;

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