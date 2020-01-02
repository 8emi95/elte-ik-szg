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

glm::vec3 CMyApp::GetSpherePos(float u, float v)
{
	u *= 2* 3.14159265359f;
	v *= 3.14159265359f;
	float cu = cosf(u), su = sinf(u), cv = cosf(v), sv = sinf(v);
	float r = 1;
	return glm::vec3( r*cu*sv, r*cv, r*su*sv );
}

void CMyApp::InitCube() {
	glm::vec3 ppp(+.5, +.5, +.5);
	glm::vec3 ppn(+.5, +.5, -.5);
	glm::vec3 pnp(+.5, -.5, +.5);
	glm::vec3 pnn(+.5, -.5, -.5);
	glm::vec3 npp(-.5, +.5, +.5);
	glm::vec3 npn(-.5, +.5, -.5);
	glm::vec3 nnp(-.5, -.5, +.5);
	glm::vec3 nnn(-.5, -.5, -.5);

	std::vector<glm::vec3> pos = {
				
		//elulso _|
		nnp,pnp,ppp,npp,
		//jobb _|
		pnp,pnn,ppn,ppp,
		//hatso |_
		pnn,nnn,npn,ppn,
		//bal |_
		nnn,nnp,npp,npn,
		//also |¨
		nnn,pnn,pnp,nnp,
		//felso ¨|
		npp,ppp,ppn,npn
	};

	glm::vec3 x(1, 0, 0);
	glm::vec3 y(0, 1, 0);
	glm::vec3 z(0, 0, 1);

	std::vector<glm::vec3> norm = {
		//elulso
		z,z,z,z,
		//jobb
		x,x,x,x,
		//hatso
		-z,-z,-z,-z,
		//bal
		-x,-x,-x,-x,
		//also
		-y,-y,-y,-y,
		//felso
		y,y,y,y
	};

	glm::vec2 mm(0, 0);
	glm::vec2 mM(0, 1);
	glm::vec2 Mm(1, 0);
	glm::vec2 MM(1, 1);

	std::vector<glm::vec2> tex = {
		mm,Mm,MM,mM,
		mm,Mm,MM,mM,
		mm,Mm,MM,mM,
		mm,Mm,MM,mM,
		mm,Mm,MM,mM,
		mm,Mm,MM,mM
	};

	std::vector<GLuint> ind;
	ind.reserve(36);
	for (int i = 0; i < 6; ++i) {
		ind.push_back(4 * i + 0);
		ind.push_back(4 * i + 1);
		ind.push_back(4 * i + 2);
		ind.push_back(4 * i + 2);
		ind.push_back(4 * i + 3);
		ind.push_back(4 * i + 0);
	}

	ArrayBuffer posBuffer;
	ArrayBuffer normBuffer;
	ArrayBuffer texBuffer;
	IndexBuffer indBuffer;
	posBuffer.BufferData(pos);
	normBuffer.BufferData(norm);
	texBuffer.BufferData(tex);
	indBuffer.BufferData(ind);

	m_vao_cube.Init(
	{
		{ CreateAttribute<0, glm::vec3, 0, sizeof(glm::vec3)>, posBuffer },
		{ CreateAttribute<1, glm::vec3, 0, sizeof(glm::vec3)>, normBuffer },
		{ CreateAttribute<2, glm::vec2, 0, sizeof(glm::vec2)>, texBuffer }
	},
		indBuffer
	);
}

void CMyApp::InitSphere() {
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


	// tengelyeket kirajzoló shader rövid inicializálása
	m_axesProgram.Init({
		{GL_VERTEX_SHADER, "axes.vert"},
		{GL_FRAGMENT_SHADER, "axes.frag"}
	});

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


	InitCube(); 
	InitSphere();

	// textúra betöltése
	m_textureMetal.FromFile("texture.png");
	m_textureWater.FromFile("water.jpg");
	m_textureWall.FromFile("wall.png");
	m_textureFish.FromFile("hal.png");

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

	m_program.Use();
	// akvárium
	m_program.SetTexture("texImage", 0, m_textureWater);
	m_program.SetUniform("shade", 0);
	m_program.SetUniform("useColor", 0);
	DrawCube(glm::scale(glm::vec3(60,15,40))*glm::translate(glm::vec3(0, 0.5f, 0))*glm::scale(glm::vec3(-1)));

	// piramis
	m_program.SetTexture("texImage", 0, m_textureWall);
	m_program.SetUniform("shade", 1);
	m_program.SetUniform("useColor", 0);
	DrawPyramid(glm::scale(glm::vec3(2.0f)));

	// hal
	m_program.SetTexture("texImage", 0, m_textureFish);
	m_program.SetUniform("shade", 1);

	for (int i = 0; i < 10; ++i)
	{
		float t = SDL_GetTicks() / 1000.f;
		DrawSphere(glm::rotate<float>(2*M_PI * t/3, glm::vec3(0,1,0))*glm::translate(glm::vec3(sinf(2*M_PI*t)+1, sinf(2*M_PI*t)+2, 0))*glm::rotate<float>(i*2*M_PI/10, glm::vec3(0,1,0))*glm::translate(glm::vec3(0, 2, 13))*glm::scale(glm::vec3(3.0f / 2.0f, 1.0f, 1.5f / 2.0f)));
	}
	
	// buborék
	m_program.SetUniform("useColor", 1);
	m_program.SetUniform("color", glm::vec3(1));
	m_program.SetUniform("shade", 0);
	DrawSphere(glm::translate(glm::vec3(0, 5, 5))*glm::scale(glm::vec3(0.25f)));
}

void CMyApp::DrawCube(const glm::mat4& world) {
	m_program.Use();
	m_program.SetUniform("world", world);
	m_program.SetUniform("worldIT", glm::transpose(glm::inverse(world)));
	m_program.SetUniform("MVP", m_camera.GetViewProj()*world);
	m_program.SetUniform("Kd", glm::vec4(1, 1, 1, 1));

	m_vao_cube.Bind();
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

}
void CMyApp::DrawPyramid(const glm::mat4& world) {
	DrawCube(world*glm::translate(glm::vec3(0, 0.5f, 0))*glm::scale(glm::vec3(8, 2, 8)));
	DrawCube(world*glm::translate(glm::vec3(0, 3.5f, 0))*glm::scale(glm::vec3(6, 2, 6)));
	DrawCube(world*glm::translate(glm::vec3(0, 5.5f, 0))*glm::scale(glm::vec3(4, 2, 4)));
	DrawCube(world*glm::translate(glm::vec3(0, 7.5f, 0))*glm::scale(glm::vec3(2,2,2)));
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
