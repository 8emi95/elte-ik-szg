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
}

CMyApp::~CMyApp(void)
{
	std::cout << "dtor!\n";
}

void CMyApp::InitCube()
{
	//struct Vertex{ glm::vec3 position; glm::vec3 normals; glm::vec2 texture; };
	std::vector<Vertex>vertices;
	vertices.reserve(6 * 4);
	
	//front									 
	vertices.push_back({ glm::vec3(-0.5, -0.5, +0.5), glm::vec3(0, 0, 1), glm::vec2(0, 0) });
	vertices.push_back({ glm::vec3(+0.5, -0.5, +0.5), glm::vec3(0, 0, 1), glm::vec2(1, 0) });
	vertices.push_back({ glm::vec3(-0.5, +0.5, +0.5), glm::vec3(0, 0, 1), glm::vec2(0, 1) });
	vertices.push_back({ glm::vec3(+0.5, +0.5, +0.5), glm::vec3(0, 0, 1), glm::vec2(1, 1) });
	//back
	vertices.push_back({ glm::vec3(+0.5, -0.5, -0.5), glm::vec3(0, 0, -1), glm::vec2(0, 0) });
	vertices.push_back({ glm::vec3(-0.5, -0.5, -0.5), glm::vec3(0, 0, -1), glm::vec2(1, 0) });
	vertices.push_back({ glm::vec3(+0.5, +0.5, -0.5), glm::vec3(0, 0, -1), glm::vec2(0, 1) });
	vertices.push_back({ glm::vec3(-0.5, +0.5, -0.5), glm::vec3(0, 0, -1), glm::vec2(1, 1) });
	//right									 
	vertices.push_back({ glm::vec3(+0.5, -0.5, +0.5), glm::vec3(1, 0, 0), glm::vec2(0, 0) });
	vertices.push_back({ glm::vec3(+0.5, -0.5, -0.5), glm::vec3(1, 0, 0), glm::vec2(1, 0) });
	vertices.push_back({ glm::vec3(+0.5, +0.5, +0.5), glm::vec3(1, 0, 0), glm::vec2(0, 1) });
	vertices.push_back({ glm::vec3(+0.5, +0.5, -0.5), glm::vec3(1, 0, 0), glm::vec2(1, 1) });
	//left									 
	vertices.push_back({ glm::vec3(-0.5, -0.5, -0.5), glm::vec3(-1, 0, 0), glm::vec2(0, 0) });
	vertices.push_back({ glm::vec3(-0.5, -0.5, +0.5), glm::vec3(-1, 0, 0), glm::vec2(1, 0) });
	vertices.push_back({ glm::vec3(-0.5, +0.5, -0.5), glm::vec3(-1, 0, 0), glm::vec2(0, 1) });
	vertices.push_back({ glm::vec3(-0.5, +0.5, +0.5), glm::vec3(-1, 0, 0), glm::vec2(1, 1) });
	//top									 
	vertices.push_back({ glm::vec3(-0.5, +0.5, +0.5), glm::vec3(0, 1, 0), glm::vec2(0, 0) });
	vertices.push_back({ glm::vec3(+0.5, +0.5, +0.5), glm::vec3(0, 1, 0), glm::vec2(1, 0) });
	vertices.push_back({ glm::vec3(-0.5, +0.5, -0.5), glm::vec3(0, 1, 0), glm::vec2(0, 1) });
	vertices.push_back({ glm::vec3(+0.5, +0.5, -0.5), glm::vec3(0, 1, 0), glm::vec2(1, 1) });
	//bottom								 
	vertices.push_back({ glm::vec3(-0.5, -0.5, -0.5), glm::vec3(0, -1, 0), glm::vec2(0, 0) });
	vertices.push_back({ glm::vec3(+0.5, -0.5, -0.5), glm::vec3(0, -1, 0), glm::vec2(1, 0) });
	vertices.push_back({ glm::vec3(-0.5, -0.5, +0.5), glm::vec3(0, -1, 0), glm::vec2(0, 1) });
	vertices.push_back({ glm::vec3(+0.5, -0.5, +0.5), glm::vec3(0, -1, 0), glm::vec2(1, 1) });

	std::vector<GLuint> indices(36);
	size_t index = 0;
	for (int i = 0; i < 6 * 4; i += 4)
	{
		indices[index + 0] = i + 0;
		indices[index + 1] = i + 1;
		indices[index + 2] = i + 2;
		indices[index + 3] = i + 1;
		indices[index + 4] = i + 3;
		indices[index + 5] = i + 2;
		index += 6;
	}

	//
	// geometria definiálása (std::vector<...>) és GPU pufferekbe való feltöltése BufferData-val
	//

	// vertexek pozíciói:
	/*
	Az m_CubeVertexBuffer konstruktora már létrehozott egy GPU puffer azonosítót és a most következő BufferData hívás ezt
	1. bind-olni fogja GL_ARRAY_BUFFER target-re (hiszen m_CubeVertexBuffer típusa ArrayBuffer) és
	2. glBufferData segítségével áttölti a GPU-ra az argumentumban adott tároló értékeit

	*/

	m_CubeVertexBuffer.BufferData(vertices);

	// és a primitíveket alkotó csúcspontok indexei (az előző tömbökből) - triangle list-el való kirajzolásra felkészülve
	m_CubeIndices.BufferData(indices);

	// geometria VAO-ban való regisztrálása
	m_CubeVao.Init(
		{
			// 0-ás attribútum "lényegében" glm::vec3-ak sorozata és az adatok az m_CubeVertexBuffer GPU pufferben vannak
			//{ CreateAttribute<	0,						// attribútum: 0
			//						glm::vec3,				// CPU oldali adattípus amit a 0-ás attribútum meghatározására használtunk <- az eljárás a glm::vec3-ból kikövetkezteti, hogy 3 darab float-ból áll a 0-ás attribútum
			//						0,						// offset: az attribútum tároló elejétől vett offset-je, byte-ban
			//						sizeof(Vertex)			// stride: a következő csúcspont ezen attribútuma hány byte-ra van az aktuálistól
			//					>, m_CubeVertexBuffer },
			{ CreateAttribute<0, glm::vec3, 0,						 sizeof(Vertex)>, m_CubeVertexBuffer },
			{ CreateAttribute<1, glm::vec3, (sizeof(glm::vec3)),	 sizeof(Vertex)>, m_CubeVertexBuffer },
			{ CreateAttribute<2, glm::vec2, (2 * sizeof(glm::vec3)), sizeof(Vertex)>, m_CubeVertexBuffer },
		},
		m_CubeIndices
	);
}

glm::vec3 GetTorusPos(float u, float v)
{
	u *= 2 * float(M_PI);
	v *= 2 * float(M_PI);
	float cu = cosf(u), su = sinf(u), cv = cosf(v), sv = sinf(v);
	float r = 2;
	float R = 4;
	return glm::vec3((R + r * cu) * cv, r * su, (R + r * cu) * sv);
}
glm::vec3 GetTorusNorm(float u, float v)
{
	u *= 2 * float(M_PI);
	v *= 2 * float(M_PI);
	float cu = cosf(u), su = sinf(u), cv = cosf(v), sv = sinf(v);
	return glm::vec3(cu * cv, su, cu * sv);
	//return glm::vec3(cu * sv, cv, su * sv);
}
glm::vec2 GetTorusTex(float u, float v)
{
	return glm::vec2(u, v);
}
void CMyApp::InitTorus()
{
	//struct Vertex{ glm::vec3 position; glm::vec3 normals; glm::vec2 texture; };
	std::vector<Vertex>vertices((N + 1) * (M + 1));
	for (int i = 0; i <= N; ++i)
		for (int j = 0; j <= M; ++j)
		{
			float u = i / (float)N;
			float v = j / (float)M;

			vertices[i + j * (N + 1)].p = GetTorusPos(u, v);
			vertices[i + j * (N + 1)].n = GetTorusNorm(u, v);
			vertices[i + j * (N + 1)].t = GetTorusTex(u, v);
		}

	std::vector<GLuint> indices(3 * 2 * (N) * (M));
	for (int i = 0; i < N; ++i)
		for (int j = 0; j < M; ++j)
		{
			indices[6*i + j*3*2*(N) + 0] = (i)		+ (j)*	(N+1);
			indices[6*i + j*3*2*(N) + 1] = (i+1)	+ (j)*	(N+1);
			indices[6*i + j*3*2*(N) + 2] = (i)		+ (j+1)*(N+1);
			indices[6*i + j*3*2*(N) + 3] = (i+1)	+ (j)*	(N+1);
			indices[6*i + j*3*2*(N) + 4] = (i+1)	+ (j+1)*(N+1);
			indices[6*i + j*3*2*(N) + 5] = (i)		+ (j+1)*(N+1);
		}

	m_TorusVertexBuffer.BufferData(vertices);
	m_TorusIndices.BufferData(indices);
	m_TorusVao.Init(
		{
			{ CreateAttribute<0, glm::vec3, 0,						sizeof(Vertex)>, m_TorusVertexBuffer },
			{ CreateAttribute<1, glm::vec3,	(sizeof(glm::vec3)),	sizeof(Vertex)>, m_TorusVertexBuffer },
			{ CreateAttribute<2, glm::vec2, (2 * sizeof(glm::vec3)),sizeof(Vertex)>, m_TorusVertexBuffer },
		},
		m_TorusIndices
		);
}


// sphere
glm::vec3 GetSpherePos(float u, float v)
{
	u *= 2 * float(M_PI);
	v *= float(M_PI);
	float cu = cosf(u), su = sinf(u), cv = cosf(v), sv = sinf(v);
	float r = 1;
	return glm::vec3(r * cu * sv, r * cv, r * su * sv);
}
glm::vec3 GetSphereNorm(float u, float v)
{
	/*
	// tórusz
	u *= 2 * float(M_PI);
	v *= 2 * float(M_PI);
	float cu = cosf(u), su = sinf(u), cv = cosf(v), sv = sinf(v);
	return glm::vec3(cu * cv, su, cu * sv);
	//return glm::vec3(cu * sv, cv, su * sv);
	*/

	// Képlettel
	u *= float(2 * M_PI);
	v *= float(M_PI);
	float cu = cosf(u), su = sinf(u), cv = cosf(v), sv = sinf(v);
	return glm::vec3(cu * sv, cv, su * sv);

	// Numerikusan (nem kell ismerni a képletet, elég a pozícióét)
	/*
	glm::vec3 du = GetPos(u+0.01, v)-GetPos(u-0.01, v);
	glm::vec3 dv = GetPos(u, v+0.01)-GetPos(u, v-0.01);

	return glm::normalize(glm::cross(du, dv));*/
}
glm::vec2 GetSphereTex(float u, float v)
{
	return glm::vec2(u, v);
}
void CMyApp::InitSphere()
{
	//struct Vertex{ glm::vec3 position; glm::vec3 normals; glm::vec2 texture; };
	std::vector<Vertex>vertices((N + 1) * (M + 1));
	for (int i = 0; i <= N; ++i)
		for (int j = 0; j <= M; ++j)
		{
			float u = i / (float)N;
			float v = j / (float)M;

			vertices[i + j * (N + 1)].p = GetSpherePos(u, v);
			vertices[i + j * (N + 1)].n = GetSphereNorm(u, v);
			vertices[i + j * (N + 1)].t = GetSphereTex(u, v);
		}

	std::vector<GLuint> indices(3 * 2 * (N) * (M));
	for (int i = 0; i < N; ++i)
		for (int j = 0; j < M; ++j)
		{
			indices[6 * i + j * 3 * 2 * (N)+0] = (i)+(j) * (N + 1);
			indices[6 * i + j * 3 * 2 * (N)+1] = (i + 1) + (j) * (N + 1);
			indices[6 * i + j * 3 * 2 * (N)+2] = (i)+(j + 1) * (N + 1);
			indices[6 * i + j * 3 * 2 * (N)+3] = (i + 1) + (j) * (N + 1);
			indices[6 * i + j * 3 * 2 * (N)+4] = (i + 1) + (j + 1) * (N + 1);
			indices[6 * i + j * 3 * 2 * (N)+5] = (i)+(j + 1) * (N + 1);
		}

	m_SphereVertexBuffer.BufferData(vertices);
	m_SphereIndices.BufferData(indices);
	m_SphereVao.Init(
		{
			{ CreateAttribute<0, glm::vec3, 0,						sizeof(Vertex)>, m_SphereVertexBuffer },
			{ CreateAttribute<1, glm::vec3,	(sizeof(glm::vec3)),	sizeof(Vertex)>, m_SphereVertexBuffer },
			{ CreateAttribute<2, glm::vec2, (2 * sizeof(glm::vec3)),sizeof(Vertex)>, m_SphereVertexBuffer },
		},
		m_SphereIndices
		);
}



void CMyApp::InitSkyBox()
{
	m_SkyboxPos.BufferData(
		std::vector<glm::vec3>{
		// hátsó lap
		glm::vec3(-1, -1, -1),
		glm::vec3( 1, -1, -1),
		glm::vec3( 1,  1, -1),
		glm::vec3(-1,  1, -1),
		// elülső lap
		glm::vec3(-1, -1,  1),
		glm::vec3( 1, -1,  1),
		glm::vec3( 1,  1,  1),
		glm::vec3(-1,  1,  1),
	}
	);

	m_SkyboxIndices.BufferData(
		std::vector<GLuint>{
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

	// geometria VAO-ban való regisztrálása
	m_SkyboxVao.Init(
		{
			{ CreateAttribute<0, glm::vec3, 0, sizeof(glm::vec3)>, m_SkyboxPos },
		}, m_SkyboxIndices
	);

	// skybox texture
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	glGenTextures(1, &m_skyboxTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_skyboxTexture);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	// TextureFromFileAttach("assets/xpos.png", GL_TEXTURE_CUBE_MAP_POSITIVE_X);
	// TextureFromFileAttach("assets/xneg.png", GL_TEXTURE_CUBE_MAP_NEGATIVE_X);
	// TextureFromFileAttach("assets/ypos.png", GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
	// TextureFromFileAttach("assets/yneg.png", GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);
	// TextureFromFileAttach("assets/zpos.png", GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
	// TextureFromFileAttach("assets/zneg.png", GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);

	TextureFromFileAttach("assets/SkyBox/bkg1_right.png", GL_TEXTURE_CUBE_MAP_POSITIVE_X);
	TextureFromFileAttach("assets/SkyBox/bkg1_left.png", GL_TEXTURE_CUBE_MAP_NEGATIVE_X);
	TextureFromFileAttach("assets/SkyBox/bkg1_top.png", GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
	TextureFromFileAttach("assets/SkyBox/bkg1_bot.png", GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);
	TextureFromFileAttach("assets/SkyBox/bkg1_front.png", GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
	TextureFromFileAttach("assets/SkyBox/bkg1_back.png", GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

}

void CMyApp::InitShaders()
{
	// a shadereket tároló program létrehozása az OpenGL-hez hasonló módon:
	m_MeshProgram.AttachShaders({
		{ GL_VERTEX_SHADER, "myVert.vert"},
		{ GL_FRAGMENT_SHADER, "myFrag.frag"}
		});

	// attributomok osszerendelese a VAO es shader kozt
	m_MeshProgram.BindAttribLocations({
		{ 0, "vs_in_pos" },				// VAO 0-as csatorna menjen a vs_in_pos-ba
		{ 1, "vs_in_norm" },			// VAO 1-es csatorna menjen a vs_in_norm-ba
		{ 2, "vs_in_tex" },				// VAO 2-es csatorna menjen a vs_in_tex-be
		});

	m_MeshProgram.LinkProgram();

	// shader program rövid létrehozása, egyetlen függvényhívással a fenti három:
	m_SkyboxProgram.Init(
		{
			{ GL_VERTEX_SHADER, "skybox.vert" },
			{ GL_FRAGMENT_SHADER, "skybox.frag" }
		},
		{
			{ 0, "vs_in_pos" },				// VAO 0-as csatorna menjen a vs_in_pos-ba
		}
		);

	// shader program létrehozása még rövidebben:
	// { GL_VERTEX_SHADER, "shader.vert" } helyett "shader.vert"_vs
	m_AxesProgram.Init({ "axes.vert"_vs, "axes.frag"_fs });
}

bool CMyApp::Init()
{
	// törlési szín legyen kékes
	glClearColor(0.125f, 0.25f, 0.5f, 1.0f);

	glEnable(GL_CULL_FACE); // kapcsoljuk be a hatrafelé nező lapok eldobását
	glEnable(GL_DEPTH_TEST); // mélységi teszt bekapcsolása (takarás)

	InitShaders();
	InitCube();
	InitTorus();
	InitSkyBox();

	// sphere
	InitSphere();

	// egyéb textúrák betöltése
	m_WoodTexture.FromFile("assets/wood.jpg");
	m_MarronTexture.FromFile("assets/marron.jpg");



	// sun
	m_sunTexture.FromFile("assets/sun.jpg");
	// planet
	m_planetTexture.FromFile("assets/planet_gas.jpg");
	// moon
	m_moonTexture.FromFile("assets/moon.jpg");
	// mesh
	m_meshTexture.FromFile("assets/metal_panel.jpg");



	// mesh betöltése
	m_SuzanneMesh = ObjParser::parse("assets/Suzanne.obj");
	m_SuzanneMesh->initBuffers();



	// death star, destroyer, nebulon
	//m_deathStarMesh = ObjParser::parse("assets/death_star.obj");
	//m_deathStarMesh->initBuffers();
	m_destroyerMesh = ObjParser::parse("assets/destroyer.obj");
	m_destroyerMesh->initBuffers();
	m_nebulonMesh = ObjParser::parse("assets/nebulon.obj");
	m_nebulonMesh->initBuffers();
	
	m_deathStar = ObjParser::parse("assets/death_star.obj");
	m_deathStar->initBuffers();


	// kamera
	m_camera.SetProj(45.0f, 640.0f / 480.0f, 0.01f, 1000.0f);

	return true;
}

void CMyApp::Clean()
{
	glDeleteTextures(1, &m_skyboxTexture);

	delete m_SuzanneMesh;

	// death star, destroyer, nebulon
	//delete m_deathStarMesh;
	delete m_deathStar;
	delete m_destroyerMesh;
	delete m_nebulonMesh;
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
	// töröljük a framebuffert (GL_COLOR_BUFFER_BIT) és a mélységi Z buffert (GL_DEPTH_BUFFER_BIT)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



	glm::mat4 viewProj = m_camera.GetViewProj();

	m_MeshProgram.Use();


	glm::vec3 eye = m_camera.GetEye();
	m_MeshProgram.SetUniform("eye_pos", eye);

	/*
	// tengelyek
	m_AxesProgram.Use();
	m_AxesProgram.SetUniform("mvp", viewProj);
	glDrawArrays(GL_LINES, 0, 6);

	// Suzanne
	glm::mat4 world = glm::translate(glm::vec3(-3, 0, 0));
	m_MeshProgram.SetTexture("texImage", 0, m_MarronTexture);
	m_MeshProgram.SetUniform("MVP", viewProj * world);
	m_MeshProgram.SetUniform("world", world);
	m_MeshProgram.SetUniform("worldIT", glm::inverse(glm::transpose(world)));
	m_SuzanneMesh->draw();
	*/


	// deathStar
	glm::mat4 deathStarWorld = glm::translate(glm::vec3(30, 0, 10)) * glm::scale(glm::vec3(1, 1, 1));
	m_MeshProgram.SetTexture("texImage", 0, m_meshTexture);
	m_MeshProgram.SetUniform("MVP", viewProj * deathStarWorld);
	m_MeshProgram.SetUniform("deathStarWorld", deathStarWorld);
	m_MeshProgram.SetUniform("deathStarWorldIT", glm::inverse(glm::transpose(deathStarWorld)));
	m_deathStar->draw();

	// destroyer
	for (int i = 0; i < 15; ++i)
	{
		glm::mat4 destroyerWorld = glm::translate(glm::vec3(30 + i / 10, 0, 0)) * glm::scale(glm::vec3(0.2f, 0.2f, 0.2f));
		m_MeshProgram.SetTexture("texImage", 0, m_meshTexture);
		m_MeshProgram.SetUniform("MVP", viewProj * destroyerWorld);
		m_MeshProgram.SetUniform("destroyerWorld", destroyerWorld);
		m_MeshProgram.SetUniform("destroyerWorldIT", glm::inverse(glm::transpose(destroyerWorld)));
		m_destroyerMesh->draw();
	}

	// nebulon

	for (int i = 0; i < 20; ++i)
	{
		glm::mat4 nebulonWorld = glm::translate(glm::vec3(30 + i / 10, 5, 0)) * glm::scale(glm::vec3(0.1f, 0.1f, 0.1f));
		m_MeshProgram.SetTexture("texImage", 0, m_meshTexture);
		m_MeshProgram.SetUniform("MVP", viewProj * nebulonWorld);
		m_MeshProgram.SetUniform("nebulonWorld", nebulonWorld);
		m_MeshProgram.SetUniform("nebulonWorldIT", glm::inverse(glm::transpose(nebulonWorld)));
		m_nebulonMesh->draw();

	}


	/*
	// kocka
	world = glm::translate(glm::vec3(3, 0, 0));
	m_MeshProgram.SetTexture("texImage", 0, m_WoodTexture);
	m_MeshProgram.SetUniform("MVP", viewProj * world);
	m_MeshProgram.SetUniform("world", world);
	m_MeshProgram.SetUniform("worldIT", glm::inverse(glm::transpose(world)));
	m_CubeVao.Bind();
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
	m_CubeVao.Unbind();



	// tórusz
	float alpha = SDL_GetTicks() / 1000.0f / 2;
	world = glm::scale(glm::vec3(0.25f, 0.25f, 0.25f));
	m_MeshProgram.SetUniform("MVP", viewProj * world);
	m_MeshProgram.SetUniform("world", world);
	m_MeshProgram.SetUniform("worldIT", glm::inverse(glm::transpose(world)));
	m_TorusVao.Bind();
	glDrawElements(GL_TRIANGLES, N * M * 6, GL_UNSIGNED_INT, nullptr);
	m_TorusVao.Unbind();
	*/


	float time = SDL_GetTicks() / 1000.0f;

	// sun
	m_MeshProgram.SetTexture("texImage", 0, m_sunTexture);
	glm::mat4 sunWorld =
		glm::translate(glm::vec3(0, 0, 0))
		*
		glm::scale(glm::vec3(10, 10, 10))
		*
		glm::rotate<float>(time, glm::vec3(0, 1, 0))
		;
	m_MeshProgram.SetUniform("MVP", viewProj * sunWorld);
	m_MeshProgram.SetUniform("world", sunWorld);
	m_MeshProgram.SetUniform("worldIT", glm::inverse(glm::transpose(sunWorld)));
	m_SphereVao.Bind();
	glDrawElements(GL_TRIANGLES, N * M * 6, GL_UNSIGNED_INT, nullptr);
	m_SphereVao.Unbind();

	// planet
	m_MeshProgram.SetTexture("texImage", 0, m_planetTexture);
	glm::mat4 planetWorld =
		glm::rotate<float>(2 * M_PI * time / 60, glm::vec3(0, 1, 0))
		*
		glm::translate(glm::vec3(50, 0, 0))
		*
		glm::scale(glm::vec3(1, 1, 1))
		*
		glm::rotate<float>(time, glm::vec3(0, 1, 0))
		;
	m_MeshProgram.SetUniform("MVP", viewProj * planetWorld);
	m_MeshProgram.SetUniform("world", planetWorld);
	m_MeshProgram.SetUniform("worldIT", glm::inverse(glm::transpose(planetWorld)));
	m_SphereVao.Bind();
	glDrawElements(GL_TRIANGLES, N * M * 6, GL_UNSIGNED_INT, nullptr);
	m_SphereVao.Unbind();

	// moon
	m_MeshProgram.SetTexture("texImage", 0, m_moonTexture);
	glm::mat4 moonWorld =
		glm::rotate<float>(2 * M_PI * time / 60, glm::vec3(0, 1, 0))
		*
		glm::translate(glm::vec3(50, 0, 0))
		*
		glm::rotate<float>(2 * M_PI * time / 10, glm::vec3(0, 1, 0))
		*
		glm::translate(glm::vec3(2, 0, 0))
		*
		glm::scale(glm::vec3(0.1f, 0.1f, 0.1f))
		*
		glm::rotate<float>(time, glm::vec3(0, 1, 0))
		;
	m_MeshProgram.SetUniform("MVP", viewProj * moonWorld);
	m_MeshProgram.SetUniform("world", moonWorld);
	m_MeshProgram.SetUniform("worldIT", glm::inverse(glm::transpose(moonWorld)));
	m_SphereVao.Bind();
	glDrawElements(GL_TRIANGLES, N * M * 6, GL_UNSIGNED_INT, nullptr);
	m_SphereVao.Unbind();


	m_MeshProgram.Unuse();



	// skybox
	GLint prevDepthFnc;
	glGetIntegerv(GL_DEPTH_FUNC, &prevDepthFnc);
	glDepthFunc(GL_LEQUAL);



	m_SkyboxProgram.Use();
	m_SkyboxProgram.SetUniform("MVP", viewProj * glm::translate(m_camera.GetEye()));
	m_SkyboxProgram.SetCubeTexture("skyboxTexture", 0, m_skyboxTexture);
	m_SkyboxVao.Bind();
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
	m_SkyboxVao.Unbind();
	m_SkyboxProgram.Unuse();



	glDepthFunc(prevDepthFnc);

	// ImGui demo ablak
	//ImGui::ShowTestWindow();



	ImGui::SetNextWindowPos(ImVec2(300, 400), ImGuiSetCond_FirstUseEver);
	if (ImGui::Begin("Tesztablak"))
	{
		ImGui::Text("elpusztitott destroyer-ek: %i", numDestroyer);
		ImGui::Text("elpusztitott nebulon-ok: %i", numNebulon);
	}
	ImGui::End();
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
