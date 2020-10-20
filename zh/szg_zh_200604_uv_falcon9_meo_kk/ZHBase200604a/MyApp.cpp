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
glm::vec3 GetSpherePos(float u, float v)
{
	u *= 2 * float(M_PI);
	v *= float(M_PI);
	float cu = cosf(u), su = sinf(u), cv = cosf(v), sv = sinf(v);
	return glm::vec3(cu * sv, cv, su * sv);
}
glm::vec3 GetSphereNorm(float u, float v)
{
	u *= 2 * float(M_PI);
	v *= float(M_PI);
	float cu = cosf(u), su = sinf(u), cv = cosf(v), sv = sinf(v);
	return glm::vec3(cu * sv, cv, su * sv);
}
glm::vec2 GetSphereTex(float u, float v)
{
	return glm::vec2(1 - u, 1 - v);
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
glm::vec3 GetParaboloidPos(float u, float v)
{
	u *= 2 * float(M_PI);
	v *=  1;
	v = 1 - v;
	float r = 1;
	float h = 1;
	float cu = cosf(u), su = sinf(u), cv = cosf(v), sv = sinf(v);
	return glm::vec3(r*sqrt(v/h) * cu, v, r*sqrt(v/h)*su);
}
glm::vec3 GetParaboloidNorm(float u, float v)
{
	u *= 2 * float(M_PI);
	v *= 1;
	v = 1 - v;
	float r = 1;
	float h = 1;
	float cu = cosf(u), su = sinf(u), cv = cosf(v), sv = sinf(v);
	return glm::vec3(r * sqrt(v / h) * cu, v, r * sqrt(v / h) * su);
}
glm::vec2 GetParaboloidTex(float u, float v)
{
	return glm::vec2(1 - u, 1 - v);
}
void CMyApp::InitParaboloid()
{
	//struct Vertex{ glm::vec3 position; glm::vec3 normals; glm::vec2 texture; };
	std::vector<Vertex>vertices((N + 1) * (M + 1));
	for (int i = 0; i <= N; ++i)
		for (int j = 0; j <= M; ++j)
		{
			float u = i / (float)N;
			float v = j / (float)M;

			vertices[i + j * (N + 1)].p = GetParaboloidPos(u, v);
			vertices[i + j * (N + 1)].n = GetParaboloidNorm(u, v);
			vertices[i + j * (N + 1)].t = GetParaboloidTex(u, v);
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

	m_ParaboloidVertexBuffer.BufferData(vertices);
	m_ParaboloidIndices.BufferData(indices);
	m_ParaboloidVao.Init(
		{
			{ CreateAttribute<0, glm::vec3, 0,						sizeof(Vertex)>, m_ParaboloidVertexBuffer },
			{ CreateAttribute<1, glm::vec3,	(sizeof(glm::vec3)),	sizeof(Vertex)>, m_ParaboloidVertexBuffer },
			{ CreateAttribute<2, glm::vec2, (2 * sizeof(glm::vec3)),sizeof(Vertex)>, m_ParaboloidVertexBuffer },
		},
		m_ParaboloidIndices
		);
}
glm::vec3 GetParaboloidOuterPos(float u, float v)
{
	u *= 2 * float(M_PI);
	v *= 1;
	float r = 1;
	float h = 1;
	float cu = cosf(u), su = sinf(u), cv = cosf(v), sv = sinf(v);
	return glm::vec3(r * sqrt(v / h) * cu, v, r * sqrt(v / h) * su);
}
glm::vec3 GetParaboloidOuterNorm(float u, float v)
{
	u *= 2 * float(M_PI);
	v *= 1;
	float r = 1;
	float h = 1;
	float cu = cosf(u), su = sinf(u), cv = cosf(v), sv = sinf(v);
	return glm::vec3(r * sqrt(v / h) * cu, v, r * sqrt(v / h) * su);
}
void CMyApp::InitParaboloidOuter()
{
	//struct Vertex{ glm::vec3 position; glm::vec3 normals; glm::vec2 texture; };
	std::vector<Vertex>vertices((N + 1) * (M + 1));
	for (int i = 0; i <= N; ++i)
		for (int j = 0; j <= M; ++j)
		{
			float u = i / (float)N;
			float v = j / (float)M;

			vertices[i + j * (N + 1)].p = GetParaboloidOuterPos(u, v);
			vertices[i + j * (N + 1)].n = GetParaboloidOuterNorm(u, v);
			vertices[i + j * (N + 1)].t = GetParaboloidTex(u, v);
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

	m_ParaboloidOuterVertexBuffer.BufferData(vertices);
	m_ParaboloidOuterIndices.BufferData(indices);
	m_ParaboloidOuterVao.Init(
		{
			{ CreateAttribute<0, glm::vec3, 0,						sizeof(Vertex)>, m_ParaboloidOuterVertexBuffer },
			{ CreateAttribute<1, glm::vec3,	(sizeof(glm::vec3)),	sizeof(Vertex)>, m_ParaboloidOuterVertexBuffer },
			{ CreateAttribute<2, glm::vec2, (2 * sizeof(glm::vec3)),sizeof(Vertex)>, m_ParaboloidOuterVertexBuffer },
		},
		m_ParaboloidOuterIndices
		);
}

glm::vec3 GetCylinderPos(float u, float v)
{
	u *= 2 * float(M_PI); v = 1 - v;
	return glm::vec3(cos(u), v, sin(u));
}
glm::vec3 GetCylinderNorm(float u, float v)
{
	u *= 2 * float(M_PI);
	return glm::vec3(cos(u), 0, sin(u));
}
glm::vec2 GetCylinderTex(float u, float v)
{
	return glm::vec2(u, 1 - v);
}
void CMyApp::InitCylinder()
{
	//struct Vertex{ glm::vec3 position; glm::vec3 normals; glm::vec2 texture; };
	std::vector<Vertex>vertices((N + 1) * (M + 1));
	for (int i = 0; i <= N; ++i)
		for (int j = 0; j <= M; ++j)
		{
			float u = i / (float)N;
			float v = j / (float)M;

			vertices[i + j * (N + 1)].p = GetCylinderPos(u, v);
			vertices[i + j * (N + 1)].n = GetCylinderNorm(u, v);
			vertices[i + j * (N + 1)].t = GetCylinderTex(u, v);
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

	m_CylinderVertexBuffer.BufferData(vertices);
	m_CylinderIndices.BufferData(indices);
	m_CylinderVao.Init(
		{
			{ CreateAttribute<0, glm::vec3, 0,						sizeof(Vertex)>, m_CylinderVertexBuffer },
			{ CreateAttribute<1, glm::vec3,	(sizeof(glm::vec3)),	sizeof(Vertex)>, m_CylinderVertexBuffer },
			{ CreateAttribute<2, glm::vec2, (2 * sizeof(glm::vec3)),sizeof(Vertex)>, m_CylinderVertexBuffer },
		},
		m_CylinderIndices
		);
}
glm::vec3 GetCylinderOuterPos(float u, float v)
{
	u *= 2 * float(M_PI);
	return glm::vec3(cos(u), v, sin(u));
}
void CMyApp::InitCylinderOuter()
{
	//struct Vertex{ glm::vec3 position; glm::vec3 normals; glm::vec2 texture; };
	std::vector<Vertex>vertices((N + 1) * (M + 1));
	for (int i = 0; i <= N; ++i)
		for (int j = 0; j <= M; ++j)
		{
			float u = i / (float)N;
			float v = j / (float)M;

			vertices[i + j * (N + 1)].p = GetCylinderOuterPos(u, v);
			vertices[i + j * (N + 1)].n = GetCylinderNorm(u, v);
			vertices[i + j * (N + 1)].t = GetCylinderTex(u, v);
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

	m_CylinderOuterVertexBuffer.BufferData(vertices);
	m_CylinderOuterIndices.BufferData(indices);
	m_CylinderOuterVao.Init(
		{
			{ CreateAttribute<0, glm::vec3, 0,						sizeof(Vertex)>, m_CylinderOuterVertexBuffer },
			{ CreateAttribute<1, glm::vec3,	(sizeof(glm::vec3)),	sizeof(Vertex)>, m_CylinderOuterVertexBuffer },
			{ CreateAttribute<2, glm::vec2, (2 * sizeof(glm::vec3)),sizeof(Vertex)>, m_CylinderOuterVertexBuffer },
		},
		m_CylinderOuterIndices
		);
}

void CMyApp::InitCircle()
{
	std::vector<Vertex>vertices(N + 2);
	vertices[0].p = glm::vec3(0);
	vertices[0].n = glm::vec3(0, 1, 0);
	vertices[0].t = glm::vec2(0.5);
	for (int i = 0; i <= N; ++i)
	{
		float t = -i / float(N) * 2 * M_PI;
		vertices[i + 1].p = glm::vec3(cos(t), 0, sin(t));
		vertices[i + 1].n = glm::vec3(0, 1, 0);
		vertices[i + 1].t = glm::vec2(cos(t), sin(t)) * 0.5f + 0.5f;
	}
	m_CircleVertexBuffer.BufferData(vertices);
	m_CircleVao.Init(
		{
			{ CreateAttribute<0, glm::vec3, 0,						sizeof(Vertex)>, m_CircleVertexBuffer },
			{ CreateAttribute<1, glm::vec3,	(sizeof(glm::vec3)),	sizeof(Vertex)>, m_CircleVertexBuffer },
			{ CreateAttribute<2, glm::vec2, (2 * sizeof(glm::vec3)),sizeof(Vertex)>, m_CircleVertexBuffer },
		});
}
void CMyApp::InitRocket() {
	InitParaboloid();
	InitCylinder();
	InitCircle();
	InitParaboloidOuter();
	InitCylinderOuter();
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

	TextureFromFileAttach("assets/sky_earth/xpos.png", GL_TEXTURE_CUBE_MAP_POSITIVE_X);
	TextureFromFileAttach("assets/sky_earth/xneg.png", GL_TEXTURE_CUBE_MAP_NEGATIVE_X);
	TextureFromFileAttach("assets/sky_earth/ypos.png", GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
	TextureFromFileAttach("assets/sky_earth/yneg.png", GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);
	TextureFromFileAttach("assets/sky_earth/zpos.png", GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
	TextureFromFileAttach("assets/sky_earth/zneg.png", GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);

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
	m_camera.SetSpeed(500);
	// törlési szín legyen kékes
	glClearColor(0.125f, 0.25f, 0.5f, 1.0f);

	glEnable(GL_CULL_FACE); // kapcsoljuk be a hatrafelé nező lapok eldobását
	glEnable(GL_DEPTH_TEST); // mélységi teszt bekapcsolása (takarás)

	InitRocket();
	InitShaders();
	InitSphere();
	InitSkyBox();

	// egyéb textúrák betöltése
	m_MetalTexture.FromFile("assets/Metal.jpg");
	m_FalconTexture.FromFile("assets/falcon1.jpg");
	m_Falcon2Texture.FromFile("assets/falcon1.jpg");
	m_EarthTexture.FromFile("assets/earth.jpg");
	m_DragonTexture.FromFile("assets/dragon1.jpg");
	m_Dragon2Texture.FromFile("assets/dragon2.jpg");

	// mesh betöltése
	//m_SuzanneMesh = ObjParser::parse("assets/Suzanne.obj");
	//m_SuzanneMesh->initBuffers();
	
	// kamera
	m_camera.SetProj(45.0f, 640.0f / 480.0f, 0.01f, 1000.0f);

	return true;
}

void CMyApp::Clean()
{
	glDeleteTextures(1, &m_skyboxTexture);

	//delete m_SuzanneMesh;
}

void CMyApp::Update()
{
	static Uint32 last_time = SDL_GetTicks();
	float delta_time = (SDL_GetTicks() - last_time) / 1000.0f;

	m_camera.Update(delta_time);

	last_time = SDL_GetTicks();
}

void CMyApp::DrawParaboloid(glm::mat4 trafo) {
	glm::mat4 viewProj = m_camera.GetViewProj();
	glm::mat4 paraboloidWorld = trafo* glm::rotate(float(M_PI), glm::vec3(0, 0, 1));
	m_MeshProgram.SetTexture("texImage", 0, m_MetalTexture);
	m_MeshProgram.SetUniform("MVP", viewProj *paraboloidWorld);
	m_MeshProgram.SetUniform("world", paraboloidWorld);
	m_MeshProgram.SetUniform("worldIT", glm::inverse(glm::transpose(paraboloidWorld)));
	m_ParaboloidVao.Bind();
	glDrawElements(GL_TRIANGLES, N * M * 6, GL_UNSIGNED_INT, nullptr);
	m_ParaboloidVao.Unbind();
	m_MeshProgram.SetTexture("texImage", 0, m_MetalTexture);
	m_MeshProgram.SetUniform("MVP", viewProj * paraboloidWorld);
	m_MeshProgram.SetUniform("world", paraboloidWorld);
	m_MeshProgram.SetUniform("worldIT", glm::inverse(glm::transpose(paraboloidWorld)));
	m_ParaboloidOuterVao.Bind();
	glDrawElements(GL_TRIANGLES, N * M * 6, GL_UNSIGNED_INT, nullptr);
	m_ParaboloidOuterVao.Unbind();
}
void CMyApp::DrawParaboloid2(glm::mat4 trafo) {
	glm::mat4 viewProj = m_camera.GetViewProj();
	glm::mat4 paraboloidWorld = trafo * glm::rotate(float(M_PI), glm::vec3(0, 0, 1));
	m_MeshProgram.SetTexture("texImage", 0, m_Dragon2Texture);
	m_MeshProgram.SetUniform("MVP", viewProj * paraboloidWorld);
	m_MeshProgram.SetUniform("world", paraboloidWorld);
	m_MeshProgram.SetUniform("worldIT", glm::inverse(glm::transpose(paraboloidWorld)));
	m_ParaboloidVao.Bind();
	glDrawElements(GL_TRIANGLES, N * M * 6, GL_UNSIGNED_INT, nullptr);
	m_ParaboloidVao.Unbind();
	m_MeshProgram.SetTexture("texImage", 0, m_Dragon2Texture);
	m_MeshProgram.SetUniform("MVP", viewProj * paraboloidWorld);
	m_MeshProgram.SetUniform("world", paraboloidWorld);
	m_MeshProgram.SetUniform("worldIT", glm::inverse(glm::transpose(paraboloidWorld)));
	m_ParaboloidOuterVao.Bind();
	glDrawElements(GL_TRIANGLES, N * M * 6, GL_UNSIGNED_INT, nullptr);
	m_ParaboloidOuterVao.Unbind();
}
void CMyApp::DrawCylinder(glm::mat4 trafo) {
	glm::mat4 viewProj = m_camera.GetViewProj();
	glm::mat4 hengerWorld = trafo;
	m_MeshProgram.SetTexture("texImage", 0, m_MetalTexture);
	m_MeshProgram.SetUniform("MVP", viewProj * hengerWorld);
	m_MeshProgram.SetUniform("world", hengerWorld);
	m_MeshProgram.SetUniform("worldIT", glm::inverse(glm::transpose(hengerWorld)));
	m_CylinderVao.Bind();
	glDrawElements(GL_TRIANGLES, N * M * 6, GL_UNSIGNED_INT, nullptr);
	m_CylinderVao.Unbind();
	m_MeshProgram.SetTexture("texImage", 0, m_MetalTexture);
	m_MeshProgram.SetUniform("MVP", viewProj * hengerWorld);
	m_MeshProgram.SetUniform("world", hengerWorld);
	m_MeshProgram.SetUniform("worldIT", glm::inverse(glm::transpose(hengerWorld)));
	m_CylinderOuterVao.Bind();
	glDrawElements(GL_TRIANGLES, N * M * 6, GL_UNSIGNED_INT, nullptr);
	m_CylinderOuterVao.Unbind();
}
void CMyApp::DrawCylinder2(glm::mat4 trafo) {
	glm::mat4 viewProj = m_camera.GetViewProj();
	glm::mat4 hengerWorld = trafo;
	m_MeshProgram.SetTexture("texImage", 0, m_DragonTexture);
	m_MeshProgram.SetUniform("MVP", viewProj * hengerWorld);
	m_MeshProgram.SetUniform("world", hengerWorld);
	m_MeshProgram.SetUniform("worldIT", glm::inverse(glm::transpose(hengerWorld)));
	m_CylinderVao.Bind();
	glDrawElements(GL_TRIANGLES, N * M * 6, GL_UNSIGNED_INT, nullptr);
	m_CylinderVao.Unbind();
	m_MeshProgram.SetTexture("texImage", 0, m_MetalTexture);
	m_MeshProgram.SetUniform("MVP", viewProj * hengerWorld);
	m_MeshProgram.SetUniform("world", hengerWorld);
	m_MeshProgram.SetUniform("worldIT", glm::inverse(glm::transpose(hengerWorld)));
	m_CylinderOuterVao.Bind();
	glDrawElements(GL_TRIANGLES, N * M * 6, GL_UNSIGNED_INT, nullptr);
	m_CylinderOuterVao.Unbind();
}
void CMyApp::DrawHenger(glm::mat4 trafo) {
	glm::mat4 viewProj = m_camera.GetViewProj();
	glm::mat4 hengerWorld = trafo;
	m_MeshProgram.SetTexture("texImage", 0, m_FalconTexture);
	m_MeshProgram.SetUniform("MVP", viewProj * hengerWorld);
	m_MeshProgram.SetUniform("world", hengerWorld);
	m_MeshProgram.SetUniform("worldIT", glm::inverse(glm::transpose(hengerWorld)));
	m_CylinderVao.Bind();
	glDrawElements(GL_TRIANGLES, N * M * 6, GL_UNSIGNED_INT, nullptr);
	m_CylinderVao.Unbind();
	m_MeshProgram.SetTexture("texImage", 0, m_FalconTexture);
	m_MeshProgram.SetUniform("MVP", viewProj * hengerWorld);
	m_MeshProgram.SetUniform("world", hengerWorld);
	m_MeshProgram.SetUniform("worldIT", glm::inverse(glm::transpose(hengerWorld)));
	m_CylinderOuterVao.Bind();
	glDrawElements(GL_TRIANGLES, N * M * 6, GL_UNSIGNED_INT, nullptr);
	m_CylinderOuterVao.Unbind();

	hengerWorld = trafo * glm::translate(glm::vec3(0, 1, 0));
	m_MeshProgram.SetTexture("texImage", 0, m_MetalTexture);
	m_MeshProgram.SetUniform("MVP", viewProj * hengerWorld);
	m_MeshProgram.SetUniform("world", hengerWorld);
	m_MeshProgram.SetUniform("worldIT", glm::inverse(glm::transpose(hengerWorld)));
	m_CircleVao.Bind();
	glDrawArrays(GL_TRIANGLE_FAN, 0, N + 2);
	m_CircleVao.Unbind();
	hengerWorld = trafo *glm::translate(glm::vec3(0,1,0))* glm::rotate(float(M_PI), glm::vec3(0, 0, 1));
	m_MeshProgram.SetTexture("texImage", 0, m_MetalTexture);
	m_MeshProgram.SetUniform("MVP", viewProj * hengerWorld);
	m_MeshProgram.SetUniform("world", hengerWorld);
	m_MeshProgram.SetUniform("worldIT", glm::inverse(glm::transpose(hengerWorld)));
	m_CircleVao.Bind();
	glDrawArrays(GL_TRIANGLE_FAN, 0, N + 2);
	m_CircleVao.Unbind();

	hengerWorld = trafo * glm::rotate(float(M_PI), glm::vec3(0, 0, 1));
	m_MeshProgram.SetTexture("texImage", 0, m_MetalTexture);
	m_MeshProgram.SetUniform("MVP", viewProj * hengerWorld);
	m_MeshProgram.SetUniform("world", hengerWorld);
	m_MeshProgram.SetUniform("worldIT", glm::inverse(glm::transpose(hengerWorld)));
	m_CircleVao.Bind();
	glDrawArrays(GL_TRIANGLE_FAN, 0, N + 2);
	m_CircleVao.Unbind();
	hengerWorld = trafo;
	m_MeshProgram.SetTexture("texImage", 0, m_MetalTexture);
	m_MeshProgram.SetUniform("MVP", viewProj * hengerWorld);
	m_MeshProgram.SetUniform("world", hengerWorld);
	m_MeshProgram.SetUniform("worldIT", glm::inverse(glm::transpose(hengerWorld)));
	m_CircleVao.Bind();
	glDrawArrays(GL_TRIANGLE_FAN, 0, N + 2);
	m_CircleVao.Unbind();
}
void CMyApp::DrawCircle(glm::mat4 trafo) {
	glm::mat4 viewProj = m_camera.GetViewProj();
	glm::mat4 circleWorld = trafo;
	m_MeshProgram.SetTexture("texImage", 0, m_MetalTexture);
	m_MeshProgram.SetUniform("MVP", viewProj * circleWorld);
	m_MeshProgram.SetUniform("world", circleWorld);
	m_MeshProgram.SetUniform("worldIT", glm::inverse(glm::transpose(circleWorld)));
	m_CircleVao.Bind();
	glDrawArrays(GL_TRIANGLE_FAN, 0, N + 2);
	m_CircleVao.Unbind();

	circleWorld = trafo * glm::rotate(float(M_PI), glm::vec3(0, 0, 1));
	m_MeshProgram.SetTexture("texImage", 0, m_MetalTexture);
	m_MeshProgram.SetUniform("MVP", viewProj * circleWorld);
	m_MeshProgram.SetUniform("world", circleWorld);
	m_MeshProgram.SetUniform("worldIT", glm::inverse(glm::transpose(circleWorld)));
	m_CircleVao.Bind();
	glDrawArrays(GL_TRIANGLE_FAN, 0, N + 2);
	m_CircleVao.Unbind();

}
void CMyApp::DrawHenger2(glm::mat4 trafo) {
	glm::mat4 viewProj = m_camera.GetViewProj();
	glm::mat4 hengerWorld = trafo;
	m_MeshProgram.SetTexture("texImage", 0, m_Falcon2Texture);
	m_MeshProgram.SetUniform("MVP", viewProj * hengerWorld);
	m_MeshProgram.SetUniform("world", hengerWorld);
	m_MeshProgram.SetUniform("worldIT", glm::inverse(glm::transpose(hengerWorld)));
	m_CylinderVao.Bind();
	glDrawElements(GL_TRIANGLES, N * M * 6, GL_UNSIGNED_INT, nullptr);
	m_CylinderVao.Unbind();
	m_MeshProgram.SetTexture("texImage", 0, m_Falcon2Texture);
	m_MeshProgram.SetUniform("MVP", viewProj * hengerWorld);
	m_MeshProgram.SetUniform("world", hengerWorld);
	m_MeshProgram.SetUniform("worldIT", glm::inverse(glm::transpose(hengerWorld)));
	m_CylinderOuterVao.Bind();
	glDrawElements(GL_TRIANGLES, N * M * 6, GL_UNSIGNED_INT, nullptr);
	m_CylinderOuterVao.Unbind();

	hengerWorld = trafo * glm::translate(glm::vec3(0, 1, 0));
	m_MeshProgram.SetTexture("texImage", 0, m_MetalTexture);
	m_MeshProgram.SetUniform("MVP", viewProj * hengerWorld);
	m_MeshProgram.SetUniform("world", hengerWorld);
	m_MeshProgram.SetUniform("worldIT", glm::inverse(glm::transpose(hengerWorld)));
	m_CircleVao.Bind();
	glDrawArrays(GL_TRIANGLE_FAN, 0, N + 2);
	m_CircleVao.Unbind();
	hengerWorld = trafo * glm::translate(glm::vec3(0, 1, 0)) * glm::rotate(float(M_PI), glm::vec3(0, 0, 1));
	m_MeshProgram.SetTexture("texImage", 0, m_MetalTexture);
	m_MeshProgram.SetUniform("MVP", viewProj * hengerWorld);
	m_MeshProgram.SetUniform("world", hengerWorld);
	m_MeshProgram.SetUniform("worldIT", glm::inverse(glm::transpose(hengerWorld)));
	m_CircleVao.Bind();
	glDrawArrays(GL_TRIANGLE_FAN, 0, N + 2);
	m_CircleVao.Unbind();

	hengerWorld = trafo * glm::rotate(float(M_PI), glm::vec3(0, 0, 1));
	m_MeshProgram.SetTexture("texImage", 0, m_MetalTexture);
	m_MeshProgram.SetUniform("MVP", viewProj * hengerWorld);
	m_MeshProgram.SetUniform("world", hengerWorld);
	m_MeshProgram.SetUniform("worldIT", glm::inverse(glm::transpose(hengerWorld)));
	m_CircleVao.Bind();
	glDrawArrays(GL_TRIANGLE_FAN, 0, N + 2);
	m_CircleVao.Unbind();
	hengerWorld = trafo;
	m_MeshProgram.SetTexture("texImage", 0, m_MetalTexture);
	m_MeshProgram.SetUniform("MVP", viewProj * hengerWorld);
	m_MeshProgram.SetUniform("world", hengerWorld);
	m_MeshProgram.SetUniform("worldIT", glm::inverse(glm::transpose(hengerWorld)));
	m_CircleVao.Bind();
	glDrawArrays(GL_TRIANGLE_FAN, 0, N + 2);
	m_CircleVao.Unbind();
}
void CMyApp::DrawRocket(glm::mat4 trafo){
	DrawParaboloid(trafo*glm::scale(glm::vec3(0.25, 1, 0.25)));
	const float r = 0.75f;
	float beta =2 * M_PI / 6.0f;
	for (int i = 1; i <= 8; i++) {
		float alpha = i * 2 * M_PI / 8 + beta;
		DrawParaboloid(trafo* glm::translate(glm::vec3(r * cos(alpha), 0, r * sin(alpha))) * glm::scale(glm::vec3(0.25, 1, 0.25)));
	}
	DrawHenger(trafo*glm::scale(glm::vec3(1.85,43,1.85)));
	DrawCylinder(glm::translate(glm::vec3(0.0, 43.0, 0.0))* glm::scale(glm::vec3(1.85, 6, 1.85)));
	DrawParaboloid(trafo * glm::translate(glm::vec3(0, 49, 0)) *glm::scale(glm::vec3(0.75, 3.5, 0.75)));
	DrawHenger2(trafo * glm::translate(glm::vec3(0, 49, 0)) * glm::scale(glm::vec3(1.85, 7, 1.85)));
	DrawParaboloid2(trafo * glm::translate(glm::vec3(0, 64, 0)) * glm::scale(glm::vec3(1.85, 4, 1.85)));
	DrawCircle(trafo * glm::translate(glm::vec3(0, 60, 0)) * glm::scale(glm::vec3(1.85, 1, 1.85)));
	DrawCylinder2(trafo * glm::translate(glm::vec3(0, 56, 0)) * glm::scale(glm::vec3(1.85, 4, 1.85)));

}

void CMyApp::Render()
{
	// töröljük a framebuffert (GL_COLOR_BUFFER_BIT) és a mélységi Z buffert (GL_DEPTH_BUFFER_BIT)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 viewProj = m_camera.GetViewProj();

	// tengelyek
	m_AxesProgram.Use();
	m_AxesProgram.SetUniform("mvp", viewProj);
	glDrawArrays(GL_LINES, 0, 6);
	
	m_MeshProgram.Use();
	// Suzanne
	glm::mat4 world = glm::translate(glm::vec3(-3, 0, 0));
	/*m_MeshProgram.SetTexture("texImage", 0, m_MarronTexture);
	m_MeshProgram.SetUniform("MVP", viewProj * world);
	m_MeshProgram.SetUniform("world", world);
	m_MeshProgram.SetUniform("worldIT", glm::inverse(glm::transpose(world)));
	m_SuzanneMesh->draw();
	*/
	// gömb
	float alpha = SDL_GetTicks() / 1000.0f / 2;
	world =glm::scale(glm::vec3(1000,1000,1000));
	m_MeshProgram.SetTexture("texImage", 0, m_EarthTexture);
	m_MeshProgram.SetUniform("MVP", viewProj * world);
	m_MeshProgram.SetUniform("world", world);
	m_MeshProgram.SetUniform("worldIT", glm::inverse(glm::transpose(world)));
	m_SphereVao.Bind();
	glDrawElements(GL_TRIANGLES, N * M * 6, GL_UNSIGNED_INT, nullptr);
	m_SphereVao.Unbind();

	DrawRocket(glm::mat4(1));
	m_MeshProgram.Unuse();
	// skybox
	GLint prevDepthFnc;
	glGetIntegerv(GL_DEPTH_FUNC, &prevDepthFnc);
	glDepthFunc(GL_LEQUAL);

	m_SkyboxProgram.Use();
	m_SkyboxProgram.SetUniform("MVP", viewProj * glm::translate( m_camera.GetEye()) );
	m_SkyboxProgram.SetCubeTexture("skyboxTexture", 0, m_skyboxTexture);
	m_SkyboxVao.Bind();
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
	m_SkyboxVao.Unbind();
	m_SkyboxProgram.Unuse();

	glDepthFunc(prevDepthFnc);
	
	// ImGui demo ablak
	//ImGui::ShowTestWindow();

	/*ImGui::SetNextWindowPos(ImVec2(300, 400), ImGuiSetCond_FirstUseEver);
	if (ImGui::Begin("Tesztablak"))
	{
		ImGui::Text("Tesztszoveg");
	}
	ImGui::End();
	*/
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
