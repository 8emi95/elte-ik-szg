#include "MyApp.h"

#include <math.h>
#include <vector>

#include <array>
#include <list>
#include <tuple>

#include "imgui\imgui.h"

#include "ObjParser_OGL3.h"

CMyApp::CMyApp(void)
{
}


CMyApp::~CMyApp(void)
{
	
}

glm::vec3 CMyApp::GetTalajPos(float u, float v)
{
	return glm::vec3(20 * u - 10, 0, 20 * v - 10);
}

glm::vec3 CMyApp::GetTalajNorm(float u, float v)
{
	// Numerikusan (nem kell ismerni a képletet, elég a pozícióét)
	glm::vec3 uv = GetTalajPos(u, v);
	glm::vec3 du = GetTalajPos(u + 0.01, v) - GetTalajPos(u - 0.01, v);
	glm::vec3 dv = GetTalajPos(u, v + 0.01) - GetTalajPos(u, v - 0.01);

	return (glm::cross(du, dv));
}

glm::vec2 CMyApp::GetTalajTex(float u, float v)
{
	return glm::vec2(u, 1 - v);
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

	m_talajProgram.Init({
		{ GL_VERTEX_SHADER, "talaj.vert" },
		{ GL_FRAGMENT_SHADER, "talaj.frag" }
		},
	{
		{ 0, "vs_in_pos" },					// VAO 0-as csatorna menjen a vs_in_pos-ba
		{ 1, "vs_in_normal" },				// VAO 1-es csatorna menjen a vs_in_normal-ba
		{ 2, "vs_out_tex0" },				// VAO 2-es csatorna menjen a vs_in_tex0-ba
	});

	m_talajProgram.LinkProgram();
	//--------------------------------------------------------------------------------------------------
	m_mozdonyProgram.Init({
		{ GL_VERTEX_SHADER, "mozdony.vert" },
		{ GL_FRAGMENT_SHADER, "mozdony.frag" }
		},
	{
		{ 0, "vs_in_pos" },					// VAO 0-as csatorna menjen a vs_in_pos-ba
		{ 1, "vs_in_normal" },				// VAO 1-es csatorna menjen a vs_in_normal-ba
	});

	m_mozdonyProgram.LinkProgram();

	//-----------------------------------------------------------------------------------------------------
	m_vagonProgram.Init({
		{ GL_VERTEX_SHADER, "vagon.vert" },
		{ GL_FRAGMENT_SHADER, "vagon.frag" }
		},
	{
		{ 0, "vs_in_pos" },					// VAO 0-as csatorna menjen a vs_in_pos-ba
		{ 1, "vs_in_normal" },				// VAO 1-es csatorna menjen a vs_in_normal-ba
	});

	m_vagonProgram.LinkProgram();

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
	});

	//
	// geometria definiálása (std::vector<...>) és GPU pufferekbe (m_buffer*) való feltöltése BufferData-val
	//

	// vertexek pozíciói:
	/*
	Az m_gpuBufferPos konstruktora már létrehozott egy GPU puffer azonosítót és a most következő BufferData hívás ezt 
		1. bind-olni fogja GL_ARRAY_BUFFER target-re (hiszen m_gpuBufferPos típusa ArrayBuffer) és
		2. glBufferData segítségével áttölti a GPU-ra az argumentumban adott tároló értékeit

	*/
	std::vector<glm::vec3> talajPos;
	std::vector<glm::vec3> talajNorm;
	std::vector<glm::vec2> talajText;
	std::vector<int> talajInd;
	//Vertex vert[(N + 1)*(M + 1)];
	for (int i = 0; i <= N; ++i)
		for (int j = 0; j <= M; ++j)
		{
			float u = i / (float)N;
			float v = j / (float)M;

			talajPos.push_back(GetTalajPos(u, v));
			talajNorm.push_back(GetTalajNorm(u, v));
			talajText.push_back(GetTalajTex(u, v));
		}
	
	//GLushort indices[3 * 2 * (N)*(M)];
	for (int i = 0; i < N; ++i)
		for (int j = 0; j < M; ++j)
		{
			// minden négyszögre csináljunk kettő háromszöget, amelyek a következő 
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
			talajInd.push_back((i)+(j)*	(N + 1));
			talajInd.push_back((i + 1) + (j)*	(N + 1));
			talajInd.push_back((i)+(j + 1)*(N + 1));
			talajInd.push_back((i + 1) + (j)*	(N + 1));
			talajInd.push_back((i + 1) + (j + 1)*(N + 1));
			talajInd.push_back((i)+(j + 1)*(N + 1));
		}

	talaj_gpuBufferPos.BufferData(talajPos);
	talaj_gpuBufferNormal.BufferData(talajNorm);
	talaj_gpuBufferTex.BufferData(talajText);
	talaj_gpuBufferIndices.BufferData(talajInd);


	//-----------------------------------------------------------------------------------------------
	//kocka mozdonyhoz
	mozdony_gpuBufferPos.BufferData(
		std::vector<glm::vec3>{
		// hátsó lap
		glm::vec3(-1, -1, -1),
			glm::vec3(1, -1, -1),
			glm::vec3(1, 1, -1),
			glm::vec3(-1, 1, -1),
			// elülső lap
			glm::vec3(-1, -1, 1),
			glm::vec3(1, -1, 1),
			glm::vec3(1, 1, 1),
			glm::vec3(-1, 1, 1),

	}
	);

	// és a primitíveket alkotó csúcspontok indexei (az előző tömbökből) - triangle list-el való kirajzolásra felkészülve
	mozdony_gpuBufferIndices.BufferData(
		std::vector<int>{
		// hátsó lap
		0, 1, 2,
			2, 3, 0,
			// elülső lap
			4, 6, 5,
			6, 4, 7,
			// bal
			0, 3, 4,
			4, 3, 7,
			// jobb
			1, 5, 2,
			5, 6, 2,
			// alsó
			1, 0, 4,
			1, 4, 5,
			// felső
			3, 2, 6,
			3, 6, 7,
	}
	);
	//-----------------------------------------------------------------------------------------------
	//kocka vagonokhoz
	vagon_gpuBufferPos.BufferData(
		std::vector<glm::vec3>{
		// hátsó lap
		glm::vec3(-1, -1, -1),
			glm::vec3(1, -1, -1),
			glm::vec3(1, 1, -1),
			glm::vec3(-1, 1, -1),
			// elülső lap
			glm::vec3(-1, -1, 1),
			glm::vec3(1, -1, 1),
			glm::vec3(1, 1, 1),
			glm::vec3(-1, 1, 1),

	}
	);

	// és a primitíveket alkotó csúcspontok indexei (az előző tömbökből) - triangle list-el való kirajzolásra felkészülve
	vagon_gpuBufferIndices.BufferData(
		std::vector<int>{
		// hátsó lap
		0, 1, 2,
			2, 3, 0,
			// elülső lap
			4, 6, 5,
			6, 4, 7,
			// bal
			0, 3, 4,
			4, 3, 7,
			// jobb
			1, 5, 2,
			5, 6, 2,
			// alsó
			1, 0, 4,
			1, 4, 5,
			// felső
			3, 2, 6,
			3, 6, 7,
	}
	);
	//-----------------------------------------------------------------------------------------------

	m_gpuBufferPos.BufferData( 
		std::vector<glm::vec3>{
			//		  X, Y, Z
			glm::vec3(-10,-1, -10),
			glm::vec3(-10,-1,  10), 
			glm::vec3( 10,-1, -10),
			glm::vec3( 10,-1,  10) 
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
			0,1,2,	// 1. háromszög: 0-1-2 csúcspontokból
			2,1,3	// 2. háromszög: 2-1-3 csúcspontokból
		}
	);

	// geometria VAO-ban való regisztrálása
	m_vao.Init(
		{
			{ CreateAttribute<0, glm::vec3, 0, sizeof(glm::vec3)>, m_gpuBufferPos },	// 0-ás attribútum "lényegében" glm::vec3-ak sorozata és az adatok az m_gpuBufferPos GPU pufferben vannak
			{ CreateAttribute<1, glm::vec3, 0, sizeof(glm::vec3)>, m_gpuBufferNormal },	// 1-es attribútum "lényegében" glm::vec3-ak sorozata és az adatok az m_gpuBufferNormal GPU pufferben vannak
			{ CreateAttribute<2, glm::vec2, 0, sizeof(glm::vec2)>, m_gpuBufferTex }		// 2-es attribútum "lényegében" glm::vec2-ők sorozata és az adatok az m_gpuBufferTex GPU pufferben vannak
		},
		m_gpuBufferIndices
	);

	talaj_vao.Init(
		{
			{ CreateAttribute<0, glm::vec3, 0, sizeof(glm::vec3)>, talaj_gpuBufferPos },	// 0-ás attribútum "lényegében" glm::vec3-ak sorozata és az adatok az m_gpuBufferPos GPU pufferben vannak
			{ CreateAttribute<1, glm::vec3, 0, sizeof(glm::vec3)>, talaj_gpuBufferNormal },	// 1-es attribútum "lényegében" glm::vec3-ak sorozata és az adatok az m_gpuBufferNormal GPU pufferben vannak
			{ CreateAttribute<2, glm::vec2, 0, sizeof(glm::vec2)>, talaj_gpuBufferTex }		// 2-es attribútum "lényegében" glm::vec2-ők sorozata és az adatok az m_gpuBufferTex GPU pufferben vannak
		},
		talaj_gpuBufferIndices
	);


	mozdony_vao.Init(
		{
			{ CreateAttribute<0, glm::vec3, 0, sizeof(glm::vec3)>, mozdony_gpuBufferPos },	// 0-ás attribútum "lényegében" glm::vec3-ak sorozata és az adatok az m_gpuBufferPos GPU pufferben vannak
			{ CreateAttribute<1, glm::vec3, 0, sizeof(glm::vec3)>, mozdony_gpuBufferNormal },	// 1-es attribútum "lényegében" glm::vec3-ak sorozata és az adatok az m_gpuBufferNormal GPU pufferben vannak
		},
		mozdony_gpuBufferIndices
	);

	vagon_vao.Init(
		{
			{ CreateAttribute<0, glm::vec3, 0, sizeof(glm::vec3)>, vagon_gpuBufferPos },	// 0-ás attribútum "lényegében" glm::vec3-ak sorozata és az adatok az m_gpuBufferPos GPU pufferben vannak
			{ CreateAttribute<1, glm::vec3, 0, sizeof(glm::vec3)>, vagon_gpuBufferNormal },	// 1-es attribútum "lényegében" glm::vec3-ak sorozata és az adatok az m_gpuBufferNormal GPU pufferben vannak
		},
		vagon_gpuBufferIndices
	);

	
	// textúra betöltése
	m_textureMetal.FromFile("texture.png");

	// mesh betöltése
	m_mesh = ObjParser::parse("suzanne.obj");

	// kamera
	m_camera.SetProj(45.0f, 640.0f / 480.0f, 0.01f, 1000.0f);

	return true;
}

void CMyApp::Clean()
{
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

	// tengelyek
	/*m_axesProgram.Use();
	m_axesProgram.SetUniform("mvp", m_camera.GetViewProj()*glm::translate(glm::vec3(0,0.1f,0)));

	glDrawArrays(GL_LINES, 0, 6);

	// kontrollpontok
	m_pointProgram.Use();
	
	m_pointProgram.SetUniform("mvp", m_camera.GetViewProj());
	m_pointProgram.SetUniform("points", m_controlPoints);
	m_pointProgram.SetUniform("color", glm::vec4(1,0,1,1));

	glDrawArrays(GL_POINTS, 0, (GLsizei)m_controlPoints.size());

	// kontrollpontokat összekötő szakaszok
	m_pointProgram.SetUniform("color", glm::vec4(0.5, 0.5, 0.5, 1));
	glDrawArrays(GL_LINE_STRIP, 0, (GLsizei)m_controlPoints.size());

	// mindegyik geometria ugyanazt a shader programot hasznalja: ne kapcsolgassuk most ki-be
	/*m_program.Use();

	m_program.SetTexture("texImage", 0, m_textureMetal);

	// talaj
	glm::mat4 wallWorld = glm::translate(glm::vec3(0, 0, 0));
	m_program.SetUniform("world", wallWorld);
	m_program.SetUniform("worldIT", glm::transpose(glm::inverse(wallWorld)));
	m_program.SetUniform("MVP", m_camera.GetViewProj()*wallWorld);
	m_program.SetUniform("Kd", glm::vec4(1,1,1, 1));

	m_vao.Bind();
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	m_vao.Unbind(); // nem feltétlen szükséges: a m_mesh->draw beállítja a saját VAO-ját*/

	//saját talaj
	m_talajProgram.Use();

	m_talajProgram.SetTexture("texImage", 0, m_textureMetal);

	glm::mat4 talajWorld = glm::translate(glm::vec3(0, 0, 0));
	m_talajProgram.SetUniform("world", talajWorld);
	m_talajProgram.SetUniform("worldIT", glm::transpose(glm::inverse(talajWorld)));
	m_talajProgram.SetUniform("MVP", m_camera.GetViewProj()*talajWorld);
	m_talajProgram.SetUniform("Kd", glm::vec4(1, 1, 1, 1));

	talaj_vao.Bind();
	glDrawElements(GL_TRIANGLES, 3 * 2 * N * M, GL_UNSIGNED_INT, 0);
	talaj_vao.Unbind(); // nem feltétlen szükséges: a m_mesh->draw beállítja a saját VAO-ját

	//---------------------------------------------------------------------------------------
	//mozdony téglalap
	float t = SDL_GetTicks() / 1000.f;

	mozdony_vao.Bind();

	m_mozdonyProgram.Use();

	glm::mat4 mozdonyWorld = glm::rotate(glm::radians(t * 120.f), glm::vec3(0, 1, 0)) * glm::translate(glm::vec3(0, 0, -8.5f)) * glm::translate(glm::vec3(0, 2, 0)) * glm::scale(glm::vec3(-1.5f, -0.5f, -0.5f));

	m_mozdonyProgram.SetUniform("MVP", m_camera.GetViewProj()*mozdonyWorld);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);

	//mozdony fülke
	mozdony_vao.Bind();

	m_mozdonyProgram.Use();

	mozdonyWorld = glm::rotate(glm::radians(t * 120.f), glm::vec3(0, 1, 0)) * glm::translate(glm::vec3(0, 0, -8.5f)) * glm::translate(glm::vec3(1, 2.75f, 0)) * glm::scale(glm::vec3(-0.5f, -0.25f, -0.5f));

	m_program.SetUniform("MVP", m_camera.GetViewProj()*mozdonyWorld);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);

	//---------------------------------------------------------------------------------------
	//vagonok téglalap
	vagon_vao.Bind();

	m_vagonProgram.Use();

	glm::mat4 vagonWorld;

	for (int i = 0; i < 3; i++) {
	
		vagonWorld = /*glm::translate(glm::vec3(8.5f * cosf(glm::radians(30.f)), 0, -8.5f * sinf(glm::radians(30.f)))) **/
			glm::rotate(glm::radians(t * 120.f), glm::vec3(0, 1, 0)) *
			glm::rotate(glm::radians(-30.f * (i+1)), glm::vec3(0, 1, 0)) *
			glm::translate(glm::vec3(0, 0, -8.5f)) *
			glm::translate(glm::vec3(0, 2, 0)) * glm::scale(glm::vec3(-2.0f, -0.75f, -0.6f));

		m_vagonProgram.SetUniform("MVP", m_camera.GetViewProj()*vagonWorld);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);

	}

	//-------------------------------------------------------------------------------------------
	// Suzanne 1
	/*glm::mat4 suzanne1World = glm::translate( Eval(m_currentParam) );
	m_program.SetUniform("world", suzanne1World);
	m_program.SetUniform("worldIT", glm::transpose(glm::inverse(suzanne1World)));
	m_program.SetUniform("MVP", m_camera.GetViewProj()*suzanne1World);
	m_program.SetUniform("Kd", glm::vec4(1, 0.3, 0.3, 1));
	
	m_mesh->draw();

	// végetért a 3D színtér rajzolása
	m_program.Unuse();
	*/
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
		static size_t currentItem = 0;

		ImGui::ListBoxHeader("List", 4);
		for (size_t i = 0; i < m_pointName.size(); ++i)
		{
			if (ImGui::Selectable(m_pointName[i].c_str(), (i==currentItem))  )
				currentItem = i;
		}
		ImGui::ListBoxFooter();

		if (ImGui::Button("Add") && m_pointName.size() < kMaxPointCount)
		{
			m_pointName.push_back("Point " + std::to_string(m_pointName.size()+1));
			m_controlPoints.push_back({ 0,0,0 });
			currentItem = m_pointName.size() - 1;
		}

		ImGui::SameLine();

		if (ImGui::Button("Delete") && m_pointName.size() > 0 && currentItem < m_pointName.size() )
		{
			m_pointName.erase(m_pointName.begin() + currentItem);
			m_controlPoints.erase(m_controlPoints.begin() + currentItem);

			size_t number = currentItem+1;
			for (auto& it = m_pointName.begin()+ currentItem; it != m_pointName.end(); ++it)
			{
				*it = "Point " + std::to_string(number);
				++number;
			}
			if (m_pointName.size() == 0)
				currentItem = 0;
			else
				if (currentItem >= m_pointName.size())
					currentItem = m_pointName.size() - 1;
		}

		if (m_controlPoints.size() > 0)
			ImGui::SliderFloat3("Coordinates", &(m_controlPoints[currentItem][0]), -10, 10);

		ImGui::SliderFloat("Parameter", &m_currentParam, 0, (float)(m_controlPoints.size()-1));

		// 1. feladat: Suzanne feje mindig forduljon a menetirány felé! Először ezt elég síkban (=XZ) megoldani!
		// 2. feladat: valósíts meg egy Animate gombot! Amíg nyomva van az m_currentParameter periodikusan változzon a [0,m_controlPoints.size()-1] intervallumon belül!
		// 3. feladat: egyenközű Catmull-Rom spline-nal valósítsd meg Suzanne görbéjét a mostani törött vonal helyett!

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

glm::vec3 CMyApp::Eval(float t)
{
	if (m_controlPoints.size() == 0)
		return glm::vec3(0);

	int interval = (int)t;

	if (interval < 0)
		return m_controlPoints[0];
		
	if (interval >= m_controlPoints.size()-1)
		return m_controlPoints[m_controlPoints.size()-1];

	float localT = t - interval;
	return (1- localT)*m_controlPoints[interval] + localT*m_controlPoints[interval+1];
}
