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
	m_camera.SetView(glm::vec3(0, 0, 5), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
}


CMyApp::~CMyApp(void)
{
	
}

glm::vec3 CMyApp::GetSpherePos(float u, float v)
{
	// origó középpontú, egységsugarú gömb parametrikus alakja: http://hu.wikipedia.org/wiki/G%C3%B6mb#Egyenletek 
	// figyeljünk:	matematikában sokszor a Z tengely mutat felfelé, de nálunk az Y, tehát a legtöbb képlethez képest nálunk
	//				az Y és Z koordináták felcserélve szerepelnek
	/* Gömb */
	u *= 3.14159265359f;
	v *= 3.14159265359f;
	float cu = cosf(u), su = sinf(u), cv = cosf(v), sv = sinf(v);
	float r = 1;
	return glm::vec3( r*cu*sv, r*cv, r*su*sv );
}

glm::vec3 CMyApp::GetCirclePos(float u, float v)
{

	u *= 2 * 3.14159265359f;
	v *= 3.14159265359f;
	float cu = cosf(u), su = sinf(u), cv = cosf(v), sv = sinf(v);
	float r = 1;
	return glm::vec3(r*cu*sv, r, r*su*sv);
}

glm::vec3 CMyApp::GetCircleNorm(float u, float v)
{
	// origó középpontú, egységsugarú gömb parametrikus alakja: http://hu.wikipedia.org/wiki/G%C3%B6mb#Egyenletek 
	// figyeljünk:	matematikában sokszor a Z tengely mutat felfelé, de nálunk az Y, tehát a legtöbb képlethez képest nálunk
	//				az Y és Z koordináták felcserélve szerepelnek
	/* Gömb */
	u *= 2 * 3.14159265359f;
	v *= 3.14159265359f;
	float cu = cosf(u), su = sinf(u), cv = cosf(v), sv = sinf(v);
	float r = 1;
	return glm::vec3(0, r, 0);
}

void CMyApp::InitCube() {
	std::vector<glm::vec3> pos = {
		// hátsó lap
		glm::vec3(-1, -1, -1),
		glm::vec3(1, -1, -1),
		glm::vec3(1, 0, -1),
		glm::vec3(-1, 0, -1),
		// elülső lap
		glm::vec3(-1, -1, 1),
		glm::vec3(1, -1, 1),
		glm::vec3(1, 0, 1),
		glm::vec3(-1, 0, 1),
		glm::vec3(0, 1, 0)
	};

	// normálisai
	std::vector<glm::vec3> norm = {
		// normal.X,.Y,.Z
		glm::vec3(0, 1, 0), // 0-ás csúcspont
		glm::vec3(0, 1, 0), // 1-es
		glm::vec3(0, 1, 0), // 2-es
		glm::vec3(0, 1, 0)  // 3-as
	};

	std::vector<GLuint> ind = {
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
		3, 8, 2,
		6, 8, 7,
		2, 8, 6,
		3, 8, 7
	};

	ArrayBuffer posBuffer;
	ArrayBuffer normBuffer;
	ArrayBuffer texBuffer;
	IndexBuffer indBuffer;
	posBuffer.BufferData(pos);
	normBuffer.BufferData(norm);
	//texBuffer.BufferData(tex);
	indBuffer.BufferData(ind);

	m_vao_cube.Init(
	{
		{ CreateAttribute<0, glm::vec3, 0, sizeof(glm::vec3)>, posBuffer },
		{ CreateAttribute<1, glm::vec3, 0, sizeof(glm::vec3)>, normBuffer }/*,
		{ CreateAttribute<2, glm::vec2, 0, sizeof(glm::vec2)>, texBuffer }*/
	},
		indBuffer
	);
}

void CMyApp::InitShpere() {
	std::vector<glm::vec3> pos;
	std::vector<glm::vec3> norm;
	std::vector<glm::vec2> tex;
	std::vector<GLuint> ind;

	pos.resize((N + 1)*(M + 1));
	norm.resize((N + 1)*(M + 1));
	tex.resize((N + 1)*(M + 1));
	ind.resize(3 * 2 * (N)*(M));

	for (int i = 0; i <= N; ++i)
		for (int j = 0; j <= M; ++j)
		{
			float u = i / (float)N;
			float v = j / (float)M;

			pos[i + j*(N + 1)] = GetSpherePos(u, v);
			norm[i + j*(N + 1)] = GetSpherePos(u, v); // mivel egység sugarú
			tex[i + j*(N + 1)] = glm::vec2(u, v);
		}
	for (int i = 0; i<N; ++i)
		for (int j = 0; j<M; ++j)
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
			ind[6 * i + j * 3 * 2 * (N)+0] = (i)+(j)*	(N + 1);
			ind[6 * i + j * 3 * 2 * (N)+1] = (i + 1) + (j)*	(N + 1);
			ind[6 * i + j * 3 * 2 * (N)+2] = (i)+(j + 1)*(N + 1);
			ind[6 * i + j * 3 * 2 * (N)+3] = (i + 1) + (j)*	(N + 1);
			ind[6 * i + j * 3 * 2 * (N)+4] = (i + 1) + (j + 1)*(N + 1);
			ind[6 * i + j * 3 * 2 * (N)+5] = (i)+(j + 1)*(N + 1);
		}

	ArrayBuffer posBuffer;
	ArrayBuffer normBuffer;
	ArrayBuffer texBuffer;
	IndexBuffer indBuffer;
	posBuffer.BufferData(pos);
	normBuffer.BufferData(norm);
	texBuffer.BufferData(tex);
	indBuffer.BufferData(ind);

	m_vao_shpere.Init(
	{
		{ CreateAttribute<0, glm::vec3, 0, sizeof(glm::vec3)>, posBuffer },
		{ CreateAttribute<1, glm::vec3, 0, sizeof(glm::vec3)>, normBuffer },
		{ CreateAttribute<2, glm::vec2, 0, sizeof(glm::vec2)>, texBuffer }
	},
		indBuffer
	);
}

void CMyApp::InitCircle() {
	std::vector<glm::vec3> pos;
	std::vector<glm::vec3> norm;
	std::vector<glm::vec2> tex;
	std::vector<GLuint> ind;

	pos.resize((N + 1)*(M + 1));
	norm.resize((N + 1)*(M + 1));
	tex.resize((N + 1)*(M + 1));
	ind.resize(3 * 2 * (N)*(M));

	for (int i = 0; i <= N; ++i)
		for (int j = 0; j <= M; ++j)
		{
			float u = i / (float)N;
			float v = j / (float)M;

			pos[i + j * (N + 1)] = GetCirclePos(u, v);
			norm[i + j * (N + 1)] = GetCircleNorm(u, v); // mivel egység sugarú
			tex[i + j * (N + 1)] = glm::vec2(u, v);
		}
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
			ind[6 * i + j * 3 * 2 * (N)+0] = (i)+(j)*	(N + 1);
			ind[6 * i + j * 3 * 2 * (N)+1] = (i + 1) + (j)*	(N + 1);
			ind[6 * i + j * 3 * 2 * (N)+2] = (i)+(j + 1)*(N + 1);
			ind[6 * i + j * 3 * 2 * (N)+3] = (i + 1) + (j)*	(N + 1);
			ind[6 * i + j * 3 * 2 * (N)+4] = (i + 1) + (j + 1)*(N + 1);
			ind[6 * i + j * 3 * 2 * (N)+5] = (i)+(j + 1)*(N + 1);
		}

	ArrayBuffer posBuffer;
	ArrayBuffer normBuffer;
	ArrayBuffer texBuffer;
	IndexBuffer indBuffer;
	posBuffer.BufferData(pos);
	normBuffer.BufferData(norm);
	texBuffer.BufferData(tex);
	indBuffer.BufferData(ind);

	m_vao_circle.Init(
		{
			{ CreateAttribute<0, glm::vec3, 0, sizeof(glm::vec3)>, posBuffer },
			{ CreateAttribute<1, glm::vec3, 0, sizeof(glm::vec3)>, normBuffer },
			{ CreateAttribute<2, glm::vec2, 0, sizeof(glm::vec2)>, texBuffer }
		},
		indBuffer
	);
}

bool CMyApp::Init()
{
	// törlési szín legyen fehér
	glClearColor(1,1,1, 1);

	//glEnable(GL_CULL_FACE); // kapcsoljuk be a hatrafele nezo lapok eldobasat
	glEnable(GL_DEPTH_TEST); // mélységi teszt bekapcsolása (takarás)
	//glDepthMask(GL_FALSE); // kikapcsolja a z-pufferbe történő írást - https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/glDepthMask.xml

	glLineWidth(4.0f); // a vonalprimitívek vastagsága: https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glLineWidth.xhtml
	//glEnable(GL_LINE_SMOOTH);

	// átlátszóság engedélyezése
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // meghatározza, hogy az átlátszó objektum az adott pixelben hogyan módosítsa a korábbi fragmentekből oda lerakott színt: https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glBlendFunc.xhtml

	// a raszterizált pontprimitívek mérete
	glPointSize(15.0f);

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
	});


	std::vector<glm::vec3> pos = {
		glm::vec3(-1, 0,-1),
		glm::vec3(1, 0,-1),
		glm::vec3(-1, 0, 1),
		glm::vec3(1, 0, 1)
	};
	std::vector<glm::vec3> norm = {
		glm::vec3(0, 1, 0),
		glm::vec3(0, 1, 0),
		glm::vec3(0, 1, 0),
		glm::vec3(0, 1, 0)
	};
	std::vector<glm::vec2> tex = { 
		glm::vec2(0, 0),
		glm::vec2(1, 0),
		glm::vec2(0, 1),
		glm::vec2(1, 1)
	};
	std::vector<GLuint> ind = { 0, 1, 2, 2, 1, 3 };


	m_gpuBufferPos.BufferData(pos);
	m_gpuBufferNormal.BufferData(norm);
	m_gpuBufferTex.BufferData(tex);
	m_gpuBufferIndices.BufferData(ind);

	// geometria VAO-ban való regisztrálása
	m_vao.Init(
		{
			{ CreateAttribute<0, glm::vec3, 0, sizeof(glm::vec3)>, m_gpuBufferPos },	// 0-ás attribútum "lényegében" glm::vec3-ak sorozata és az adatok az m_gpuBufferPos GPU pufferben vannak
			{ CreateAttribute<1, glm::vec3, 0, sizeof(glm::vec3)>, m_gpuBufferNormal },	// 1-es attribútum "lényegében" glm::vec3-ak sorozata és az adatok az m_gpuBufferNormal GPU pufferben vannak
			{ CreateAttribute<2, glm::vec2, 0, sizeof(glm::vec2)>, m_gpuBufferTex }		// 2-es attribútum "lényegében" glm::vec2-ők sorozata és az adatok az m_gpuBufferTex GPU pufferben vannak
		},
		m_gpuBufferIndices
	);


	InitCube(); 
	InitShpere();
	InitCircle();

	// textúra betöltése
	m_textureFish.FromFile("pacman.bmp");

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

	m_program.Use();
	// szellem
	m_program.SetUniform("shade", 0);
	m_program.SetUniform("useColor", 1);
	m_program.SetUniform("color", glm::vec3(0,0,1));
	DrawCube(glm::translate(glm::vec3(3, 0, 0))*glm::mat4(1));

	// pacman
	m_program.SetUniform("shade", 1);
	m_program.SetTexture("texImage", 0, m_textureFish);
	m_program.SetUniform("useColor", 0);
	DrawSphere(glm::rotate<float>(M_PI / 2, glm::vec3(1, 0, 0))*glm::mat4(1));
	DrawSphere(glm::rotate<float>(M_PI / 2, glm::vec3(1, 0, 0))*glm::rotate<float>(M_PI/2,glm::vec3(1,0,0))*glm::mat4(1));
	DrawCircle(glm::rotate<float>(M_PI / 2, glm::vec3(1, 0, 0))*glm::translate(glm::vec3(0, 0, 1))*glm::rotate<float>(-M_PI / 2, glm::vec3(1, 0, 0))*glm::mat4(1));
	DrawCircle(glm::rotate<float>(M_PI / 2, glm::vec3(1, 0, 0))*glm::translate(glm::vec3(0, 1, 0))*glm::rotate<float>(M_PI, glm::vec3(1, 0, 0))*glm::mat4(1));
	
	// tábla
	m_vao.Bind();
	m_program.SetUniform("useColor", 1);
	for (int i=0; i<Z; i++)
		for (int j = 0; j<Z; j++) {
			m_program.SetUniform("color", glm::vec3(float((i + j * (Z + 1)) %2 == 0), float((i +1 + j * (Z + 1)) % 2 == 0), float((i + j * (Z + 1)) % 2 == 0)));
			DrawFloor(glm::translate(glm::vec3(-Z/2, 0, -Z/2))*glm::translate(glm::vec3(i, -1, j))*glm::mat4(1));
		}
	
}

void CMyApp::DrawFloor(const glm::mat4& world) {
	m_program.Use();
	m_program.SetUniform("world", world);
	m_program.SetUniform("worldIT", glm::transpose(glm::inverse(world)));
	m_program.SetUniform("MVP", m_camera.GetViewProj()*world);
	m_program.SetUniform("Kd", glm::vec4(1, 1, 1, 1));

	
	glDrawElements(GL_TRIANGLES, 2 * 3, GL_UNSIGNED_INT, 0);

}

void CMyApp::DrawCube(const glm::mat4& world) {
	m_program.Use();
	m_program.SetUniform("world", world);
	m_program.SetUniform("worldIT", glm::transpose(glm::inverse(world)));
	m_program.SetUniform("MVP", m_camera.GetViewProj()*world);
	m_program.SetUniform("Kd", glm::vec4(1, 1, 1, 1));

	m_vao_cube.Bind();
	glDrawElements(GL_TRIANGLES, 42, GL_UNSIGNED_INT, 0);

}

void CMyApp::DrawSphere(const glm::mat4& world) {
	m_program.Use();
	m_program.SetUniform("world", world);
	m_program.SetUniform("worldIT", glm::transpose(glm::inverse(world)));
	m_program.SetUniform("MVP", m_camera.GetViewProj()*world);
	m_program.SetUniform("Kd", glm::vec4(1, 1, 1, 1));

	m_vao_shpere.Bind();
	glDrawElements(GL_TRIANGLES, 2*3*N*M, GL_UNSIGNED_INT, 0);
}

void CMyApp::DrawCircle(const glm::mat4& world) {
	m_program.Use();
	m_program.SetUniform("world", world);
	m_program.SetUniform("worldIT", glm::transpose(glm::inverse(world)));
	m_program.SetUniform("MVP", m_camera.GetViewProj()*world);
	m_program.SetUniform("Kd", glm::vec4(1, 1, 1, 1));

	m_vao_circle.Bind();
	glDrawElements(GL_TRIANGLES, 2 * 3 * N*M, GL_UNSIGNED_INT, 0);
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
