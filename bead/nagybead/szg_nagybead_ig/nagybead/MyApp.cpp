#include "MyApp.h"
#include "GLUtils.hpp"

#include <GL/GLU.h>
#include <math.h>

#include "ObjParser_OGL3.h"

const float f_PI = 3.14159265359f;

CMyApp::CMyApp(void)
{

	m_earthTextureID = 0;
	m_earthNormalMapID = 0;
	
	m_moonTextureID = 0;
	m_moonNormalMapID = 0;

	m_masikTextureID = 0;
	m_masikNormalMapID = 0;

	m_sziklaTextureID = 0;
	m_sziklaNormalMapID = 0;


	m_sunTextureID = 0;

}


CMyApp::~CMyApp(void)
{
}

// gömb parametrikus leírása: (u,v) -> (x,y,z)
glm::vec3 CMyApp::GetUV(int mit, float u, float v) {
	glm::vec3 ret;
	switch (mit) {
		//gömb
		case 0:
		{
			u *= -2 * f_PI;
			v *= f_PI;
			ret = glm::vec3(sinf(u)*sinf(v),
				cosf(v),
				cosf(u)*sinf(v)
			);

			break;
		}
		//tórusz
		case 1:
		{
			u *= -2 * f_PI;
			v *= 2 * f_PI;
			ret = glm::vec3((TORUSZR + TORUSZV*cosf(v))*cosf(u), TORUSZV*sinf(v), (TORUSZR + TORUSZV*cosf(v))*sinf(u));
			break;
		}
		//szikla
		case 2:
		{
			u *= -2 * f_PI;
			v *= f_PI;
			srand((u * 1000 * v * 1000 * -1));
			double rnd = ((float)rand() / (RAND_MAX + 2000))-1000.0f; //0 és 2001 közötti -1000
			u += rnd;
			v += rnd;
			ret = glm::vec3(sinf(u)*sinf(v) + 0.05f*cosf(SZIKLAMERET * 20 * v),
				cosf(v),
				cosf(u)*sinf(v) + 0.05f*cosf(SZIKLAMERET * 20 * u)
			);
			break;
		}
	}
	return ret;
}


// normálvektorok számolása
glm::vec3 CMyApp::GetUV_normals(int mit, float u, float v) {

	glm::vec3 du, dv; // u és v szerinti parciális deriváltak
	glm::vec3 n; // a végsõ normálvektor

	// az alakzat paraméteres alakja az F(u,v) függvény (lásd: GetUV() eljárás)
	// Majdnem bármilyen (kellõen sima és folytonos) paraméteres felület normálvektorát
	//   megkaphatjuk egy adott (u,v) pontban az alábbi módon:
	// n(u,v) = ( dF(u,v)/du x dF(u,v)/dv ) / | dF(u,v)/du x dF(u,v)/dv |

	// Azaz venni kell az F(u,v) vektor u és v szerinti parciális deriváltját,
	// és a kapott vektorok kereszt-szorzata (egy normalizálás után) lesz a normálvektor.
	// most már adottak a parciális deriváltak (du és dv)
	// a kettõ kereszt-szorzata adja meg a felületi normálist


	switch (mit) {
		//gömb
		case 0:
		{
			u *= -2 * f_PI;
			v *= f_PI;

			du = glm::vec3(cosf(u)*sinf(v), 0, -sinf(u)*sinf(v));
			dv = glm::vec3(sinf(u)*cosf(v), -sinf(v), cosf(u)*cosf(v));
			break;
		}
		//tórusz
		case 1:
		{
			
			u *= -2 * f_PI;
			v *= 2 * f_PI;

			du = glm::vec3((TORUSZR + cosf(v)*TORUSZV)*-sinf(u), 0, (TORUSZR + cosf(v)*TORUSZV)*cosf(u));


			dv = glm::vec3(-sinf(v)*cosf(u), cosf(v), -sinf(v)*sinf(u));
			break;
		}
		//szikla
		case 2:
		{
			u *= -2 * f_PI;
			v *= f_PI;
			srand((u * 1000 * v * 1000 * -1));
			double rnd = ((float)rand() / (RAND_MAX + 2000)) - 1000.0; //0 és 2001 közötti -1000
			u += rnd;
			v += rnd;

			du = glm::vec3(cosf(u)*sinf(v), 0, -sinf(u)*sinf(v) - sinf(SZIKLAMERET * 20 * u));
			dv = glm::vec3(sinf(u)*cosf(v) - sinf(SZIKLAMERET * 20 * v), -sinf(v), cosf(u)*cosf(v));
			break;
		}
	}

	// (Ügyeljünk az irányítottságra!)
	n = glm::cross(dv, du);
	// normalizáljuk egységhosszúra
	if (glm::length(n) > 0.0f) {
		n = glm::normalize(n);
	}

	return n;


}

bool CMyApp::Init()
{
	// törlési szín legyen kékes
	glClearColor(0.125f, 0.25f, 0.5f, 1.0f);

	glEnable(GL_CULL_FACE);		// kapcsoljuk be a hatrafele nezo lapok eldobasat
	glEnable(GL_DEPTH_TEST);	// mélységi teszt bekapcsolása (takarás)

	//
	// geometria letrehozasa
	//
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// GÖMB
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	m_vb_sphere.AddAttribute(0, 3);
	m_vb_sphere.AddAttribute(1, 3);
	m_vb_sphere.AddAttribute(2, 2);

	// N*M-es rács bejárása, a vertexek (x,y,z) koordinátáját a GetUV függvénnyel kapjuk
	for (int i = 0; i <= N; ++i){
		for (int j = 0; j <= M; ++j){
			float u = i / (float)N;
			float v = j / (float)M;
			// pozíció: gömbi koordináták
			m_vb_sphere.AddData(0, GetUV(0, u, v));
			// szerencsékre gömb esetén ugyanaz a normálvektor:
			// az origóból a gömbfelszín irányába mutató irányvektor
			m_vb_sphere.AddData(1, GetUV(0, u, v));
			// a textúra-koordináták megyeznek az (u,v) pont koordinátájával
			m_vb_sphere.AddData(2, 1-u, v);
		}
	}
	// az indexek meghatározása minden négyzethez (N*M négyzetünk van)
	for (int i = 0; i < N; ++i){
		for (int j = 0; j < M; ++j){
			// az egyik háromszög a négyzet egyik fele
			m_vb_sphere.AddIndex((i)		+ (j)*		(N + 1),
								 (i)		+ (j + 1) *	(N + 1),
								 (i + 1)	+ (j) *		(N + 1)
							 );
			// a másik háromszög a négyzet másik fele
			m_vb_sphere.AddIndex((i + 1)	+ (j) *		(N + 1),
								 (i)		+ (j + 1) *	(N + 1),
								 (i + 1)	+ (j + 1) *	(N + 1)
							 );
		}
	}

	m_vb_sphere.InitBuffers();



	m_vb_torusz.AddAttribute(0, 3);
	m_vb_torusz.AddAttribute(1, 3);
	m_vb_torusz.AddAttribute(2, 2);

	// N*M-es rács bejárása, a vertexek (x,y,z) koordinátáját a GetUV függvénnyel kapjuk
	for (int i = 0; i <= N; ++i) {
		for (int j = 0; j <= M; ++j) {
			float u = i / (float)N;
			float v = j / (float)M;
			// pozíciók: a paraméteres függvény szerint
			m_vb_torusz.AddData(0, GetUV(1, u, v));
			// normálvektorok: a paraméteres függvénybõl számolva
			m_vb_torusz.AddData(1, GetUV_normals(1, u, v));
			// textúra koordináták
			// itt most 1-nél nagyobb számokat is megengedünk: ekkor a textúra minta ismétlõdni fog
			m_vb_torusz.AddData(2, 4 * (1 - u), 4 * v); // a textúra-koordinátákat más sorrendben kell bejárni!
		}
	}


	// az indexek meghatározása minden négyzethez
	for (int i = 0; i < N; ++i) {
		for (int j = 0; j < M; ++j) {
			// az egyik háromszög a négyzet egyik fele
			m_vb_torusz.AddIndex((i)+(j)*		(N + 1),
				(i)+(j + 1) *	(N + 1),
				(i + 1) + (j) *		(N + 1)
			);
			// a másik háromszög a négyzet másik fele
			m_vb_torusz.AddIndex((i + 1) + (j)*		(N + 1),
				(i)+(j + 1) *	(N + 1),
				(i + 1) + (j + 1) *	(N + 1)
			);
		}
	}

	m_vb_torusz.InitBuffers();



	m_vb_szikla.AddAttribute(0, 3);
	m_vb_szikla.AddAttribute(1, 3);
	m_vb_szikla.AddAttribute(2, 2);

	// N*M-es rács bejárása, a vertexek (x,y,z) koordinátáját a GetUV függvénnyel kapjuk
	for (int i = 0; i <= N; ++i) {
		for (int j = 0; j <= M; ++j) {
			float u = i / (float)N;
			float v = j / (float)M;
			// pozíció: gömbi koordináták
			m_vb_szikla.AddData(0, GetUV(2, u, v));
			// szerencsékre gömb esetén ugyanaz a normálvektor:
			// az origóból a gömbfelszín irányába mutató irányvektor
			// de ez csak a gömbnél van így! (Lásd fentebb a csonkakúp palást, ott másképp kellett számolnunk!)
			m_vb_szikla.AddData(1, GetUV(2, u, v));
			// a textúra-koordináták megyeznek az (u,v) pont koordinátájával
			// (kivéve, hogy a kép rossz irányba állna)
			m_vb_szikla.AddData(2, 1 - u, v);
		}
	}
	// az indexek meghatározása minden négyzethez (N*M négyzetünk van)
	for (int i = 0; i < N; ++i) {
		for (int j = 0; j < M; ++j) {
			// az egyik háromszög a négyzet egyik fele
			m_vb_szikla.AddIndex((i)+(j)*		(N + 1),
				(i)+(j + 1) *	(N + 1),
				(i + 1) + (j) *		(N + 1)
			);
			// a másik háromszög a négyzet másik fele
			m_vb_szikla.AddIndex((i + 1) + (j) *		(N + 1),
				(i)+(j + 1) *	(N + 1),
				(i + 1) + (j + 1) *	(N + 1)
			);
		}
	}

	m_vb_szikla.InitBuffers();



	//
	// shaderek betöltése
	//
	m_program.AttachShader(GL_VERTEX_SHADER, "dirLight.vert");
	m_program.AttachShader(GL_FRAGMENT_SHADER, "dirLight.frag");

	m_program.BindAttribLoc(0, "vs_in_pos");
	m_program.BindAttribLoc(1, "vs_in_normal");
	m_program.BindAttribLoc(2, "vs_in_tex0");

	if ( !m_program.LinkProgram() )
	{
		return false;
	}

	//
	// egyéb inicializálás
	//

	m_camera.SetProj(105.0f, 640.0f/480.0f, 0.01f, 1000.0f);
	m_camera.SetView( glm::vec3(100, 100, 100), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

	// textúrák betöltése

	m_earthTextureID = TextureFromFile("earthmap1k.jpg");
	m_earthNormalMapID = TextureFromFile("earthmap1k_normal.jpg");

	m_moonTextureID = TextureFromFile("moonmap1k.jpg");
	m_moonNormalMapID = TextureFromFile("moonmap1k_normal.jpg");

	m_masikTextureID = TextureFromFile("gomb_texture.bmp");
	m_masikNormalMapID = TextureFromFile("gomb_normal.bmp");

	m_sziklaTextureID = TextureFromFile("szikla.jpg");
	m_sziklaNormalMapID = TextureFromFile("szikla_normal.jpg");


	m_sunTextureID = TextureFromFile("sunmap.jpg");

	return true;
}

void CMyApp::Clean()
{

	glDeleteTextures(1, &m_earthTextureID);
	glDeleteTextures(1, &m_earthNormalMapID);
	glDeleteTextures(1, &m_moonTextureID);
	glDeleteTextures(1, &m_moonNormalMapID);
	glDeleteTextures(1, &m_masikTextureID);
	glDeleteTextures(1, &m_masikNormalMapID);
	glDeleteTextures(1, &m_sziklaTextureID);
	glDeleteTextures(1, &m_sziklaNormalMapID);
	glDeleteTextures(1, &m_sunTextureID);

	m_program.Clean();
}

void CMyApp::Update()
{
	// kameramozgatáshoz
	static Uint32 last_time = SDL_GetTicks();
	float delta_time = (SDL_GetTicks() - last_time)/1000.0f;
	m_camera.Update(delta_time);
	last_time = SDL_GetTicks();
}


/*
void CMyApp::DrawSuzanne(){

	// kétszeresére nagyítjuk és eltoljuk
	glm::mat4 matWorld = glm::translate<float>(5, 5, 0) * glm::scale<float>(2, 2, 2);
	glm::mat4 matWorldIT = glm::transpose(glm::inverse(matWorld));
	glm::mat4 mvp = m_camera.GetViewProj() *matWorld;

	m_program.SetUniform("world", matWorld);
	m_program.SetUniform("worldIT", matWorldIT);
	m_program.SetUniform("MVP", mvp);

	m_program.SetTexture("texImage", 0, m_suzanneTextureID);
	m_program.SetTexture("texNormal", 1, m_suzanneNormalMapID);

	m_mesh->draw();

}
*/

void CMyApp::DrawMasikBolygo(glm::vec3 pos) {

	float rot = SDL_GetTicks() / 3000.0f * 360;
	pos = -pos;

	glm::mat4 matWorld = glm::translate<float>(pos) * glm::rotate<float>(rot, 0, 1, 0) * glm::scale<float>(MASIKNYUJT, MASIKNYUJT, MASIKNYUJT);
	glm::mat4 matWorldIT = glm::transpose(glm::inverse(matWorld));
	glm::mat4 mvp = m_camera.GetViewProj() *matWorld;

	m_program.SetUniform("world", matWorld);
	m_program.SetUniform("worldIT", matWorldIT);
	m_program.SetUniform("MVP", mvp);

	m_program.SetTexture("texImage", 0, m_masikTextureID);
	m_program.SetTexture("texNormal", 1, m_masikNormalMapID);

	m_vb_sphere.On();
	m_vb_sphere.DrawIndexed(GL_TRIANGLES, 0, 2 * 3 * N * M, 0);
	m_vb_sphere.Off();


	matWorld = glm::translate<float>(pos) * glm::scale<float>(-MASIKNYUJT-5, 1, -MASIKNYUJT-5);
	matWorldIT = glm::transpose(glm::inverse(matWorld));
	mvp = m_camera.GetViewProj() *matWorld;

	m_program.SetUniform("world", matWorld);
	m_program.SetUniform("worldIT", matWorldIT);
	m_program.SetUniform("MVP", mvp);

	m_vb_torusz.On();
	m_vb_torusz.DrawIndexed(GL_TRIANGLES, 0, 2 * 3 * N * M, 0);
	m_vb_torusz.Off();
}


void CMyApp::DrawEarth(glm::vec3 pos){

	float rot = SDL_GetTicks() / 3000.0f * 360;

	// világtranszformáció: forgatunk az y tengely körül rot szöggel
	glm::mat4 matWorld = glm::translate<float>(pos) * glm::rotate<float>(rot, 0, 1, 0) *glm::scale<float>(FOLDNYUJT, FOLDNYUJT, FOLDNYUJT);
	glm::mat4 matWorldIT = glm::transpose(glm::inverse(matWorld));
	glm::mat4 mvp = m_camera.GetViewProj() *matWorld;

	m_program.SetUniform("world", matWorld);
	m_program.SetUniform("worldIT", matWorldIT);
	m_program.SetUniform("MVP", mvp);

	m_program.SetTexture("texImage", 0, m_earthTextureID);
	m_program.SetTexture("texNormal", 1, m_earthNormalMapID);

	m_vb_sphere.On();
	m_vb_sphere.DrawIndexed(GL_TRIANGLES, 0, 2 * 3 * N * M, 0);
	m_vb_sphere.Off();



	float eI = 50.f;
	float eJ = 10.f;
	for (int i = 0; i < eI; i = i + 2) {
		for (int j = 0; j < eJ; j = j + 2) {
			float u = i / eI * -2 * f_PI;
			float v = j / eJ * f_PI;
			int rGen = (i + 1)*(j + 1) * 10000000;
			srand(rGen);
			float rndMelyseg = ((float)rand() / (RAND_MAX)) - 1.0f; //0 és 1 között -1 => -1 és 0
			float foldSugar = (float)FOLDNYUJT + rndMelyseg;
			//sugárhoz kell hozzáadni, mert csak

			srand((i + 1)*(j + 1) + rGen);
			if (((double)rand() / (RAND_MAX)) < 0.3) {
				continue;
			}


			//random szín meghatározása
			m_program.SetUniform("sziklaSzin", glm::vec4(1.0f-((float)rand() / (RAND_MAX)), 1+rndMelyseg, ((float)rand() / (RAND_MAX)), 1));


			//sziklának a mérete
			float sziklaatmeret = ((double)rand() / (RAND_MAX + 1) - 1.0f); //0 és 2 közt -1 => -1 és 1 között


			//melyik tengely körül és hány fokkal forgatom
			int sziklaforg = (int)rand() / 181;
			int forgX = 0, forgY = 0, forgZ = 0;
			if (sziklaforg < 70) {
				forgX = 1;
			}
			else if (sziklaforg < 120) {
				forgY = 1;
			}
			else {
				forgZ = 1;
			}

			matWorld = 
				glm::translate<float>(
					pos //föld aktuális pozíciója
					+ glm::vec3(foldSugar*cosf(u)*sinf(v), foldSugar*sinf(u), foldSugar*cosf(u)*cosf(v)) //föld felületére való eltolás
					)
				* glm::rotate<float>(sziklaforg, forgX, forgY, forgZ) //forgatás
				* glm::scale<float>(sziklaatmeret, sziklaatmeret, sziklaatmeret); //nagyítás
			glm::mat4 matWorldIT = glm::transpose(glm::inverse(matWorld));
			mvp = m_camera.GetViewProj() *matWorld;

			m_program.SetUniform("world", matWorld);
			m_program.SetUniform("worldIT", matWorldIT);
			m_program.SetUniform("MVP", mvp);

			m_program.SetTexture("texImage", 0, m_sziklaTextureID);
			m_program.SetTexture("texNormal", 1, m_sziklaNormalMapID);

			m_vb_szikla.On();
			m_vb_szikla.DrawIndexed(GL_TRIANGLES, 0, 2 * 3 * N * M, 0);
			m_vb_szikla.Off();
		}
	}

	m_program.SetUniform("sziklaSzin", glm::vec4(1,1,1,1));

}

void CMyApp::DrawMoon(glm::vec3 light_pos) {
	glm::mat4 matWorld = glm::translate<float>(light_pos) * glm::scale<float>(HOLDNYUJT, HOLDNYUJT, HOLDNYUJT);
	glm::mat4 matWorldIT = glm::transpose(glm::inverse(matWorld));

	m_program.SetUniform("world", matWorld);
	m_program.SetUniform("worldIT", matWorldIT);
	glm::mat4 mvp = m_camera.GetViewProj() *matWorld;
	m_program.SetUniform("MVP", mvp);

	m_program.SetTexture("texImage", 0, m_moonTextureID);
	m_program.SetTexture("texNormal", 1, m_moonNormalMapID);

	m_vb_sphere.On();
	m_vb_sphere.DrawIndexed(GL_TRIANGLES, 0, 2 * 3 * N * M, 0);
	m_vb_sphere.Off();

}

void CMyApp::DrawSun() {

	glm::vec3 pos;
	pos = glm::vec3(0, 0, 0);
	glm::mat4 matWorld = glm::translate<float>(pos) * glm::scale<float>(NAPNYUJT, NAPNYUJT, NAPNYUJT);
	glm::mat4 matWorldIT = glm::transpose(glm::inverse(matWorld));

	m_program.SetUniform("world", matWorld);
	m_program.SetUniform("worldIT", matWorldIT);
	glm::mat4 mvp = m_camera.GetViewProj()*matWorld;
	m_program.SetUniform("MVP", mvp);

	m_program.SetUniform("is_sun", true); // napot rajzolunk

	static glm::vec4 ujSzin = glm::vec4(0, 1, 1, 1);
	static glm::vec4 regiSzin = glm::vec4(1, 0, 1, 1);

	static int startTick = SDL_GetTicks();


	static float szin = -1.f;
	static bool oldal = true;
	if (szin <= -0.1f) {
		startTick = SDL_GetTicks();
		srand(SDL_GetTicks());

		regiSzin = ujSzin;
		double rnd = (double)rand() / (RAND_MAX);

		if (rnd < 0.3) {
			ujSzin = glm::vec4(1, 0, 0, 1);
		}
		else if (rnd < 0.6) {
			ujSzin = glm::vec4(0, 1, 0, 1);
		}
		else {
			ujSzin = glm::vec4(0, 0, 1, 1);
		}
		oldal = !oldal;

		
	}


	szin = sin((SDL_GetTicks()-startTick)/1000.f/5.f);

	m_program.SetUniform("fenySzin", regiSzin*((oldal?1-szin:szin))+ujSzin*((!oldal ? 1-szin : szin)));

	m_program.SetTexture("texImage", 0, m_sunTextureID);
	m_program.SetTexture("texNormal", 1, m_sunTextureID);

	m_vb_sphere.On();
	m_vb_sphere.DrawIndexed(GL_TRIANGLES, 0, 2 * 3 * N * M, 0);
	m_vb_sphere.Off();
	m_program.SetUniform("is_sun", false); // mostmár nem nap
			


	
}


void CMyApp::Render()
{
	// töröljük a frampuffert (GL_COLOR_BUFFER_BIT) és a mélységi Z puffert (GL_DEPTH_BUFFER_BIT)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// megjelenítés módja
	glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	m_program.On(); // SHADER BEKAPCS

	// light_pos: y = 10 magasságban kering az XZ síkon, 5 mp alatt megtéve egy 10 sugarú körpályát
	// (ugyanez lesz majd a "nap" eltolása is az õ világtranszformációjában)
	float r = 2 * f_PI * SDL_GetTicks() / 1000.0f / 5.0f;
	int sugar = 3;
	glm::vec3 earth_pos = glm::vec3((NAPNYUJT + FOLDNYUJT) * sugar * cosf(r), NAPNYUJT + FOLDNYUJT*sugar, (NAPNYUJT + FOLDNYUJT) * sugar * sinf(r));
	//a hold fénye és helye
	r = -2 * f_PI * SDL_GetTicks() / 1000.0f / 2.0f;
	sugar = 2;
	glm::vec3 light_pos = glm::vec3((HOLDNYUJT + FOLDNYUJT) * sugar * cosf(r), (HOLDNYUJT + FOLDNYUJT) * sugar, (HOLDNYUJT + FOLDNYUJT) * sugar * sinf(r)) + earth_pos;

	if (isCamV)
		m_camera.SetView(earth_pos+glm::vec3(30,30,30),earth_pos,glm::vec3(0,1,0));
	

	m_program.SetUniform("light_pos", light_pos);
	m_program.SetUniform("eye_pos", m_camera.GetEye());


	DrawSun();
	DrawMoon(light_pos);
	DrawEarth(earth_pos);
	DrawMasikBolygo(earth_pos);

	m_program.Off();

}

void CMyApp::KeyboardDown(SDL_KeyboardEvent& key)
{
	if(!isCamV)
		m_camera.KeyboardDown(key);
	switch (key.keysym.sym){
	case SDLK_v:
		isCamV = !isCamV;
		break;
	}
}

void CMyApp::KeyboardUp(SDL_KeyboardEvent& key)
{
	if (!isCamV)
		m_camera.KeyboardUp(key);
}

void CMyApp::MouseMove(SDL_MouseMotionEvent& mouse)
{
	if (!isCamV)
		m_camera.MouseMove(mouse);
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


void CMyApp::Resize(int _w, int _h)
{
	glViewport(0, 0, _w, _h);

	m_camera.Resize(_w, _h);
}