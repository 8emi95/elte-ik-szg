#include "MyApp.h"

#include <math.h>
#include <vector>

#include <array>
#include <list>
#include <tuple>
#include <imgui/imgui.h>
#include "includes/GLUtils.hpp"

CMyApp::CMyApp(void)
{
	m_camera.SetView(glm::vec3(5, 5, 5), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	akvarium = nullptr;
	hal = nullptr;
}

CMyApp::~CMyApp(void)
{
	std::cout << "dtor!\n";
}

void CMyApp::initViz() {
	viz = new Mesh();
	float xt = m / (float)xsd;
	float zt = h / (float)zsd;

	for (int i = 0; i < xsd + 1; i++) {
		for (int j = 0; j < zsd + 1; j++) {
			viz->addVertex({ glm::vec3(i * xt, m, -j * zt), glm::vec3(0,1,0), glm::vec2(-1 + i * 2.0 / (float)xsd, -1 + j * 2.0 / (float)zsd)  });
		}
	}
	for (int i = 0; i < xsd + 1 ; i++) {
		for (int j = 0; j < zsd; j++) {
			viz->addIndex(i * zsd + j);
			viz->addIndex((i + 1) * zsd + j + 1);
			viz->addIndex(i * zsd + j + 1);
			viz->addIndex(i * zsd + j);
			viz->addIndex((i+1) * zsd + j);
			viz->addIndex((i + 1) * zsd + j + 1);
		}
	}
	viz->initBuffers();
}

void CMyApp::initAkvarium() {

	waterTexture.FromFile("assets/water.png");

	akvarium = new Mesh();

	//front									 
	akvarium->addVertex({ glm::vec3(0, 0, 0), glm::vec3(0, 0, 1), glm::vec2(-1, -1)	});
	akvarium->addVertex({ glm::vec3(0, m, 0), glm::vec3(0, 0, 1), glm::vec2(-1, 1)	});
	akvarium->addVertex({ glm::vec3(m, m, 0), glm::vec3(0, 0, 1), glm::vec2(1, 1)	});
	akvarium->addVertex({ glm::vec3(m, 0, 0), glm::vec3(0, 0, 1), glm::vec2(1, -1)	});
	//back
	akvarium->addVertex({ glm::vec3(0, 0, -h), glm::vec3(0, 0, -1), glm::vec2(-1, -1) });
	akvarium->addVertex({ glm::vec3(0, m, -h), glm::vec3(0, 0, -1), glm::vec2(-1, 1) });
	akvarium->addVertex({ glm::vec3(m, m, -h), glm::vec3(0, 0, -1), glm::vec2(1, 1) });
	akvarium->addVertex({ glm::vec3(m, 0, -h), glm::vec3(0, 0, -1), glm::vec2(1, -1) });
	//right									 
	akvarium->addVertex({ glm::vec3(m, 0, 0), glm::vec3(1, 0, 0),	glm::vec2(-1, -1) });
	akvarium->addVertex({ glm::vec3(m, m, 0), glm::vec3(1, 0, 0),	glm::vec2(-1, 1) });
	akvarium->addVertex({ glm::vec3(m, m, -h), glm::vec3(1, 0, 0),	glm::vec2(1, 1) });
	akvarium->addVertex({ glm::vec3(m, 0, -h), glm::vec3(1, 0, 0),	glm::vec2(1, -1) });
	//left									 
	akvarium->addVertex({ glm::vec3(0, 0, -h), glm::vec3(-1, 0, 0), glm::vec2(1, -1) });
	akvarium->addVertex({ glm::vec3(0, m, -h), glm::vec3(-1, 0, 0), glm::vec2(1, 1) });
	akvarium->addVertex({ glm::vec3(0, m, 0),  glm::vec3(-1, 0, 0), glm::vec2(-1, 1) });
	akvarium->addVertex({ glm::vec3(0, 0, 0),  glm::vec3(-1, 0, 0), glm::vec2(-1, -1) });
	//bottom								 
	akvarium->addVertex({ glm::vec3(0, 0, 0), glm::vec3(0, -1, 0), glm::vec2(-1, -1) });
	akvarium->addVertex({ glm::vec3(0, 0, -h), glm::vec3(0, -1, 0), glm::vec2(-1, 1) });
	akvarium->addVertex({ glm::vec3(m, 0, -h), glm::vec3(0, -1, 0), glm::vec2(1, 1) });
	akvarium->addVertex({ glm::vec3(m, 0, 0), glm::vec3(0, -1, 0), glm::vec2(1, -1) });

	int ind[] = { 1,0,3, 1,3,2, 6,7,4, 6,4,5, 9,8,11, 9,11,10, 13,12,15, 13,15,14, 17,19,16, 19,17,18 };
	for (int i = 0; i < 30; i++) {
		akvarium->addIndex(ind[i]);
	}

	akvarium->initBuffers();

	initViz();
}

void CMyApp::InitShaders()
{
	// a shadereket tároló program létrehozása az OpenGL-hez hasonló módon:
	m_program.AttachShaders({
		{ GL_VERTEX_SHADER, "myVert.vert"},
		{ GL_FRAGMENT_SHADER, "myFrag.frag"}
	});

	// attributomok osszerendelese a VAO es shader kozt
	m_program.BindAttribLocations({
		{ 0, "vs_in_pos" },				// VAO 0-as csatorna menjen a vs_in_pos-ba
		{ 1, "vs_in_norm" },			// VAO 1-es csatorna menjen a vs_in_norm-ba
		{ 2, "vs_in_tex" },				// VAO 2-es csatorna menjen a vs_in_tex-be
	});

	m_program.LinkProgram();

	//water
	water_program.AttachShaders({
		{ GL_VERTEX_SHADER, "water.vert"},
		{ GL_FRAGMENT_SHADER, "water.frag"}
		});

	water_program.BindAttribLocations({
		{ 0, "vs_in_pos" },				// VAO 0-as csatorna menjen a vs_in_pos-ba
		{ 1, "vs_in_norm" },			// VAO 1-es csatorna menjen a vs_in_norm-ba
		{ 2, "vs_in_tex" },				// VAO 2-es csatorna menjen a vs_in_tex-be
	});

	water_program.LinkProgram();
}

bool CMyApp::Init()
{
	// törlési szín legyen kékes
	glClearColor(0, 0, 0, 1.0f);

	glEnable(GL_CULL_FACE); // kapcsoljuk be a hatrafele nezo lapok eldobasat
	glEnable(GL_DEPTH_TEST); // mélységi teszt bekapcsolása (takarás)

	// átlátszóság engedélyezése
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	InitShaders();
	initAkvarium();

	hal = ObjParser::parse("assets/hal.obj", true);
	hal->initBuffers();
	halTexture.FromFile("assets/hal.jpg");

	// egyéb textúrák betöltése

	korall = ObjParser::parse("assets/korall.obj", true);
	korall->initBuffers();
	korallTexture.FromFile("assets/korall.jpg");
	
	// kamera
	m_camera.SetProj(45.0f, 640.0f / 480.0f, 0.01f, 1000.0f);

	return true;
}

void CMyApp::Clean()
{
	delete akvarium;
	delete viz;
	delete hal;
	delete korall;
}

void CMyApp::Update()
{
	static Uint32 last_time = SDL_GetTicks();
	float delta_time = (SDL_GetTicks() - last_time) / 1000.0f;

	m_camera.Update(delta_time);

	last_time = SDL_GetTicks();
}

void CMyApp::Render()
{
	// töröljük a frampuffert (GL_COLOR_BUFFER_BIT) és a mélységi Z puffert (GL_DEPTH_BUFFER_BIT)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 viewProj = m_camera.GetViewProj();

	//Suzanne
	glm::mat4 akvariumWorld = glm::mat4(1.0f);
	glm::mat4 halWorld = glm::mat4(1.0f);
	glm::mat4 korallWorld = glm::mat4(1.0f);
	m_program.Use();

	korallWorld = korallWorld
		* glm::translate<float>(glm::vec3(1, 0, -1))
		* glm::rotate<float>(3 * M_PI/ 2.0f, glm::vec3(1, 0, 0))
		* glm::scale<float>(glm::vec3(0.02));

	m_program.SetUniform("MVP", viewProj * korallWorld);
	m_program.SetUniform("world", korallWorld);
	m_program.SetUniform("worldIT", glm::inverse(glm::transpose(korallWorld)));
	m_program.SetTexture("texImage", 0, korallTexture);
	korall->draw();

	halWorld = halWorld * glm::rotate<float>(3 * M_PI / 2.0f, glm::vec3(1, 0, 0))
		* glm::translate<float>(glm::vec3(m/2, h/2, m/2))
		* glm::scale<float>(glm::vec3(0.1))
		* glm::rotate<float>(3 * M_PI / 2.0f, glm::vec3(0, 0, 1));
	m_program.SetUniform("MVP", viewProj * halWorld);
	m_program.SetUniform("world", halWorld);
	m_program.SetUniform("worldIT", glm::inverse(glm::transpose(halWorld)));
	m_program.SetTexture("texImage", 0, halTexture);
	hal->draw();

	m_program.SetUniform("MVP", viewProj * akvariumWorld);
	m_program.SetUniform("world", akvariumWorld);
	m_program.SetUniform("worldIT", glm::inverse(glm::transpose(akvariumWorld)));
	m_program.SetTexture("texImage", 0, waterTexture);
	akvarium->draw();
	m_program.Unuse();

	water_program.Use();
	water_program.SetUniform("MVP", viewProj * akvariumWorld);
	water_program.SetUniform("world", akvariumWorld);
	water_program.SetUniform("worldIT", glm::inverse(glm::transpose(akvariumWorld)));
	water_program.SetTexture("texImage", 0, waterTexture);
	float time = (float)SDL_GetTicks();
	water_program.SetUniform("time", time);
	water_program.SetUniform("m", m);
	water_program.SetUniform("h", h);
	viz->draw();
	water_program.Unuse();

	GLint prevDepthFnc;
	glGetIntegerv(GL_DEPTH_FUNC, &prevDepthFnc);

	// most kisebb-egyenlõt használjunk, mert mindent kitolunk a távoli vágósíkokra
	glDepthFunc(GL_LEQUAL);

	
	// cube map textúra beállítása 0-ás mintavételezõre és annak a shaderre beállítása
	glActiveTexture(GL_TEXTURE0);

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);

	// végül állítsuk vissza
	glDepthFunc(prevDepthFnc);

	//ImGui Testwindow
	//ImGui::ShowTestWindow();
}

void CMyApp::KeyboardDown(SDL_KeyboardEvent& key)
{
	m_camera.KeyboardDown(key);
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
	glViewport(0, 0, _w, _h );

	m_camera.Resize(_w, _h);
}
