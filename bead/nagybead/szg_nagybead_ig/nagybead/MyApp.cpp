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

// g�mb parametrikus le�r�sa: (u,v) -> (x,y,z)
glm::vec3 CMyApp::GetUV(int mit, float u, float v) {
	glm::vec3 ret;
	switch (mit) {
		//g�mb
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
		//t�rusz
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
			double rnd = ((float)rand() / (RAND_MAX + 2000))-1000.0f; //0 �s 2001 k�z�tti -1000
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


// norm�lvektorok sz�mol�sa
glm::vec3 CMyApp::GetUV_normals(int mit, float u, float v) {

	glm::vec3 du, dv; // u �s v szerinti parci�lis deriv�ltak
	glm::vec3 n; // a v�gs� norm�lvektor

	// az alakzat param�teres alakja az F(u,v) f�ggv�ny (l�sd: GetUV() elj�r�s)
	// Majdnem b�rmilyen (kell�en sima �s folytonos) param�teres fel�let norm�lvektor�t
	//   megkaphatjuk egy adott (u,v) pontban az al�bbi m�don:
	// n(u,v) = ( dF(u,v)/du x dF(u,v)/dv ) / | dF(u,v)/du x dF(u,v)/dv |

	// Azaz venni kell az F(u,v) vektor u �s v szerinti parci�lis deriv�ltj�t,
	// �s a kapott vektorok kereszt-szorzata (egy normaliz�l�s ut�n) lesz a norm�lvektor.
	// most m�r adottak a parci�lis deriv�ltak (du �s dv)
	// a kett� kereszt-szorzata adja meg a fel�leti norm�list


	switch (mit) {
		//g�mb
		case 0:
		{
			u *= -2 * f_PI;
			v *= f_PI;

			du = glm::vec3(cosf(u)*sinf(v), 0, -sinf(u)*sinf(v));
			dv = glm::vec3(sinf(u)*cosf(v), -sinf(v), cosf(u)*cosf(v));
			break;
		}
		//t�rusz
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
			double rnd = ((float)rand() / (RAND_MAX + 2000)) - 1000.0; //0 �s 2001 k�z�tti -1000
			u += rnd;
			v += rnd;

			du = glm::vec3(cosf(u)*sinf(v), 0, -sinf(u)*sinf(v) - sinf(SZIKLAMERET * 20 * u));
			dv = glm::vec3(sinf(u)*cosf(v) - sinf(SZIKLAMERET * 20 * v), -sinf(v), cosf(u)*cosf(v));
			break;
		}
	}

	// (�gyelj�nk az ir�ny�totts�gra!)
	n = glm::cross(dv, du);
	// normaliz�ljuk egys�ghossz�ra
	if (glm::length(n) > 0.0f) {
		n = glm::normalize(n);
	}

	return n;


}

bool CMyApp::Init()
{
	// t�rl�si sz�n legyen k�kes
	glClearColor(0.125f, 0.25f, 0.5f, 1.0f);

	glEnable(GL_CULL_FACE);		// kapcsoljuk be a hatrafele nezo lapok eldobasat
	glEnable(GL_DEPTH_TEST);	// m�lys�gi teszt bekapcsol�sa (takar�s)

	//
	// geometria letrehozasa
	//
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// G�MB
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	m_vb_sphere.AddAttribute(0, 3);
	m_vb_sphere.AddAttribute(1, 3);
	m_vb_sphere.AddAttribute(2, 2);

	// N*M-es r�cs bej�r�sa, a vertexek (x,y,z) koordin�t�j�t a GetUV f�ggv�nnyel kapjuk
	for (int i = 0; i <= N; ++i){
		for (int j = 0; j <= M; ++j){
			float u = i / (float)N;
			float v = j / (float)M;
			// poz�ci�: g�mbi koordin�t�k
			m_vb_sphere.AddData(0, GetUV(0, u, v));
			// szerencs�kre g�mb eset�n ugyanaz a norm�lvektor:
			// az orig�b�l a g�mbfelsz�n ir�ny�ba mutat� ir�nyvektor
			m_vb_sphere.AddData(1, GetUV(0, u, v));
			// a text�ra-koordin�t�k megyeznek az (u,v) pont koordin�t�j�val
			m_vb_sphere.AddData(2, 1-u, v);
		}
	}
	// az indexek meghat�roz�sa minden n�gyzethez (N*M n�gyzet�nk van)
	for (int i = 0; i < N; ++i){
		for (int j = 0; j < M; ++j){
			// az egyik h�romsz�g a n�gyzet egyik fele
			m_vb_sphere.AddIndex((i)		+ (j)*		(N + 1),
								 (i)		+ (j + 1) *	(N + 1),
								 (i + 1)	+ (j) *		(N + 1)
							 );
			// a m�sik h�romsz�g a n�gyzet m�sik fele
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

	// N*M-es r�cs bej�r�sa, a vertexek (x,y,z) koordin�t�j�t a GetUV f�ggv�nnyel kapjuk
	for (int i = 0; i <= N; ++i) {
		for (int j = 0; j <= M; ++j) {
			float u = i / (float)N;
			float v = j / (float)M;
			// poz�ci�k: a param�teres f�ggv�ny szerint
			m_vb_torusz.AddData(0, GetUV(1, u, v));
			// norm�lvektorok: a param�teres f�ggv�nyb�l sz�molva
			m_vb_torusz.AddData(1, GetUV_normals(1, u, v));
			// text�ra koordin�t�k
			// itt most 1-n�l nagyobb sz�mokat is megenged�nk: ekkor a text�ra minta ism�tl�dni fog
			m_vb_torusz.AddData(2, 4 * (1 - u), 4 * v); // a text�ra-koordin�t�kat m�s sorrendben kell bej�rni!
		}
	}


	// az indexek meghat�roz�sa minden n�gyzethez
	for (int i = 0; i < N; ++i) {
		for (int j = 0; j < M; ++j) {
			// az egyik h�romsz�g a n�gyzet egyik fele
			m_vb_torusz.AddIndex((i)+(j)*		(N + 1),
				(i)+(j + 1) *	(N + 1),
				(i + 1) + (j) *		(N + 1)
			);
			// a m�sik h�romsz�g a n�gyzet m�sik fele
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

	// N*M-es r�cs bej�r�sa, a vertexek (x,y,z) koordin�t�j�t a GetUV f�ggv�nnyel kapjuk
	for (int i = 0; i <= N; ++i) {
		for (int j = 0; j <= M; ++j) {
			float u = i / (float)N;
			float v = j / (float)M;
			// poz�ci�: g�mbi koordin�t�k
			m_vb_szikla.AddData(0, GetUV(2, u, v));
			// szerencs�kre g�mb eset�n ugyanaz a norm�lvektor:
			// az orig�b�l a g�mbfelsz�n ir�ny�ba mutat� ir�nyvektor
			// de ez csak a g�mbn�l van �gy! (L�sd fentebb a csonkak�p pal�st, ott m�sk�pp kellett sz�molnunk!)
			m_vb_szikla.AddData(1, GetUV(2, u, v));
			// a text�ra-koordin�t�k megyeznek az (u,v) pont koordin�t�j�val
			// (kiv�ve, hogy a k�p rossz ir�nyba �llna)
			m_vb_szikla.AddData(2, 1 - u, v);
		}
	}
	// az indexek meghat�roz�sa minden n�gyzethez (N*M n�gyzet�nk van)
	for (int i = 0; i < N; ++i) {
		for (int j = 0; j < M; ++j) {
			// az egyik h�romsz�g a n�gyzet egyik fele
			m_vb_szikla.AddIndex((i)+(j)*		(N + 1),
				(i)+(j + 1) *	(N + 1),
				(i + 1) + (j) *		(N + 1)
			);
			// a m�sik h�romsz�g a n�gyzet m�sik fele
			m_vb_szikla.AddIndex((i + 1) + (j) *		(N + 1),
				(i)+(j + 1) *	(N + 1),
				(i + 1) + (j + 1) *	(N + 1)
			);
		}
	}

	m_vb_szikla.InitBuffers();



	//
	// shaderek bet�lt�se
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
	// egy�b inicializ�l�s
	//

	m_camera.SetProj(105.0f, 640.0f/480.0f, 0.01f, 1000.0f);
	m_camera.SetView( glm::vec3(100, 100, 100), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

	// text�r�k bet�lt�se

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
	// kameramozgat�shoz
	static Uint32 last_time = SDL_GetTicks();
	float delta_time = (SDL_GetTicks() - last_time)/1000.0f;
	m_camera.Update(delta_time);
	last_time = SDL_GetTicks();
}


/*
void CMyApp::DrawSuzanne(){

	// k�tszeres�re nagy�tjuk �s eltoljuk
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

	// vil�gtranszform�ci�: forgatunk az y tengely k�r�l rot sz�ggel
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
			float rndMelyseg = ((float)rand() / (RAND_MAX)) - 1.0f; //0 �s 1 k�z�tt -1 => -1 �s 0
			float foldSugar = (float)FOLDNYUJT + rndMelyseg;
			//sug�rhoz kell hozz�adni, mert csak

			srand((i + 1)*(j + 1) + rGen);
			if (((double)rand() / (RAND_MAX)) < 0.3) {
				continue;
			}


			//random sz�n meghat�roz�sa
			m_program.SetUniform("sziklaSzin", glm::vec4(1.0f-((float)rand() / (RAND_MAX)), 1+rndMelyseg, ((float)rand() / (RAND_MAX)), 1));


			//szikl�nak a m�rete
			float sziklaatmeret = ((double)rand() / (RAND_MAX + 1) - 1.0f); //0 �s 2 k�zt -1 => -1 �s 1 k�z�tt


			//melyik tengely k�r�l �s h�ny fokkal forgatom
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
					pos //f�ld aktu�lis poz�ci�ja
					+ glm::vec3(foldSugar*cosf(u)*sinf(v), foldSugar*sinf(u), foldSugar*cosf(u)*cosf(v)) //f�ld fel�let�re val� eltol�s
					)
				* glm::rotate<float>(sziklaforg, forgX, forgY, forgZ) //forgat�s
				* glm::scale<float>(sziklaatmeret, sziklaatmeret, sziklaatmeret); //nagy�t�s
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
	m_program.SetUniform("is_sun", false); // mostm�r nem nap
			


	
}


void CMyApp::Render()
{
	// t�r�lj�k a frampuffert (GL_COLOR_BUFFER_BIT) �s a m�lys�gi Z puffert (GL_DEPTH_BUFFER_BIT)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// megjelen�t�s m�dja
	glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	m_program.On(); // SHADER BEKAPCS

	// light_pos: y = 10 magass�gban kering az XZ s�kon, 5 mp alatt megt�ve egy 10 sugar� k�rp�ly�t
	// (ugyanez lesz majd a "nap" eltol�sa is az � vil�gtranszform�ci�j�ban)
	float r = 2 * f_PI * SDL_GetTicks() / 1000.0f / 5.0f;
	int sugar = 3;
	glm::vec3 earth_pos = glm::vec3((NAPNYUJT + FOLDNYUJT) * sugar * cosf(r), NAPNYUJT + FOLDNYUJT*sugar, (NAPNYUJT + FOLDNYUJT) * sugar * sinf(r));
	//a hold f�nye �s helye
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