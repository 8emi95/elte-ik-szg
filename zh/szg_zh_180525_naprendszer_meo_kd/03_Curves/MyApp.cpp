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

//----------------------------------------------------------------------------
//FÉLGÖMHÖZ
glm::vec3 CMyApp::GetPos(float u, float v)
{
	// origó középpontú, egységsugarú gömb parametrikus alakja: http://hu.wikipedia.org/wiki/G%C3%B6mb#Egyenletek 
	// figyeljünk:	matematikában sokszor a Z tengely mutat felfelé, de nálunk az Y, tehát a legtöbb képlethez képest nálunk
	//				az Y és Z koordináták felcserélve szerepelnek
	u *= 3.1415f;
	v *= 3.1415f;
	float cu = cosf(u), su = sinf(u), cv = cosf(v), sv = sinf(v);
	float r = 2;

	return glm::vec3(r*cu*sv, r*cv, r*su*sv);
}

glm::vec3 CMyApp::GetNorm(float u, float v)
{
	// Képlettel
	u *= 3.1415f;
	v *= 3.1415f;
	float cu = cosf(u), su = sinf(u), cv = cosf(v), sv = sinf(v);
	return glm::vec3(cu*sv, cv, su*sv);

	// Numerikusan (nem kell ismerni a képletet, elég a pozícióét)
	/*glm::vec3 uv = GetPos(u, v);
	glm::vec3 du = GetPos(u+0.01, v)-GetPos(u-0.01, v);
	glm::vec3 dv = GetPos(u, v+0.01)-GetPos(u, v-0.01);

	return (glm::cross(du, dv));*/
}

glm::vec2 CMyApp::GetTex(float u, float v)
{
	return glm::vec2(u, 1 - v);
}
//---------------------------------------------------------------------------

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

	// kontrollpontokat kirajzoló program
	m_pointProgram.Init({
		{ GL_VERTEX_SHADER, "pointDrawer.vert" },
		{ GL_FRAGMENT_SHADER, "pointDrawer.frag" }
	});

	korlap_program.AttachShaders({
		{ GL_VERTEX_SHADER, "korlap.vert" },
		{ GL_FRAGMENT_SHADER, "korlap.frag" }
		});

	// attributomok osszerendelese a VAO es shader kozt
	korlap_program.BindAttribLocations({
		{ 0, "vs_in_pos" },				// VAO 0-as csatorna menjen a vs_in_pos-ba
		{ 1, "vs_in_col" }				// VAO 1-es csatorna menjen a vs_in_col-ba
		});

	korlap_program.LinkProgram();

	felgomb_program.AttachShaders({
		{ GL_VERTEX_SHADER, "felgomb.vert" },
		{ GL_FRAGMENT_SHADER, "felgomb.frag" }
		});

	// attributomok osszerendelese a VAO es shader kozt
	felgomb_program.BindAttribLocations({
		{ 0, "vs_in_pos" },				// VAO 0-as csatorna menjen a vs_in_pos-ba
		{ 1, "vs_in_norm" },				// VAO 1-es csatorna menjen a vs_in_col-ba
		{ 2, "vs_in_text" }
		});

	felgomb_program.LinkProgram();


	henger_program.AttachShaders({
		{ GL_VERTEX_SHADER, "henger.vert" },
		{ GL_FRAGMENT_SHADER, "henger.frag" }
		});

	// attributomok osszerendelese a VAO es shader kozt
	henger_program.BindAttribLocations({
		{ 0, "vs_in_pos" },				// VAO 0-as csatorna menjen a vs_in_pos-ba
		{ 1, "vs_in_norm" },				// VAO 1-es csatorna menjen a vs_in_col-ba
		{ 2, "vs_in_text" }
		});

	henger_program.LinkProgram();

	//-------------------------------------------------------------------------------
	// shader program rövid létrehozása, egyetlen függvényhívással a fenti három:
	m_programSkybox.Init(
		{
			{ GL_VERTEX_SHADER, "skybox.vert" },
		{ GL_FRAGMENT_SHADER, "skybox.frag" }
		},
	{
		{ 0, "vs_in_pos" },				// VAO 0-as csatorna menjen a vs_in_pos-ba
	}
	);
	//-------------------------------------------------------------------------------
	//KÖRLAP
	std::vector<glm::vec3> korlapPos;
	std::vector<glm::vec3> korlapNorm;
	std::vector<glm::vec2> korlapTex;
	std::vector<int> korlapInd;

	korlapPos.push_back(glm::vec3(0, 0, 0));
	korlapNorm.push_back(glm::vec3(0, 1, 0));
	korlapTex.push_back(glm::vec2(0.5f, 0.5f));

	int r = 2;

	for (int i = 0; i < 40; i++) {
		float a = -i / 40.f * 2 * M_PI;
		korlapPos.push_back(glm::vec3(r * sinf(((float)i * (360 / 40)) * (M_PI / 180)), 0, r * cosf(((float)i * (360 / 40)) * (M_PI / 180))));
		korlapNorm.push_back(glm::vec3(0, 1, 0));
		korlapTex.push_back(glm::vec2(cos(a) / 2 + .5, sin(a) / 2 + .5));
		korlapInd.push_back(i);
		
	}

	korlapInd.push_back(40);
	korlapInd.push_back(1);

	korlap_BufferPos.BufferData(korlapPos);
	korlap_BufferIndices.BufferData(korlapInd);
	korlap_BufferNorm.BufferData(korlapNorm);
	korlap_BufferText.BufferData(korlapTex);


	korlap_vao.Init(
		{
			{CreateAttribute<0, glm::vec3, 0, sizeof(glm::vec3)>, korlap_BufferPos},
			{ CreateAttribute<1, glm::vec3, 0, sizeof(glm::vec3)>, korlap_BufferNorm },
			{ CreateAttribute<2, glm::vec2, 0, sizeof(glm::vec2)>, korlap_BufferText },
		},
		korlap_BufferIndices
	);
	//-------------------------------------------------------------------------------
	//FÉLGÖMB
	std::vector<glm::vec3> felgombPos;
	std::vector<glm::vec3> felgombNorm;
	std::vector<glm::vec2> felgombText;
	std::vector<int> felgombInd;
	//Vertex vert[(N + 1)*(M + 1)];
	for (int i = 0; i <= N; ++i)
		for (int j = 0; j <= M; ++j)
		{
			float u = i / (float)N;
			float v = j / (float)M;

			felgombPos.push_back(GetPos(u, v));
			felgombNorm.push_back(GetNorm(u, v));
			felgombText.push_back(GetTex(u, v));
		}
	//GLushort indices[3 * 2 * (N)*(M)];
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
			felgombInd.push_back((i)+(j)*	(N + 1));
			felgombInd.push_back((i + 1) + (j)*	(N + 1));
			felgombInd.push_back((i)+(j + 1)*(N + 1));
			felgombInd.push_back((i + 1) + (j)*	(N + 1));
			felgombInd.push_back((i + 1) + (j + 1)*(N + 1));
			felgombInd.push_back((i)+(j + 1)*(N + 1));
		}

	felgomb_BufferPos.BufferData(felgombPos);
	felgomb_BufferNorm.BufferData(felgombNorm);
	felgomb_BufferText.BufferData(felgombText);
	felgomb_BufferIndices.BufferData(felgombInd);
	//-------------------------------------------------------------------------------
	//HENGER
	std::vector<glm::vec3> hengerPos;
	std::vector<glm::vec3> hengerNorm;
	std::vector<glm::vec2> hengerTex;

	for (int i = 0; i <= hengerNum; i++) {
		float a = -i / (float)hengerNum * 2 * M_PI;
		hengerPos.push_back(glm::vec3(cos(a), 1, sin(a)));
		hengerNorm.push_back(glm::vec3(cos(a), 0, sin(a)));
		hengerTex.push_back(glm::vec2(i / (float)hengerNum, 1));

		hengerPos.push_back(glm::vec3(cos(a), 0, sin(a)));
		hengerNorm.push_back(glm::vec3(cos(a), 0, sin(a)));
		hengerTex.push_back(glm::vec2(i / (float)hengerNum, 0));
	}

	henger_BufferPos.BufferData(hengerPos);
	henger_BufferNorm.BufferData(hengerNorm);
	henger_BufferText.BufferData(hengerTex);

	// geometria VAO-ban való regisztrálása
	henger_vao.Init(
		{
			{ CreateAttribute<0, glm::vec3, 0, sizeof(glm::vec3)>, henger_BufferPos },	// 0-ás attribútum "lényegében" glm::vec3-ak sorozata és az adatok az m_gpuBufferPos GPU pufferben vannak
			{ CreateAttribute<1, glm::vec3, 0, sizeof(glm::vec3)>, henger_BufferNorm },	// 1-es attribútum "lényegében" glm::vec3-ak sorozata és az adatok az m_gpuBufferNormal GPU pufferben vannak
			{ CreateAttribute<2, glm::vec2, 0, sizeof(glm::vec2)>, henger_BufferText }	    // 2-es attribútum "lényegében" glm::vec2-ők sorozata és az adatok az m_gpuBufferTex GPU pufferben vannak
		}
	);

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

	felgomb_vao.Init({
		{ CreateAttribute<0, glm::vec3, 0, sizeof(glm::vec3)>, felgomb_BufferPos },
		{ CreateAttribute<1, glm::vec3, 0, sizeof(glm::vec3)>, felgomb_BufferNorm },
		{ CreateAttribute<2, glm::vec2, 0, sizeof(glm::vec2)>, felgomb_BufferText },
		},
		felgomb_BufferIndices
		);

	// textúra betöltése
	m_textureMetal.FromFile("shell.jpg");
	m_textureMetalEarth.FromFile("earth.png");
	m_textureMetalScales.FromFile("scales.jpg");
	//-----------------------------------------------------------------------------------
	// skybox
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	glGenTextures(1, &m_skyboxTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_skyboxTexture);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	TextureFromFileAttach("xpos.png", GL_TEXTURE_CUBE_MAP_POSITIVE_X);
	TextureFromFileAttach("xneg.png", GL_TEXTURE_CUBE_MAP_NEGATIVE_X);
	TextureFromFileAttach("ypos.png", GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
	TextureFromFileAttach("yneg.png", GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);
	TextureFromFileAttach("zpos.png", GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
	TextureFromFileAttach("zneg.png", GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	//-----------------------------------------------------------------------------------
	// kamera
	m_camera.SetProj(45.0f, 640.0f / 480.0f, 0.01f, 1000.0f);
	return true;
}

void CMyApp::TextureFromFileAttach(const char* filename, GLuint role) const
{
	SDL_Surface* loaded_img = IMG_Load(filename);

	int img_mode = 0;

	if (loaded_img == 0)
	{
		std::cout << "[TextureFromFile] Hiba a kép betöltése közben: " << filename << std::endl;
		return;
	}

#if SDL_BYTEORDER == SDL_LIL_ENDIAN
	if (loaded_img->format->BytesPerPixel == 4)
		img_mode = GL_BGRA;
	else
		img_mode = GL_BGR;
#else
	if (loaded_img->format->BytesPerPixel == 4)
		img_mode = GL_RGBA;
	else
		img_mode = GL_RGB;
#endif

	glTexImage2D(role, 0, GL_RGBA, loaded_img->w, loaded_img->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, loaded_img->pixels);

	SDL_FreeSurface(loaded_img);
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

	float t = SDL_GetTicks() / 1000.0f;

	




	for (int j= 0; j < m_controlPoints.size(); j++) {
		//Föld
		//körlap
		glm::mat4 korlapWorld;
		if (b) {
			korlap_vao.Bind();

			korlap_program.Use();

			korlap_program.SetTexture("texImage", 0, m_textureMetalEarth);

			korlapWorld = glm::translate(m_controlPoints.at(j)) * glm::rotate(glm::radians(amp * 30.f * cos(glm::radians(t*180.f))), glm::vec3(0, 0, 1)) * glm::translate(glm::vec3(0, 2, 0));

			korlap_program.SetUniform("world", korlapWorld);
			korlap_program.SetUniform("worldIT", glm::transpose(glm::inverse(korlapWorld)));
			korlap_program.SetUniform("Kd", glm::vec4(1, 0.3, 0.3, 1));
			korlap_program.SetUniform("MVP", m_camera.GetViewProj()*korlapWorld);
			glDrawElements(GL_TRIANGLE_FAN, 40 + 2, GL_UNSIGNED_INT, nullptr);

			//körlap
			korlap_vao.Bind();

			korlap_program.Use();

			korlap_program.SetTexture("texImage", 0, m_textureMetalEarth);

			korlapWorld = glm::translate(m_controlPoints.at(j)) *glm::translate(glm::vec3(0, 2, 0)) * glm::rotate(glm::radians(180.f), glm::vec3(0, 0, 1));

			korlap_program.SetUniform("world", korlapWorld);
			korlap_program.SetUniform("worldIT", glm::transpose(glm::inverse(korlapWorld)));
			korlap_program.SetUniform("Kd", glm::vec4(1, 0.3, 0.3, 1));
			korlap_program.SetUniform("MVP", m_camera.GetViewProj()*korlapWorld);
			glDrawElements(GL_TRIANGLE_FAN, 40 + 2, GL_UNSIGNED_INT, nullptr);
		}
		//FÉLGÖMB-körlap
		korlap_vao.Bind();

		korlap_program.Use();

		korlap_program.SetTexture("texImage", 0, m_textureMetal);

		korlapWorld = glm::translate(m_controlPoints.at(j)) * glm::rotate(glm::radians(180.f), glm::vec3(1, 0, 0));

		korlap_program.SetUniform("world", korlapWorld);
		korlap_program.SetUniform("worldIT", glm::transpose(glm::inverse(korlapWorld)));
		korlap_program.SetUniform("Kd", glm::vec4(1, 0.3, 0.3, 1));
		korlap_program.SetUniform("MVP", m_camera.GetViewProj()*korlapWorld);
		glDrawElements(GL_TRIANGLE_FAN, 40 + 2, GL_UNSIGNED_INT, nullptr);

		//----------------------------------------------------------------	
		//félgömb
		felgomb_vao.Bind();

		felgomb_program.Use();

		felgomb_program.SetTexture("texImage", 0, m_textureMetal);

		glm::mat4 felgombWorld = glm::translate(m_controlPoints.at(j)) * glm::rotate(glm::radians(90.f), glm::vec3(1, 0, 0)) * glm::scale(glm::vec3(-1, -1, -1));

		felgomb_program.SetUniform("world", felgombWorld);
		felgomb_program.SetUniform("worldIT", glm::transpose(glm::inverse(felgombWorld)));
		felgomb_program.SetUniform("Kd", glm::vec4(1, 0.3, 0.3, 1));
		felgomb_program.SetUniform("MVP", m_camera.GetViewProj() * felgombWorld);
		glDrawElements(GL_TRIANGLES, 3 * 2 * (N)*(M), GL_UNSIGNED_INT, nullptr);

		//-----------------------------------------------------------------------------------
		//HENGER-LÁBAK
		henger_vao.Bind();

		henger_program.Use();

		henger_program.SetTexture("texImage", 0, m_textureMetalScales);

		int r = 1.5f;

		glm::mat4 hengerWorld;

		for (int i = 0; i < 4; i++) {
			hengerWorld = glm::translate(m_controlPoints.at(j)) * glm::translate(glm::vec3(r * sin(glm::radians(90.f*i + 45.f)), 0, r * cos(glm::radians(90.f*i + 45.f)))) * glm::rotate(glm::radians(45.f * cos(glm::radians(t * 8 * ((90.f / runSpeed))))), glm::vec3(1.f, 0, 0)) * glm::translate(glm::vec3(0, -1.f, 0)) * glm::scale(glm::vec3(0.25f, 1.f, 0.25f));
			henger_program.SetUniform("world", hengerWorld);
			henger_program.SetUniform("worldIT", glm::transpose(glm::inverse(hengerWorld)));
			henger_program.SetUniform("MVP", m_camera.GetViewProj() * hengerWorld);
			henger_program.SetUniform("Kd", glm::vec4(1, 1, 1, 1));
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 2 * (float)hengerNum + 2);
		}
		//HENGER-FEJ
		henger_vao.Bind();

		henger_program.Use();

		henger_program.SetTexture("texImage", 0, m_textureMetalScales);

		hengerWorld = glm::translate(m_controlPoints.at(j)) * glm::rotate(glm::radians(90.f), glm::vec3(0, 1, 0)) * glm::translate(glm::vec3(-1, 0, 0)) *  glm::rotate(glm::radians(90.f), glm::vec3(0, 0, 1)) * glm::scale(glm::vec3(0.5f, 2.f, 0.5f)) * glm::rotate(glm::radians(t * 90.f), glm::vec3(0, 1, 0));
		henger_program.SetUniform("world", hengerWorld);
		henger_program.SetUniform("worldIT", glm::transpose(glm::inverse(hengerWorld)));
		henger_program.SetUniform("MVP", m_camera.GetViewProj() * hengerWorld);
		henger_program.SetUniform("Kd", glm::vec4(1, 1, 1, 1));
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 2 * (float)hengerNum + 2);

		//HENGER-FEJ-KORLAPOK
		korlap_vao.Bind();

		korlap_program.Use();

		korlap_program.SetTexture("texImage", 0, m_textureMetalScales);

		korlapWorld = glm::translate(m_controlPoints.at(j)) * glm::rotate(glm::radians(90.f), glm::vec3(0, 1, 0)) * glm::translate(glm::vec3(-3.f, 0, 0)) *  glm::rotate(glm::radians(90.f), glm::vec3(0, 0, 1)) * glm::scale(glm::vec3(0.25f, 1, 0.25f)) * glm::rotate(glm::radians(t * 90.f), glm::vec3(0, 1, 0));

		korlap_program.SetUniform("world", korlapWorld);
		korlap_program.SetUniform("worldIT", glm::transpose(glm::inverse(korlapWorld)));
		korlap_program.SetUniform("Kd", glm::vec4(1, 0.3, 0.3, 1));
		korlap_program.SetUniform("MVP", m_camera.GetViewProj()*korlapWorld);
		glDrawElements(GL_TRIANGLE_FAN, 40 + 2, GL_UNSIGNED_INT, nullptr);

		//2
		korlap_vao.Bind();

		korlap_program.Use();

		korlap_program.SetTexture("texImage", 0, m_textureMetalScales);

		korlapWorld = glm::translate(m_controlPoints.at(j)) * glm::rotate(glm::radians(90.f), glm::vec3(0, 1, 0)) * glm::translate(glm::vec3(-1.f, 0, 0)) *  glm::rotate(glm::radians(90.f), glm::vec3(0, 0, 1)) * glm::scale(glm::vec3(0.25f, 1, 0.25f)) * glm::rotate(glm::radians(180.f), glm::vec3(1, 0, 0)) * glm::rotate(glm::radians(t * 90.f), glm::vec3(0, 1, 0));;

		korlap_program.SetUniform("world", korlapWorld);
		korlap_program.SetUniform("worldIT", glm::transpose(glm::inverse(korlapWorld)));
		korlap_program.SetUniform("Kd", glm::vec4(1, 0.3, 0.3, 1));
		korlap_program.SetUniform("MVP", m_camera.GetViewProj()*korlapWorld);
		glDrawElements(GL_TRIANGLE_FAN, 40 + 2, GL_UNSIGNED_INT, nullptr);

		//HENGER-FAROK
		henger_vao.Bind();

		henger_program.Use();

		henger_program.SetTexture("texImage", 0, m_textureMetalScales);

		hengerWorld = glm::translate(m_controlPoints.at(j)) * glm::rotate(glm::radians(90.f), glm::vec3(0, 1.f, 0)) * glm::translate(glm::vec3(3.f, 0, 0)) *  glm::rotate(glm::radians(90.f), glm::vec3(0, 0, 1)) * glm::scale(glm::vec3(0.25f, 2.f, 0.25f));
		henger_program.SetUniform("world", hengerWorld);
		henger_program.SetUniform("worldIT", glm::transpose(glm::inverse(hengerWorld)));
		henger_program.SetUniform("MVP", m_camera.GetViewProj() * hengerWorld);
		henger_program.SetUniform("Kd", glm::vec4(1, 1, 1, 1));
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 2 * (float)hengerNum + 2);

		//HENGER-FAROK-KORLAP
		korlap_vao.Bind();

		korlap_program.Use();

		korlap_program.SetTexture("texImage", 0, m_textureMetalScales);

		korlapWorld = glm::translate(m_controlPoints.at(j)) * glm::rotate(glm::radians(90.f), glm::vec3(0, 1, 0)) * glm::translate(glm::vec3(3.f, 0, 0)) *  glm::rotate(glm::radians(90.f), glm::vec3(0, 0, 1)) * glm::scale(glm::vec3(0.125f, 1, 0.125f)) * glm::rotate(glm::radians(180.f), glm::vec3(1, 0, 0));

		korlap_program.SetUniform("world", korlapWorld);
		korlap_program.SetUniform("worldIT", glm::transpose(glm::inverse(korlapWorld)));
		korlap_program.SetUniform("Kd", glm::vec4(1, 0.3, 0.3, 1));
		korlap_program.SetUniform("MVP", m_camera.GetViewProj()*korlapWorld);
		glDrawElements(GL_TRIANGLE_FAN, 40 + 2, GL_UNSIGNED_INT, nullptr);

		//2
		korlap_vao.Bind();

		korlap_program.Use();

		korlap_program.SetTexture("texImage", 0, m_textureMetalScales);

		korlapWorld = glm::translate(m_controlPoints.at(j)) * glm::rotate(glm::radians(90.f), glm::vec3(0, 1, 0)) * glm::translate(glm::vec3(1.f, 0, 0)) *  glm::rotate(glm::radians(90.f), glm::vec3(0, 0, 1)) * glm::scale(glm::vec3(0.125f, 1, 0.125f));

		korlap_program.SetUniform("world", korlapWorld);
		korlap_program.SetUniform("worldIT", glm::transpose(glm::inverse(korlapWorld)));
		korlap_program.SetUniform("Kd", glm::vec4(1, 0.3, 0.3, 1));
		korlap_program.SetUniform("MVP", m_camera.GetViewProj()*korlapWorld);
		glDrawElements(GL_TRIANGLE_FAN, 40 + 2, GL_UNSIGNED_INT, nullptr);

	}
	//---------------------------------------------------------------
	
	// skybox

	// mentsük el az előző Z-test eredményt, azaz azt a relációt, ami alapján update-eljük a pixelt.
	GLint prevDepthFnc;
	glGetIntegerv(GL_DEPTH_FUNC, &prevDepthFnc);

	// most kisebb-egyenlőt használjunk, mert mindent kitolunk a távoli vágósíkokra
	glDepthFunc(GL_LEQUAL);

	m_vao.Bind();

	m_programSkybox.Use();
	m_programSkybox.SetUniform("MVP", m_camera.GetViewProj() * glm::translate(m_camera.GetEye()));

	// cube map textúra beállítása 0-ás mintavételezőre és annak a shaderre beállítása
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_skyboxTexture);
	glUniform1i(m_programSkybox.GetLocation("skyboxTexture"), 0);

	// az előző három sor <=> m_programSkybox.SetCubeTexture("skyboxTexture", 0, m_skyboxTexture);

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);

	// végül állítsuk vissza
	glDepthFunc(prevDepthFnc);

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
	if (ImGui::Begin("Teki"))	
	{
		ImGui::SliderInt("Teki futas", &runSpeed, 1, 10);

		ImGui::SliderFloat3("Teki futas", &(temp[0]), -10, 10);

		if (ImGui::Button("Add")) {
			m_controlPoints.push_back(temp);
		}

		if (b) {
			ImGui::Text("Moon landing was fake!");
		}
		else {
			ImGui::Text("Never trust a reptile!");
		}
		
	}
	ImGui::End(); // ...de még ha le is volt, End()-et hívnunk kell

	if (runSpeed < 8 && runSpeed > 1) {
		amp = (float)(2.f * (1.f / runSpeed));
	}
	else if (runSpeed >= 8){
		amp = 0;
	}
	else if (runSpeed <= 1) {
		b = false;
	}
	//std::cout << amp << std::endl;
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
