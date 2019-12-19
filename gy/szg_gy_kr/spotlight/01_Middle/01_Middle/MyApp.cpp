#include "MyApp.h"
#include "GLUtils.hpp"

#include <GL/GLU.h>
#include <math.h>

#include "ObjParser_OGL3.h"

CMyApp::CMyApp(void)
{
	m_textureID = 0;
	m_mesh = 0;
}


CMyApp::~CMyApp(void)
{
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

	// TALAJ

	m_vb_ground.AddAttribute(0, 3);
	m_vb_ground.AddAttribute(1, 3);
	m_vb_ground.AddAttribute(2, 2);

	m_vb_ground.AddData(0, -10,  0, -10);
	m_vb_ground.AddData(0,  10,  0, -10);
	m_vb_ground.AddData(0, -10,  0,  10);
	m_vb_ground.AddData(0,  10,  0,  10);

	m_vb_ground.AddData(1, 0, 1, 0);
	m_vb_ground.AddData(1, 0, 1, 0);
	m_vb_ground.AddData(1, 0, 1, 0);
	m_vb_ground.AddData(1, 0, 1, 0);

	m_vb_ground.AddData(2, 0, 0);
	m_vb_ground.AddData(2, 1, 0);
	m_vb_ground.AddData(2, 0, 1);
	m_vb_ground.AddData(2, 1, 1);

	m_vb_ground.AddIndex(1, 0, 2);
	m_vb_ground.AddIndex(1, 2, 3);

	m_vb_ground.InitBuffers();

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// KÚPPALÁST
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// a kúpot úgy hozzuk létre, hogy a csúcsa az origóban van
	// így majd amikor forgatjuk, a transzformációk révén a csúcsánál fogva fog forogni

	m_vb_cone.AddAttribute(0, 3);
	m_vb_cone.AddAttribute(1, 3);
	m_vb_cone.AddAttribute(2, 2);

	// nulladik vertex: az origóban
	m_vb_cone.AddData(0, 0, 0, 0);
	m_vb_cone.AddData(1, 0, 1, 0);
	m_vb_cone.AddData(2, 0.5, 0.5);

	// elsõ, második, ..., N-edik vertex
	// az N+1-edik vertexet ugyanoda rakjuk, ahova az elsõt
	for (int i = 0; i <= N; ++i){
		float a = i * 2 * 3.14153f / N;
		// pozíció: XZ sugarú körvonal mentén, Y = -0.25 magasságban
		m_vb_cone.AddData(0, 0.25 * cosf(a), -0.25, 0.25 * sinf(a));
		// nem ez a korrekt normálvektor!
		// de mivel shader szinten úgysem foglalkozunk a kúp megvilágításával, így mindegy is
		m_vb_cone.AddData(1, cosf(a), 0, sinf(a));
		// textúrázás: a textúra közepébõl kivágunk egy körlapot
		m_vb_cone.AddData(2, 0.5*cosf(a) + 0.5, 0.5*sinf(a) + 0.5);
	}

	// indices
	for (int i = 0; i < N; ++i){
		m_vb_cone.AddIndex(0, i + 2, i + 1);
	}

	m_vb_cone.InitBuffers();

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// KÚP ALJA
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	m_vb_cone_bottom.AddAttribute(0, 3);
	m_vb_cone_bottom.AddAttribute(1, 3);
	m_vb_cone_bottom.AddAttribute(2, 2);

	// nulladik vertex: az origóban
	m_vb_cone_bottom.AddData(0, 0, -0.25, 0);
	m_vb_cone_bottom.AddData(1, 0, 1, 0);
	m_vb_cone_bottom.AddData(2, 0.5, 0.5);

	// elsõ, második, ..., N-edik vertex
	// az N+1-edik vertexet ugyanoda rakjuk, ahova az elsõt
	for (int i = 0; i <= N; ++i){
		float a = i * 2 * 3.14153f / N;
		// pozíció: XZ sugarú körvonal mentén, Y = -0.25 magasságban
		m_vb_cone_bottom.AddData(0, 0.25 * cosf(-a), -0.25, 0.25 * sinf(-a));
		// normálvektorok (bár itt is mindegy)
		m_vb_cone_bottom.AddData(1, 0, -1, 0);
		// textúrázás: a textúra közepébõl kivágunk egy körlapot
		m_vb_cone_bottom.AddData(2, 0.5*cosf(a) + 0.5, 0.5*sinf(a) + 0.5);
	}

	// indices
	for (int i = 0; i < N; ++i){
		m_vb_cone_bottom.AddIndex(0, i + 2, i + 1);
	}

	m_vb_cone_bottom.InitBuffers();


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

	m_camera.SetProj(45.0f, 640.0f/480.0f, 0.01f, 1000.0f);

	// textúra betöltése
	m_textureID = TextureFromFile("texture.png");

	// mesh betöltés
	m_mesh = ObjParser::parse("Stargate.obj");
	m_mesh->initBuffers();

	return true;
}

void CMyApp::Clean()
{
	glDeleteTextures(1, &m_textureID);

	m_program.Clean();
}

void CMyApp::Update()
{
	static Uint32 last_time = SDL_GetTicks();
	float delta_time = (SDL_GetTicks() - last_time)/1000.0f;

	m_camera.Update(delta_time);

	last_time = SDL_GetTicks();
}


void CMyApp::Render()
{
	// töröljük a frampuffert (GL_COLOR_BUFFER_BIT) és a mélységi Z puffert (GL_DEPTH_BUFFER_BIT)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// **********************************
	// A MEGVILÁGÍTÁS BEÁLLÍTÁSAI
	// **********************************

	m_program.On();

	// akarunk-e fényeket
	m_program.SetUniform("light_switch", light_switch);

	// milyen irányba álljon a spotlámpa? A lightDir-t homogenizáljuk,
	// majd u és v szerinti forgatást alkalmazunk rajta
	// (Z és X tengelyek mentén mennyivel forgassunk)
	glm::vec4 lightDirCurrent =
		glm::rotate<float>(v, glm::vec3(1, 0, 0)) *
		glm::rotate<float>(u, glm::vec3(0, 0, 1)) *
		glm::vec4(lightDir, 0);

	// átadjuk a shadernek a pozíciót és az irányt
	m_program.SetUniform("light_pos", light_x, light_y, light_z);
	m_program.SetUniform("lightDir", lightDirCurrent.x, lightDirCurrent.y, lightDirCurrent.z);
	m_program.SetUniform("is_sharp", is_sharp);

	m_program.Off();

	// **********************************
	// KÚP RAJZOLÁS
	// **********************************

	m_program.On();

	// a kúpot ugyanúgy forgatjuk, ahogy a spotlámpa irányát, és ugyanoda is toljuk el
	glm::mat4 matWorld =
		glm::translate<float>(glm::vec3(light_x, light_y, light_z)) *
		glm::rotate<float>(v, glm::vec3(1, 0, 0)) *
		glm::rotate<float>(u, glm::vec3(0, 0, 1));
	glm::mat4 matWorldIT = glm::transpose(glm::inverse(matWorld));
	glm::mat4 mvp = m_camera.GetViewProj() *matWorld;

	m_program.SetUniform("world", matWorld);
	m_program.SetUniform("worldIT", matWorldIT);
	m_program.SetUniform("MVP", mvp);
	m_program.SetUniform("eye_pos", m_camera.GetEye());

	m_program.SetTexture("texImage", 0, m_textureID);

	// értesítjük a shader-t, hogy a kúpra ne számoljon fényeket (lásd shader kód)
	m_program.SetUniform("this_is_cone", true);
	// a kúp két részbõl tevõdik össze: palást...
	m_vb_cone.On();
	m_vb_cone.DrawIndexed(GL_TRIANGLES, 0, 3 * N, 0);
	m_vb_cone.Off();
	// ...és alsó lap
	m_vb_cone_bottom.On();
	m_vb_cone_bottom.DrawIndexed(GL_TRIANGLES, 0, 3 * N, 0);
	m_vb_cone_bottom.Off();
	m_program.SetUniform("this_is_cone", false);
	// ezután a rész után ismét számoljon fényeket

	// shader kikapcsolasa
	m_program.Off();

	// **********************************
	// TALAJ RAJZOLÁS
	// **********************************

	m_program.On();

	matWorld = glm::mat4(1.0f);
	matWorldIT = glm::transpose( glm::inverse( matWorld ) );
	mvp = m_camera.GetViewProj() *matWorld;

	// a talajnak más anyagjellemzõket szeretnénk, mint a mesh esetében,
	// ezért a shaderben átírjuk a megfelelõ uniform változókat
	m_program.SetUniform("Ka", 1, 1, 1, 1);
	m_program.SetUniform("Kd", 0.3, 1, 0.3, 1);
	m_program.SetUniform("Ks", 0, 0.5, 1, 1);

	m_program.SetUniform( "world", matWorld );
	m_program.SetUniform( "worldIT", matWorldIT );
	m_program.SetUniform( "MVP", mvp );
	m_program.SetUniform( "eye_pos", m_camera.GetEye() );

	m_program.SetTexture("texImage", 0, m_textureID);

	// kapcsoljuk be a VAO-t (a VBO jön vele együtt)
	m_vb_ground.On();
	m_vb_ground.DrawIndexed(GL_TRIANGLES, 0, 6, 0);
	m_vb_ground.Off();

	// shader kikapcsolasa
	m_program.Off();

	// **********************************
	// MESH RAJZOLÁS
	// **********************************

	m_program.On();

	matWorld =
		glm::translate<float>(glm::vec3(0, 0, 3)) *
		glm::scale<float>(glm::vec3(0.1, 0.1, 0.1));
	matWorldIT = glm::transpose( glm::inverse( matWorld ) );
	mvp = m_camera.GetViewProj() *matWorld;

	// a mesh kirajzolásánál más anyagjellemzõket szeretnénk
	m_program.SetUniform("Ka", 1, 1, 1, 1);
	m_program.SetUniform("Kd", 1, 0.2, 0.2, 1);
	m_program.SetUniform("Ks", 1, 0.5, 1, 1);

	m_program.SetUniform( "world", matWorld );
	m_program.SetUniform( "worldIT", matWorldIT );
	m_program.SetUniform( "MVP", mvp );
	m_program.SetUniform( "eye_pos", m_camera.GetEye() );

	m_program.SetTexture("texImage", 0, m_textureID);

	m_mesh->draw();

	m_program.Off();
}

void CMyApp::KeyboardDown(SDL_KeyboardEvent& key)
{
	m_camera.KeyboardDown(key);

	switch (key.keysym.sym){
	case SDLK_q:
		is_sharp = !is_sharp;
		break;
	case SDLK_UP:
		light_z -= 0.5f;
		break;
	case SDLK_DOWN:
		light_z += 0.5f;
		break;
	case SDLK_LEFT:
		light_x -= 0.5f;
		break;
	case SDLK_RIGHT:
		light_x += 0.5f;
		break;
	case SDLK_f:
		light_y -= 0.5f;
		break;
	case SDLK_r:
		light_y += 0.5f;
		break;
	case SDLK_j:
		u -= 1.0f;
		break;
	case SDLK_l:
		u += 1.0f;
		break;
	case SDLK_i:
		v += 1.0f;
		break;
	case SDLK_k:
		v -= 1.0f;
		break;
	}
}

void CMyApp::KeyboardUp(SDL_KeyboardEvent& key)
{
	m_camera.KeyboardUp(key);
}

void CMyApp::MouseMove(SDL_MouseMotionEvent& mouse)
{
	m_camera.MouseMove(mouse);
}

void CMyApp::MouseDown(SDL_MouseButtonEvent& mouse)
{
	if (mouse.button == SDL_BUTTON_RIGHT){
		light_switch = !light_switch;
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

	m_camera.Resize(_w, _h);
}