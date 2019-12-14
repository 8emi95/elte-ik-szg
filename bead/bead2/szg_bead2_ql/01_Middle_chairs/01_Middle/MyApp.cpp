#include "MyApp.h"
#include "GLUtils.hpp"

#include <GL/GLU.h>
#include <math.h>

#include "ObjParser_OGL3.h"

CMyApp::CMyApp(void)
{
	m_textureID = 0;
	m_textureID2 = 0;
	bigChair = 0;
	forward = 1;
	right = -1;
	up = 12;
	smallChair = 0;
	bigChair = 0;
	on = false;
}


CMyApp::~CMyApp(void)
{
}


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

bool CMyApp::Init()
{
	// törlési szín legyen kékes
	glClearColor(0.125f, 0.25f, 0.5f, 1.0f);

	glEnable(GL_CULL_FACE);		// kapcsoljuk be a hatrafele nezo lapok eldobasat
	glEnable(GL_DEPTH_TEST);	// mélységi teszt bekapcsolása (takarás)

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
	m_textureID = TextureFromFile("t.jpg");
	m_textureID2 = TextureFromFile("texture.png");

	// mesh betöltés
	bigChair = ObjParser::parse("a.obj");
	bigChair->initBuffers();

	smallChair = ObjParser::parse("b.obj");
	smallChair->initBuffers();

	return true;
}

void CMyApp::Clean()
{
	glDeleteTextures(1, &m_textureID);
	glDeleteTextures(1, &m_textureID2);

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

	m_program.On();

	glm::mat4 matWorld = glm::mat4(1.0f);
	glm::mat4 matWorldIT = glm::transpose( glm::inverse( matWorld ) );
	glm::mat4 mvp = m_camera.GetViewProj() *matWorld;

	m_program.SetUniform( "world", matWorld );
	m_program.SetUniform( "worldIT", matWorldIT );
	m_program.SetUniform( "MVP", mvp );
	m_program.SetUniform( "eye_pos", m_camera.GetEye() );
	m_program.SetUniform("right", right);
	m_program.SetUniform("forward", forward);
	m_program.SetUniform("on", on);
	m_program.SetUniform("up", up);

	m_program.SetTexture("texImage", 0, m_textureID);
	m_program.SetTexture("texImage2", 1, m_textureID2);

	// kapcsoljuk be a VAO-t (a VBO jön vele együtt)

	// shader kikapcsolasa
	m_program.Off();

	// 2. program
	m_program.On();

	matWorldIT = glm::transpose( glm::inverse( matWorld ) );
	mvp = m_camera.GetViewProj() *matWorld;

	m_program.SetUniform( "world", matWorld );
	m_program.SetUniform( "worldIT", matWorldIT );
	m_program.SetUniform( "MVP", mvp );
	m_program.SetUniform( "eye_pos", m_camera.GetEye() );
	m_program.SetUniform("forward", forward);
	m_program.SetUniform("right", right);
	m_program.SetUniform("on", on);
	m_program.SetUniform("up", up);
	m_program.SetUniform("Kd", glm::vec3(93 / 255.0, 37 / 255.0, 109 / 255.0) );

	m_program.SetTexture("texImage", 0, m_textureID);
	m_program.SetTexture("texImage2", 1, m_textureID2);
	m_program.SetUniform("t", (sinf((SDL_GetTicks() - start) / 11000.0f * 2 * M_PI) + 1 / 2));

	for (int i = 0; i < 5; ++i)
	{
		float a = ((i % 5 == 0 || i % 5 == 2 || i % 5 == 5) ? 1 : -1) * i;
		float b = (i % 2 == 0 ? -1: 1) * (5 - i * 0.85);
		float c = (i % 3 == 0 ? -1 : 1) * sqrt(pow(5, 2) - pow(a, 2) - pow(b, 2));
		matWorld = glm::translate(glm::vec3(a, b, c))
			*glm::rotate<float>(-M_PI / 2, glm::vec3(1, 0, 0));
		matWorldIT = glm::transpose(glm::inverse(matWorld));
		mvp = m_camera.GetViewProj() *matWorld;

		m_program.SetUniform("world", matWorld);
		m_program.SetUniform("worldIT", matWorldIT);
		m_program.SetUniform("MVP", mvp);
		m_program.SetUniform("eye_pos", m_camera.GetEye());
		m_program.SetUniform("forward", forward);
		m_program.SetUniform("right", right);
		m_program.SetUniform("up", up);
		m_program.SetUniform("on", on);
		m_program.SetTexture("texImage", 0, m_textureID);
		m_program.SetTexture("texImage2", 1, m_textureID2);

		if (i % 2 == 0)
		{
			bigChair->draw();
		}
		else
		{
			smallChair->draw();
		}
	}

	m_program.Off();
}

void CMyApp::KeyboardDown(SDL_KeyboardEvent& key)
{
	m_camera.KeyboardDown(key);
	switch (key.keysym.sym)
	{
	case SDLK_2:
		on = !on;
		start = SDL_GetTicks();
		break;
	case SDLK_UP:
		forward -= 0.5;
		break;
	case SDLK_DOWN:
		forward += 0.5;
		break;
	case SDLK_RIGHT:
		right += 0.5;
		break;
	case SDLK_LEFT:
		right -= 0.5;
		break;
	case SDLK_0:
		up += 0.5;
		break;
	case SDLK_1:
		up -= 0.5;
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