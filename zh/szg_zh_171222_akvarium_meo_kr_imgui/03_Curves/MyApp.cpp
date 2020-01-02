#include "MyApp.h"

#include <math.h>
#include <vector>

#include <array>
#include <list>
#include <tuple>

#include "imgui/imgui.h"

#include "ObjParser_OGL3.h"

CMyApp::CMyApp(void)
{
}


CMyApp::~CMyApp(void)
{
	
}

bool CMyApp::Init()
{
	// törlési szín legyen fehér
	glClearColor(1,1,1, 1);

	glEnable(GL_CULL_FACE); // kapcsoljuk be a hatrafele nezo lapok eldobasat
	glEnable(GL_DEPTH_TEST); // mélységi teszt bekapcsolása (takarás)
	//glDepthMask(GL_FALSE); // kikapcsolja a z-pufferbe történő írást - https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/glDepthMask.xml

	glLineWidth(4.0f); // a vonalprimitívek vastagsága: https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glLineWidth.xhtml
	//glEnable(GL_LINE_SMOOTH);

	// átlátszóság engedélyezése
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // meghatározza, hogy az átlátszó objektum az adott pixelben hogyan módosítsa a korábbi fragmentekből oda lerakott színt: https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glBlendFunc.xhtml

	// a raszterizált pontprimitívek mérete
	glPointSize(15.0f);

	//
	// shaderek betöltése
	//

	// így is létre lehetne hozni:

	// a shadereket tároló program létrehozása
	m_program.Init({ 
		{ GL_VERTEX_SHADER, "myVert.vert"},
		{ GL_FRAGMENT_SHADER, "myFrag.frag"}
	},
	{ 
		{ 0, "vs_in_pos" },					// VAO 0-as csatorna menjen a vs_in_pos-ba
		{ 1, "vs_in_normal" },				// VAO 1-es csatorna menjen a vs_in_normal-ba
		{ 2, "vs_out_tex0" },				// VAO 2-es csatorna menjen a vs_in_tex0-ba
	});

	m_program.LinkProgram();

	// tengelyeket kirajzoló shader rövid inicializálása
	m_axesProgram.Init({
		{GL_VERTEX_SHADER, "axes.vert"},
		{GL_FRAGMENT_SHADER, "axes.frag"}
	});
	// haladóknak, még rövidebb: 
	//m_axesProgram.Init({"axes.vert"_vs, "axes.frag"_fs});

	// kontrollpontokat kirajzoló program
	m_pointProgram.Init({
			{ GL_VERTEX_SHADER, "pointDrawer.vert" },
			{ GL_FRAGMENT_SHADER, "pointDrawer.frag" }
		}, 
		{
			{ 0, "vs_in_pos" },					// VAO 0-as csatorna menjen a vs_in_pos-ba
			{ 1, "vs_in_normal" },				// VAO 1-es csatorna menjen a vs_in_normal-ba
			{ 2, "vs_out_tex0" },				// VAO 2-es csatorna menjen a vs_in_tex0-ba
		});

	//
	// geometria definiálása (std::vector<...>) és GPU pufferekbe (m_buffer*) való feltöltése BufferData-val
	//
	initGround();
	initSphere();

	// textúra betöltése
	m_textureMetal.FromFile("water.jpg");
	m_textureWall.FromFile("wall.png");
	m_textureFish.FromFile("hal.png");

	// mesh betöltése
	m_mesh = ObjParser::parse("Suzanne.obj");

	// kamera
	m_camera.SetProj(45.0f, 640.0f / 480.0f, 0.01f, 1000.0f);
	m_camera.SetView(glm::vec3(15, 15, 15), glm::vec3(0, 0, 0), glm::vec3(0, 1,0));

 	std::random_device rd;
    std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(-20, 20);
	for (int i = 0; i < bubbleNum; ++i){
		glm::vec3 pos = glm::vec3(dis(gen), -15/2.0f, dis(gen));
		bubblePositions.push_back(pos);
	}
	
	

	return true;
}

glm::vec3	CMyApp::GetUV(float u, float v)
{
	u *= 2 * 3.1415f;
	v *= 3.1415f;
	float cu = cosf(u), su = sinf(u), cv = cosf(v), sv = sinf(v);
	float r = 1;

	return glm::vec3(r*cu*sv, r*cv, r*su*sv);
}

void CMyApp::initSphere() {

	IndexBuffer			m_gpuBufferIndices;		// indexek
	ArrayBuffer			m_gpuBufferPos;	// pozíciók tömbje
	ArrayBuffer			m_gpuBufferNormal;	// normálisok tömbje
	ArrayBuffer			m_gpuBufferTex;	// textúrakoordináták tömbje

	// NxM darab négyszöggel közelítjük a parametrikus felületünket => (N+1)x(M+1) pontban kell kiértékelni
	//Vertex vert[(N + 1)*(M + 1)];
	std::vector<glm::vec3> pos;
	std::vector<glm::vec3> normal;
	std::vector<glm::vec2> tex;

	pos.resize((N + 1)*(M + 1));
	normal.resize((N + 1)*(M + 1));
	tex.resize((N + 1)*(M + 1));

	for (int i = 0; i <= N; ++i)
		for (int j = 0; j <= M; ++j)
		{
			float u = i / (float)N;
			float v = j / (float)M;

			pos[i + j * (N + 1)] = GetUV(u, v);
			normal[i + j * (N + 1)] = glm::normalize(GetUV(u, v));
			tex[i + j * (N + 1)] = glm::vec2(u, v);
		}
	m_gpuBufferPos.BufferData(pos);
	m_gpuBufferNormal.BufferData(normal);
	m_gpuBufferTex.BufferData(tex);

	// indexpuffer adatai: NxM négyszög = 2xNxM háromszög = háromszöglista esetén 3x2xNxM index
	//GLushort indices[3 * 2 * (N)*(M)];
	std::vector<int> indices;
	indices.resize(N*M * 6);
	for (int i = 0; i < N; ++i)
		for (int j = 0; j < M; ++j)
		{
			indices[6 * i + j * 3 * 2 * (N)+0] = (i)+(j)*	(N + 1);
			indices[6 * i + j * 3 * 2 * (N)+1] = (i + 1) + (j)*	(N + 1);
			indices[6 * i + j * 3 * 2 * (N)+2] = (i)+(j + 1)*(N + 1);
			indices[6 * i + j * 3 * 2 * (N)+3] = (i + 1) + (j)*	(N + 1);
			indices[6 * i + j * 3 * 2 * (N)+4] = (i + 1) + (j + 1)*(N + 1);
			indices[6 * i + j * 3 * 2 * (N)+5] = (i)+(j + 1)*(N + 1);
		}

	m_gpuBufferIndices.BufferData(indices);

	// geometria VAO-ban való regisztrálása
	m_vaoSphere.Init(
		{
			{ CreateAttribute<0, glm::vec3, 0, sizeof(glm::vec3)>, m_gpuBufferPos },	// 0-ás attribútum "lényegében" glm::vec3-ak sorozata és az adatok az m_gpuBufferPos GPU pufferben vannak
			{ CreateAttribute<1, glm::vec3, 0, sizeof(glm::vec3)>, m_gpuBufferNormal },	// 1-es attribútum "lényegében" glm::vec3-ak sorozata és az adatok az m_gpuBufferNormal GPU pufferben vannak
			{ CreateAttribute<2, glm::vec2, 0, sizeof(glm::vec2)>, m_gpuBufferTex }		// 2-es attribútum "lényegében" glm::vec2-ők sorozata és az adatok az m_gpuBufferTex GPU pufferben vannak
		},
		m_gpuBufferIndices
	);
}

void CMyApp::initGround() {

	IndexBuffer			m_gpuBufferIndices;		// indexek
	ArrayBuffer			m_gpuBufferPos;	// pozíciók tömbje
	ArrayBuffer			m_gpuBufferNormal;	// normálisok tömbje
	ArrayBuffer			m_gpuBufferTex;	// textúrakoordináták tömbje

	m_gpuBufferPos.BufferData(
		std::vector<glm::vec3>{
		//		  X, Y, Z
		glm::vec3(-0.5, 0, -0.5),
			glm::vec3(-0.5, 0, 0.5),
			glm::vec3(0.5, 0, -0.5),
			glm::vec3(0.5, 0, 0.5)
	}
	);

	// normálisai
	m_gpuBufferNormal.BufferData(
		std::vector<glm::vec3>{
		// normal.X,.Y,.Z
		glm::vec3(0, 1, 0), // 0-ás csúcspont
			glm::vec3(0, 1, 0), // 1-es
			glm::vec3(0, 1, 0), // 2-es
			glm::vec3(0, 1, 0)  // 3-as
	}
	);

	// textúrakoordinátái
	m_gpuBufferTex.BufferData(
		std::vector<glm::vec2>{
		//        u, v
		glm::vec2(0, 0), // 0-ás csúcspont
			glm::vec2(1, 0), // 1-es
			glm::vec2(0, 1), // 2-es
			glm::vec2(1, 1)	 // 3-as
	}
	);

	// és a primitíveket alkotó csúcspontok indexei (az előző tömbökből) - triangle list-el való kirajzolásra felkészülve
	m_gpuBufferIndices.BufferData(
		std::vector<int>{
		0, 1, 2,	// 1. háromszög: 0-1-2 csúcspontokból
			2, 1, 3	// 2. háromszög: 2-1-3 csúcspontokból
	}
	);

	// geometria VAO-ban való regisztrálása
	m_vaoGround.Init(
		{
			{ CreateAttribute<0, glm::vec3, 0, sizeof(glm::vec3)>, m_gpuBufferPos },	// 0-ás attribútum "lényegében" glm::vec3-ak sorozata és az adatok az m_gpuBufferPos GPU pufferben vannak
			{ CreateAttribute<1, glm::vec3, 0, sizeof(glm::vec3)>, m_gpuBufferNormal },	// 1-es attribútum "lényegében" glm::vec3-ak sorozata és az adatok az m_gpuBufferNormal GPU pufferben vannak
			{ CreateAttribute<2, glm::vec2, 0, sizeof(glm::vec2)>, m_gpuBufferTex }		// 2-es attribútum "lényegében" glm::vec2-ők sorozata és az adatok az m_gpuBufferTex GPU pufferben vannak
		},
		m_gpuBufferIndices
	);
}



void CMyApp::Clean()
{
}

void CMyApp::Update()
{
	static Uint32 last_time = SDL_GetTicks();
	float delta_time = (SDL_GetTicks() - last_time) / 1000.0f;

	m_camera.Update(delta_time);
	
 	std::random_device rd;
    	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(-20, 20);
	
	//move bubbles upward
	glm::vec3 bubbleDir = delta_time * glm::vec3(0, 1, 0);
	for (auto& bubblePos : bubblePositions) {
		bubblePos += bubbleDir;
		
		if (bubblePos.y > 15.0/2) {
			bubblePos.x = dis(gen);
			bubblePos.y = -15/2.0f;
			bubblePos.z = dis(gen);
		}
	}

	last_time = SDL_GetTicks();
}


void CMyApp::renderAquarium() {

	glm::mat4 cubeWorld = glm::scale(glm::vec3(60, 15, 40)) * glm::scale(glm::vec3(-1, -1, -1));

	renderCube( cubeWorld, m_textureMetal);
}

/*
	Build cube from 6 squares
*/
void CMyApp::renderCube(const glm::mat4 &world, const Texture2D &tex) {

	m_program.Use();
	

	m_program.SetTexture("texImage", 0, tex);
	m_program.SetUniform("light_dir", lightDir);
	m_program.SetUniform("eye_pos", m_camera.GetEye());
	m_program.SetUniform("Ld", glm::vec4(lightColor.x, lightColor.y, lightColor.z, 1.0));

	m_vaoGround.Bind();
	
	//top
	glm::mat4 cubeWorld = world * glm::translate<float>(glm::vec3(0, 0.5, 0));
	m_program.SetUniform("world", cubeWorld);
	m_program.SetUniform("worldIT", glm::transpose(glm::inverse(cubeWorld)));
	m_program.SetUniform("MVP", m_camera.GetViewProj()*cubeWorld);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	
	//bottom
	cubeWorld = world * glm::translate<float>(glm::vec3(0, -0.5, 0)) * glm::rotate<float>(M_PI, glm::vec3(1, 0, 0));
	m_program.SetUniform("world", cubeWorld);
	m_program.SetUniform("worldIT", glm::transpose(glm::inverse(cubeWorld)));
	m_program.SetUniform("MVP", m_camera.GetViewProj()*cubeWorld);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	
	//transform 1 side face of cube
	glm::mat4 sideWorld = glm::translate<float>(glm::vec3(-0.5, 0, 0)) * glm::rotate<float>(M_PI/2, glm::vec3(0, 0, 1));
	
	for (int i = 0; i < 4; ++i) {
		cubeWorld = world * glm::rotate<float>(M_PI/2.0 * i, glm::vec3(0, 1, 0)) * sideWorld; //draw faces by rotation
		m_program.SetUniform("world", cubeWorld);
		m_program.SetUniform("worldIT", glm::transpose(glm::inverse(cubeWorld)));
		m_program.SetUniform("MVP", m_camera.GetViewProj()*cubeWorld);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);	
	}
		
	m_vaoGround.Unbind(); // nem feltétlen szükséges: a m_mesh->draw beállítja a saját VAO-ját

	m_program.Unuse();
}

void CMyApp::renderBubble(const glm::mat4 &world) {
	m_pointProgram.Use();

	glm::mat4 cubeWorld = world * glm::scale(glm::vec3(0.25, 0.25, 0.25));
	m_pointProgram.SetUniform("mvp", m_camera.GetViewProj()*cubeWorld);

	m_vaoSphere.Bind();
		glDrawElements(GL_TRIANGLES, 6 * N*M, GL_UNSIGNED_INT, 0);
	m_vaoSphere.Unbind(); // nem feltétlen szükséges: a m_mesh->draw beállítja a saját VAO-ját

	m_pointProgram.Unuse();
}

void CMyApp::renderGoldfish(const glm::mat4 &world) {
	m_program.Use();

	m_program.SetTexture("texImage", 0, m_textureFish);
	m_program.SetUniform("light_dir", lightDir);
	m_program.SetUniform("eye_pos", m_camera.GetEye());
	m_program.SetUniform("Ld", glm::vec4(lightColor.x, lightColor.y, lightColor.z, 1.0));

	glm::mat4 cubeWorld = 
		world * glm::scale(glm::vec3(3, 2, 1.5));
	m_program.SetUniform("world", cubeWorld);
	m_program.SetUniform("worldIT", glm::transpose(glm::inverse(cubeWorld)));
	m_program.SetUniform("MVP", m_camera.GetViewProj()*cubeWorld);

	m_vaoSphere.Bind();
		glDrawElements(GL_TRIANGLES, 6*N*M, GL_UNSIGNED_INT, 0);
	m_vaoSphere.Unbind(); // nem feltétlen szükséges: a m_mesh->draw beállítja a saját VAO-ját

	m_program.Unuse();
}

void CMyApp::renderPyramid(const glm::mat4 &world) {

	for (int i = 0; i < 4; ++i) {
		glm::mat4 cubeWorld = world *
			glm::translate<float>(glm::vec3(0, -15/2.0 + 1, 0))
			* glm::translate<float>(glm::vec3(0, i * 2, 0))
			* glm::scale(glm::vec3((8 - i * 2), 2, (8 - i * 2)));
		renderCube(cubeWorld, m_textureWall);
	}

}

void CMyApp::Render()
{
	// töröljük a frampuffert (GL_COLOR_BUFFER_BIT) és a mélységi Z puffert (GL_DEPTH_BUFFER_BIT)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// tengelyek
	m_axesProgram.Use();
	m_axesProgram.SetUniform("mvp", m_camera.GetViewProj()*glm::translate(glm::vec3(0,0.1f,0)));

	glDrawArrays(GL_LINES, 0, 6);
	
	renderAquarium();
	
	renderPyramid(); //pyramid in the middle
	
	for (int i = 0; i < 4; ++i) { //other pyramids
		glm::mat4 world = glm::rotate<float>(2 * M_PI / 4.0f * i, glm::vec3(0, 1, 0)) 
			* glm::translate<float>(glm::vec3(0, -15/4.0f, 14))
			* glm::rotate<float>(M_PI / 4.0f, glm::vec3(0, 1, 0))
			* glm::scale(glm::vec3(0.5, 0.5, 0.5));
		renderPyramid(world);
	}

	
	float t = SDL_GetTicks() % 6000 / 6000.0f; // [0-1]
	for (int i = 0; i < fishNum; ++i) {
		float t2 = SDL_GetTicks() % ((i+1)*1000) / ((i+1)*1000.0f);
		glm::mat4 world = 
			glm::rotate<float>(2 * M_PI * t, glm::vec3(0, 1, 0))
			* glm::rotate<float>(2 * M_PI / fishNum * i, glm::vec3(0, 1, 0))
			* glm::translate<float>(glm::vec3(0, -15/2.0 + 2 + sin(2*M_PI*t2), fishRadius));
		renderGoldfish(world);
	}

	for (int i = 0; i < bubblePositions.size(); ++i) {
		glm::mat4 world = glm::translate<float>(bubblePositions[i]);
		renderBubble(world);
	}
		
	//
	// UI
	//
	// A következő parancs megnyit egy ImGui tesztablakot és így látszik mit tud az ImGui.
	ImGui::ShowTestWindow();
	
	// A ImGui::ShowTestWindow implementációja az imgui_demo.cpp-ben található
	// Érdemes még az imgui.h-t böngészni, valamint az imgui.cpp elején a FAQ-ot elolvasni.
	// Rendes dokumentáció nincs, de a fentiek elegendőek kell legyenek.
	
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiSetCond_FirstUseEver);
	// csak akkor lépjünk be, hogy ha az ablak nincs csíkká lekicsinyítve...
	
	if (ImGui::Begin("Curve designer"))	
	{
		ImGui::SliderInt("FishNum", &fishNum, 1, 10);
		ImGui::SliderFloat("FishRadius", &fishRadius, 0.01, 20);
		ImGui::SliderFloat3("LightColor", &(lightColor[0]), 0, 1);
		ImGui::SliderFloat3("LightDir", &(lightDir[0]), -1, 1);
		
	}
	ImGui::End(); // ...de még ha le is volt, End()-et hívnunk kell
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
