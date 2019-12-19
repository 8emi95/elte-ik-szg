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

	// generáljunk egy textúra erõforrás nevet
    glGenTextures(1, &tmpID);
	// aktiváljuk a most generált nevû textúrát
    glBindTexture(GL_TEXTURE_2D, tmpID);
	// töltsük fel adatokkal az...
    gluBuild2DMipmaps(  GL_TEXTURE_2D,	// aktív 2D textúrát
						GL_RGB8,		// a vörös, zöld és kék csatornákat 8-8 biten tárolja a textúra
						256, 256,		// 256x256 méretû legyen
						GL_RGB,				// a textúra forrása RGB értékeket tárol, ilyen sorrendben
						GL_UNSIGNED_BYTE,	// egy-egy színkopmonenst egy unsigned byte-ról kell olvasni
						tex);				// és a textúra adatait a rendszermemória ezen szegletébõl töltsük fel
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// bilineáris szûrés kicsinyítéskor
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	// és nagyításkor is
	glBindTexture(GL_TEXTURE_2D, 0);

	return tmpID;
}
*/

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

	// definiáljuk a körlap csúcsait
	std::vector<Vertex> vertex_vector;
	
	// egy Vertex kerül középre (vegyyük észre, hogy a textúra közepe a (0.5, 0.5) koordináta!)
	//									X  Y  Z				R  G  B				S    T
	vertex_vector.push_back({ glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec2(0.5, 0.5) });
	// Megjegyzés: a modell RGB színét nem használjuk (lásd fragment shader kódja), csak a textúrából
	// származó színt jelenítjük meg. Valójában ki is törölhetnénk a Vertexek szín-attribútumát.
	

	// Lerakunk N darab Vertexet egy 2 sugarú, origó középpontú körvonal mentén.
	// A textúra-koordináták egy (0.5, 0.5) középpontú, 0.5 sugarú körlapot követnek! (A textúra közepébõl kivágunk egy maximális sugarú körlapot.)
	// Tehát (s,t) textúra-koordináta egy Vertex esetén s = 0.5*cosf(a) + 0.5, és t = 0.5*sinf(a) + 0.5.
	// Csakhogy a textúra-koordinarendszerben az origó a bal alsó sarok! Viszont a képet beolvasáskor felülrõl lefelé olvassuk.
	// Vagyis az y tengely tükrözött a modell-koordinátarendszerhez képest.
	// Azaz alapból fejjel lefelé jelenne meg a textúra.
	// Ezért a Vertexek (s,t) koordináták helyett (s, 1-t) koordinátákat használunk.
	for (int i = 0; i < N; ++i){
		float a = i * 2 * 3.14152f / N; // szög egy adott vertexhez (radiánban)

		//									X		   Y		  Z				R  G  B				S					 T
		vertex_vector.push_back({ glm::vec3(2*cosf(a), 2*sinf(a), 0), glm::vec3(0, 0, 0), glm::vec2(0.5 * cosf(a) + 0.5, 1 - (0.5 * sinf(a) + 0.5)) });
	}

	// indexpuffer adatai
	std::vector<GLushort> index_vector;
	
	// Kirajzoláshoz GL_TRIANGLES-t használunk.
	// Ezért az index pufferbe belekerül az elsõ N-1 háromszög... (körüljárási irányra ügyeljünk!)
	for (int i = 0; i < N-1; ++i){
		index_vector.push_back(0);
		index_vector.push_back(i+1);
		index_vector.push_back(i+2);
	}
	// ...és végül az utolsó háromszög, ahol körbeérünk az indexekben
	index_vector.push_back(0);
	index_vector.push_back(N);
	index_vector.push_back(1);
	// Megjegyzés: egyszerûbb lenne az index_vector, ha nem GL_TRIANGLES-t, hanem GL_TRIANGLE_FAN-t használnánk.
	// Gyakorlásképp módosítsuk a programunkat ennek megfelelõen, hogy a kirajzolt eredmény ugyanaz maradjon!

	/*
	// indexpuffer adatai
    GLushort indices[]=
    {
		// 1. háromszög
        0,1,2,
		// 2. háromszög
        2,1,3,
    };
	*/

	// 1 db VAO foglalasa
	glGenVertexArrays(1, &m_vaoID);
	// a frissen generált VAO beallitasa aktívnak
	glBindVertexArray(m_vaoID);
	
	// hozzunk létre egy új VBO erõforrás nevet
	glGenBuffers(1, &m_vboID); 
	glBindBuffer(GL_ARRAY_BUFFER, m_vboID); // tegyük "aktívvá" a létrehozott VBO-t
	// töltsük fel adatokkal az aktív VBO-t
	glBufferData( GL_ARRAY_BUFFER,	// az aktív VBO-ba töltsünk adatokat
				  vertex_vector.size() * sizeof(Vertex),		// ennyi bájt nagyságban
				  &vertex_vector[0],	// errõl a rendszermemóriabeli címrõl olvasva
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
		(void*)(2*sizeof(glm::vec3)) );

	// index puffer létrehozása
	glGenBuffers(1, &m_ibID);
	// a VAO észreveszi, hogy bind-olunk egy index puffert és feljegyzi, hogy melyik volt ez!
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_vector.size() * sizeof(GLushort), &index_vector[0], GL_STATIC_DRAW);

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
	
	m_loc_texture1 = glGetUniformLocation( m_programID, "texImage1" );
	m_loc_texture2 = glGetUniformLocation( m_programID, "texImage2" );
	m_loc_t =		 glGetUniformLocation( m_programID, "t" );

	//
	// egyéb erõforrások betöltése
	//

	// textúra betöltése
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
	// nézeti transzformáció beállítása
	m_matView = glm::lookAt(glm::vec3( 0,  0,  5),		// honnan nézzük a színteret
							glm::vec3( 0,  0,  0),		// a színtér melyik pontját nézzük
							glm::vec3( 0,  1,  0));		// felfelé mutató irány a világban
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

	// töröljük a frampuffert (GL_COLOR_BUFFER_BIT) és a mélységi Z puffert (GL_DEPTH_BUFFER_BIT)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// shader bekapcsolasa
	glUseProgram( m_programID );

	// shader parameterek beállítása
	/*

	GLM transzformációs mátrixokra példák:
		glm::rotate<float>( szög, tengely_x, tengely_y, tengely_z) <- tengely_{xyz} körüli elforgatás
		glm::translate<float>( eltol_x, eltol_y, eltol_z) <- eltolás
		glm::scale<float>( s_x, s_y, s_z ) <- léptékezés

	*/

	// rot szöggel forgatjuk el az objektumot
	// 360										-> egy teljes kör (fokban)
	// 360 * SDL_GetTicks()						-> mp-ként 1000-szer forog körbe (mp-ént érjük el 360 egész többszöröseit)
	// 360 * SDL_GetTicks() / 1000.0f			-> mp-ként 1-szer forog körbe
	// 360 * SDL_GetTicks() / 1000.0f / 10.0f	-> mp-ként 1/10-szer forog körbe, vagyis 10 mp-ként forog körbe teljesen
	float rot = 360 * SDL_GetTicks() / 1000.0f / 10.0f;

	m_matWorld = glm::rotate<float>(rot,glm::vec3( 0, 1, 0));

	glm::mat4 mvp = m_matProj * m_matView * m_matWorld;

	// majd küldjük át a megfelelõ mátrixot!
	glUniformMatrix4fv( m_loc_mvp,// erre a helyre töltsünk át adatot
						1,			// egy darab mátrixot
						GL_FALSE,	// NEM transzponálva
						&(mvp[0][0]) ); // innen olvasva a 16 x sizeof(float)-nyi adatot
	
	// aktiváljuk a 0-és textúra mintavételezõ egységet
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_atomicTextureID);
	// aktiváljuk a generált textúránkat 
	// textúra mintavételezõ és shader-beli sampler2D összerendelése
	glUniform1i(	m_loc_texture1,	// ezen azonosítójú sampler 2D
					0);				// olvassa az ezen indexû mintavételezõt

	// az 1-es textúra mintavételezõ egységhez másik textúra tartozik
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_donotTextureID);
	glUniform1i(m_loc_texture2, 1);

	// t értéke -1 és +1 között ciklizál, ha sinf-be rakunk egy idõben változó értéket!
	// Ne felejtsük el, hogy a sinf() radiánt vár!
	// 2 * 3.14152f										-> egy teljes periódus (radiánban 2*PI)
	// 2 * 3.14152f * SDL_GetTicks()					-> mp-ként 1000 periódus
	// 2 * 3.14152f * SDL_GetTicks() / 1000.0f			-> mp-ként 1 periódus
	// 2 * 3.14152f * SDL_GetTicks() / 1000.0f / 6.0f	-> mp-ként 1/6 periódus, azaz 6 mp-ként 1 periódus
	float t = sinf(2 * 3.14152f * SDL_GetTicks() / 1000.0f / 4.0f);
	
	// t értékét skáláznunk kell, hogy ne a [-1; +1], hanem a [0; +1] tartományban ciklizáljon
	t = (t + 1) / 2.0f;

	std::cout << "t erteke: " << t << "\n";
	glUniform1f(m_loc_t, t);

	// kapcsoljuk be a VAO-t (a VBO jön vele együtt)
	glBindVertexArray(m_vaoID);

	// kirajzolás
	glDrawElements(	GL_TRIANGLES,		// primitív típus
					3*N,				// hany csucspontot hasznalunk a kirajzolashoz
					GL_UNSIGNED_SHORT,	// indexek tipusa
					0);					// indexek cime

	// VAO kikapcsolasa
	glBindVertexArray(0);

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

// a két paraméterbe az új ablakméret szélessége (_w) és magassága (_h) található
void CMyApp::Resize(int _w, int _h)
{
	glViewport(0, 0, _w, _h);

	m_matProj = glm::perspective(  45.0f,		// 90 fokos nyilasszog
									_w/(float)_h,	// ablakmereteknek megfelelo nezeti arany
									0.01f,			// kozeli vagosik
									100.0f);		// tavoli vagosik
}