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
	m_mesh_suzanne = nullptr;
}

CMyApp::~CMyApp(void)
{
	std::cout << "dtor!\n";
}

void CMyApp::InitCube()
{
	//struct Vertex{ glm::vec3 position; glm::vec3 normals; glm::vec2 texture; };
	std::vector<Vertex>vertices;
	
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

	std::vector<int> indices(36);
	int index = 0;
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
	// geometria defini�l�sa (std::vector<...>) �s GPU pufferekbe val� felt�lt�se BufferData-val
	//

	// vertexek poz�ci�i:
	/*
	Az m_CubeVertexBuffer konstruktora m�r l�trehozott egy GPU puffer azonos�t�t �s a most k�vetkez� BufferData h�v�s ezt
	1. bind-olni fogja GL_ARRAY_BUFFER target-re (hiszen m_CubeVertexBuffer t�pusa ArrayBuffer) �s
	2. glBufferData seg�ts�g�vel �tt�lti a GPU-ra az argumentumban adott t�rol� �rt�keit

	*/

	m_CubeVertexBuffer.BufferData(vertices);

	// �s a primit�veket alkot� cs�cspontok indexei (az el�z� t�mb�kb�l) - triangle list-el val� kirajzol�sra felk�sz�lve
	m_CubeIndices.BufferData(indices);

	// geometria VAO-ban val� regisztr�l�sa
	m_CubeVao.Init(
		{
			// 0-�s attrib�tum "l�nyeg�ben" glm::vec3-ak sorozata �s az adatok az m_CubeVertexBuffer GPU pufferben vannak
			{ CreateAttribute<		0,						// attrib�tum: 0
									glm::vec3,				// CPU oldali adatt�pus amit a 0-�s attrib�tum meghat�roz�s�ra haszn�ltunk <- az elj�r�s a glm::vec3-b�l kik�vetkezteti, hogy 3 darab float-b�l �ll a 0-�s attrib�tum
									0,						// offset: az attrib�tum t�rol� elej�t�l vett offset-je, byte-ban
									sizeof(Vertex)			// stride: a k�vetkez� cs�cspont ezen attrib�tuma h�ny byte-ra van az aktu�list�l
								>, m_CubeVertexBuffer },
			{ CreateAttribute<1, glm::vec3, (sizeof(glm::vec3)), sizeof(Vertex)>, m_CubeVertexBuffer },
			{ CreateAttribute<2, glm::vec2, (2 * sizeof(glm::vec3)), sizeof(Vertex)>, m_CubeVertexBuffer },
		},
		m_CubeIndices
	);
}

void CMyApp::InitErdo()
{
	//struct Vertex{ glm::vec3 position; glm::vec3 normals; glm::vec2 texture; };
	std::vector<Vertex>vertices;

	//front									 
	vertices.push_back({ glm::vec3(-0.5, -0.5, +0.5), glm::vec3(0, 0, -1), glm::vec2(0, 0) });
	vertices.push_back({ glm::vec3(+0.5, -0.5, +0.5), glm::vec3(0, 0, -1), glm::vec2(1, 0) });
	vertices.push_back({ glm::vec3(-0.5, +0.5, +0.5), glm::vec3(0, 0, -1), glm::vec2(0, 1) });
	vertices.push_back({ glm::vec3(+0.5, +0.5, +0.5), glm::vec3(0, 0, -1), glm::vec2(1, 1) });
	//back
	vertices.push_back({ glm::vec3(+0.5, -0.5, -0.5), glm::vec3(0, 0, 1), glm::vec2(0, 0) });
	vertices.push_back({ glm::vec3(-0.5, -0.5, -0.5), glm::vec3(0, 0, 1), glm::vec2(1, 0) });
	vertices.push_back({ glm::vec3(+0.5, +0.5, -0.5), glm::vec3(0, 0, 1), glm::vec2(0, 1) });
	vertices.push_back({ glm::vec3(-0.5, +0.5, -0.5), glm::vec3(0, 0, 1), glm::vec2(1, 1) });
	//right									 
	vertices.push_back({ glm::vec3(+0.5, -0.5, +0.5), glm::vec3(-1, 0, 0), glm::vec2(0, 0) });
	vertices.push_back({ glm::vec3(+0.5, -0.5, -0.5), glm::vec3(-1, 0, 0), glm::vec2(1, 0) });
	vertices.push_back({ glm::vec3(+0.5, +0.5, +0.5), glm::vec3(-1, 0, 0), glm::vec2(0, 1) });
	vertices.push_back({ glm::vec3(+0.5, +0.5, -0.5), glm::vec3(-1, 0, 0), glm::vec2(1, 1) });
	//left									 
	vertices.push_back({ glm::vec3(-0.5, -0.5, -0.5), glm::vec3(1, 0, 0), glm::vec2(0, 0) });
	vertices.push_back({ glm::vec3(-0.5, -0.5, +0.5), glm::vec3(1, 0, 0), glm::vec2(1, 0) });
	vertices.push_back({ glm::vec3(-0.5, +0.5, -0.5), glm::vec3(1, 0, 0), glm::vec2(0, 1) });
	vertices.push_back({ glm::vec3(-0.5, +0.5, +0.5), glm::vec3(1, 0, 0), glm::vec2(1, 1) });

	std::vector<int> indices(4*6);
	int index = 0;
	for (int i = 0; i < 4 * 4; i += 4)
	{
		indices[index + 0] = i + 0;
		indices[index + 1] = i + 2;
		indices[index + 2] = i + 1;

		indices[index + 3] = i + 1;
		indices[index + 4] = i + 2;
		indices[index + 5] = i + 3;
		index += 6;
	}

	//
	// geometria defini�l�sa (std::vector<...>) �s GPU pufferekbe val� felt�lt�se BufferData-val
	//

	// vertexek poz�ci�i:
	/*
	Az m_ErdoVertexBuffer konstruktora m�r l�trehozott egy GPU puffer azonos�t�t �s a most k�vetkez� BufferData h�v�s ezt
	1. bind-olni fogja GL_ARRAY_BUFFER target-re (hiszen m_ErdoVertexBuffer t�pusa ArrayBuffer) �s
	2. glBufferData seg�ts�g�vel �tt�lti a GPU-ra az argumentumban adott t�rol� �rt�keit

	*/

	m_ErdoVertexBuffer.BufferData(vertices);

	// �s a primit�veket alkot� cs�cspontok indexei (az el�z� t�mb�kb�l) - triangle list-el val� kirajzol�sra felk�sz�lve
	m_ErdoIndices.BufferData(indices);

	// geometria VAO-ban val� regisztr�l�sa
	m_ErdoVao.Init(
		{
			// 0-�s attrib�tum "l�nyeg�ben" glm::vec3-ak sorozata �s az adatok az m_ErdoVertexBuffer GPU pufferben vannak
			{ CreateAttribute<		0,						// attrib�tum: 0
									glm::vec3,				// CPU oldali adatt�pus amit a 0-�s attrib�tum meghat�roz�s�ra haszn�ltunk <- az elj�r�s a glm::vec3-b�l kik�vetkezteti, hogy 3 darab float-b�l �ll a 0-�s attrib�tum
									0,						// offset: az attrib�tum t�rol� elej�t�l vett offset-je, byte-ban
									sizeof(Vertex)			// stride: a k�vetkez� cs�cspont ezen attrib�tuma h�ny byte-ra van az aktu�list�l
								>, m_ErdoVertexBuffer },
			{ CreateAttribute<1, glm::vec3, (sizeof(glm::vec3)), sizeof(Vertex)>, m_ErdoVertexBuffer },
			{ CreateAttribute<2, glm::vec2, (2 * sizeof(glm::vec3)), sizeof(Vertex)>, m_ErdoVertexBuffer },
		},
		m_ErdoIndices
	);
}

glm::vec3 GetTorusPos(float u, float v) {
	u *= 2 * 3.1415f;
	v *= 2 * 3.1415f;
	float cu = cosf(u), su = sinf(u), cv = cosf(v), sv = sinf(v);
	float r = 2;
	float R = 4;
	return glm::vec3((R + r * cu) * cv, r * su, (R + r * cu) * sv);
}
glm::vec3 GetTorusNormal(float u, float v) {
	glm::vec3 du = GetTorusPos(u + 0.01, v) - GetTorusPos(u - 0.01, v);
	glm::vec3 dv = GetTorusPos(u, v + 0.01) - GetTorusPos(u, v - 0.01);

	return glm::normalize(glm::cross(du, dv));
}
glm::vec2 GetTorusTexcoords(float u, float v) {
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
			vertices[i + j * (N + 1)].n = GetTorusNormal(u, v);
			vertices[i + j * (N + 1)].t = GetTorusTexcoords(u, v);
		}

	std::vector<int> indices(3 * 2 * (N) * (M));
	for (int i = 0; i < N; ++i)
		for (int j = 0; j < M; ++j)
		{
			// minden n�gysz�gre csin�ljunk kett� h�romsz�get, amelyek a k�vetkez� 
			// (i,j) indexekn�l sz�letett (u_i, v_i) param�ter�rt�kekhez tartoz�
			// pontokat k�tik �ssze:
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
			// - az (i,j)-hez tart�z� 1D-s index a VBO-ban: i+j*(N+1)
			// - az (i,j)-hez tart�z� 1D-s index az IB-ben: i*6+j*6*(N+1) 
			//		(mert minden n�gysz�gh�z 2db h�romsz�g = 6 index tartozik)
			//
			indices[6 * i + j * 3 * 2 * (N)+0] = (i)+(j) * (N + 1);
			indices[6 * i + j * 3 * 2 * (N)+1] = (i + 1) + (j) * (N + 1);
			indices[6 * i + j * 3 * 2 * (N)+2] = (i)+(j + 1) * (N + 1);
			indices[6 * i + j * 3 * 2 * (N)+3] = (i + 1) + (j) * (N + 1);
			indices[6 * i + j * 3 * 2 * (N)+4] = (i + 1) + (j + 1) * (N + 1);
			indices[6 * i + j * 3 * 2 * (N)+5] = (i)+(j + 1) * (N + 1);
		}

	//
	// geometria defini�l�sa (std::vector<...>) �s GPU pufferekbe val� felt�lt�se BufferData-val
	//

	// vertexek poz�ci�i:
	/*
	Az m_CubeVertexBuffer konstruktora m�r l�trehozott egy GPU puffer azonos�t�t �s a most k�vetkez� BufferData h�v�s ezt
	1. bind-olni fogja GL_ARRAY_BUFFER target-re (hiszen m_CubeVertexBuffer t�pusa ArrayBuffer) �s
	2. glBufferData seg�ts�g�vel �tt�lti a GPU-ra az argumentumban adott t�rol� �rt�keit

	*/

	m_TorusVertexBuffer.BufferData(vertices);

	// �s a primit�veket alkot� cs�cspontok indexei (az el�z� t�mb�kb�l) - triangle list-el val� kirajzol�sra felk�sz�lve
	m_TorusIndices.BufferData(indices);

	// geometria VAO-ban val� regisztr�l�sa
	m_TorusVao.Init(
		{
			// 0-�s attrib�tum "l�nyeg�ben" glm::vec3-ak sorozata �s az adatok az m_CubeVertexBuffer GPU pufferben vannak
			{ CreateAttribute<		0,						// attrib�tum: 0
									glm::vec3,				// CPU oldali adatt�pus amit a 0-�s attrib�tum meghat�roz�s�ra haszn�ltunk <- az elj�r�s a glm::vec3-b�l kik�vetkezteti, hogy 3 darab float-b�l �ll a 0-�s attrib�tum
									0,						// offset: az attrib�tum t�rol� elej�t�l vett offset-je, byte-ban
									sizeof(Vertex)			// stride: a k�vetkez� cs�cspont ezen attrib�tuma h�ny byte-ra van az aktu�list�l
								>, m_TorusVertexBuffer },
			{ CreateAttribute<1, glm::vec3, (sizeof(glm::vec3)), sizeof(Vertex)>, m_TorusVertexBuffer },
			{ CreateAttribute<2, glm::vec2, (2 * sizeof(glm::vec3)), sizeof(Vertex)>, m_TorusVertexBuffer },
		},
		m_TorusIndices
	);
}

float TalajMagassag(float u, float v) {
	return 0.2 * sin(10 * u) + 0.3 * sin(30 * v);
}

glm::vec3 GetTalajPos(float u, float v) {
	return glm::vec3(20 * u - 10, TalajMagassag(u, v), -20 * v + 10);
}

glm::vec3 GetTalajNormal(float u, float v) {
	glm::vec3 du = GetTalajPos(u + 0.01, v) - GetTalajPos(u - 0.01, v);
	glm::vec3 dv = GetTalajPos(u, v + 0.01) - GetTalajPos(u, v - 0.01);

	return glm::normalize(glm::cross(du, dv));
}
void CMyApp::InitTalaj()
{
	const int N = 20, M = 20; // lok�lis!!!!!! fel�l�rja az oszt�lyban l�v�t!!!!

	//struct Vertex{ glm::vec3 position; glm::vec3 normals; glm::vec2 texture; };
	std::vector<Vertex>vertices((N + 1) * (M + 1));

	for (int i = 0; i <= N; ++i)
		for (int j = 0; j <= M; ++j)
		{
			float u = i / (float)N;
			float v = j / (float)M;

			vertices[i + j * (N + 1)].p = GetTalajPos(u, v);
			vertices[i + j * (N + 1)].n = GetTalajNormal(u, v);
			vertices[i + j * (N + 1)].t = glm::vec2(u,v);
		}

	std::vector<int> indices(3 * 2 * (N) * (M));
	for (int i = 0; i < N; ++i)
		for (int j = 0; j < M; ++j)
		{
			// minden n�gysz�gre csin�ljunk kett� h�romsz�get, amelyek a k�vetkez� 
			// (i,j) indexekn�l sz�letett (u_i, v_i) param�ter�rt�kekhez tartoz�
			// pontokat k�tik �ssze:
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
			// - az (i,j)-hez tart�z� 1D-s index a VBO-ban: i+j*(N+1)
			// - az (i,j)-hez tart�z� 1D-s index az IB-ben: i*6+j*6*(N+1) 
			//		(mert minden n�gysz�gh�z 2db h�romsz�g = 6 index tartozik)
			//
			indices[6 * i + j * 3 * 2 * (N)+0] = (i)+(j) * (N + 1);
			indices[6 * i + j * 3 * 2 * (N)+1] = (i + 1) + (j) * (N + 1);
			indices[6 * i + j * 3 * 2 * (N)+2] = (i)+(j + 1) * (N + 1);
			indices[6 * i + j * 3 * 2 * (N)+3] = (i + 1) + (j) * (N + 1);
			indices[6 * i + j * 3 * 2 * (N)+4] = (i + 1) + (j + 1) * (N + 1);
			indices[6 * i + j * 3 * 2 * (N)+5] = (i)+(j + 1) * (N + 1);
		}

	//
	// geometria defini�l�sa (std::vector<...>) �s GPU pufferekbe val� felt�lt�se BufferData-val
	//

	// vertexek poz�ci�i:
	/*
	Az m_CubeVertexBuffer konstruktora m�r l�trehozott egy GPU puffer azonos�t�t �s a most k�vetkez� BufferData h�v�s ezt
	1. bind-olni fogja GL_ARRAY_BUFFER target-re (hiszen m_CubeVertexBuffer t�pusa ArrayBuffer) �s
	2. glBufferData seg�ts�g�vel �tt�lti a GPU-ra az argumentumban adott t�rol� �rt�keit

	*/

	m_TalajVertexBuffer.BufferData(vertices);

	// �s a primit�veket alkot� cs�cspontok indexei (az el�z� t�mb�kb�l) - triangle list-el val� kirajzol�sra felk�sz�lve
	m_TalajIndices.BufferData(indices);

	// geometria VAO-ban val� regisztr�l�sa
	m_TalajVao.Init(
		{
			// 0-�s attrib�tum "l�nyeg�ben" glm::vec3-ak sorozata �s az adatok az m_CubeVertexBuffer GPU pufferben vannak
			{ CreateAttribute<		0,						// attrib�tum: 0
									glm::vec3,				// CPU oldali adatt�pus amit a 0-�s attrib�tum meghat�roz�s�ra haszn�ltunk <- az elj�r�s a glm::vec3-b�l kik�vetkezteti, hogy 3 darab float-b�l �ll a 0-�s attrib�tum
									0,						// offset: az attrib�tum t�rol� elej�t�l vett offset-je, byte-ban
									sizeof(Vertex)			// stride: a k�vetkez� cs�cspont ezen attrib�tuma h�ny byte-ra van az aktu�list�l
								>, m_TalajVertexBuffer },
			{ CreateAttribute<1, glm::vec3, (sizeof(glm::vec3)), sizeof(Vertex)>, m_TalajVertexBuffer },
			{ CreateAttribute<2, glm::vec2, (2 * sizeof(glm::vec3)), sizeof(Vertex)>, m_TalajVertexBuffer },
		},
		m_TalajIndices
	);
}

void CMyApp::InitSkyBox()
{
	m_SkyboxPos.BufferData(
		std::vector<glm::vec3>{
		// h�ts� lap
		glm::vec3(-1, -1, -1),
		glm::vec3(1, -1, -1),
		glm::vec3(1, 1, -1),
		glm::vec3(-1, 1, -1),
		// el�ls� lap
		glm::vec3(-1, -1, 1),
		glm::vec3(1, -1, 1),
		glm::vec3(1, 1, 1),
		glm::vec3(-1, 1, 1),
	}
	);

	// �s a primit�veket alkot� cs�cspontok indexei (az el�z� t�mb�kb�l) - triangle list-el val� kirajzol�sra felk�sz�lve
	m_SkyboxIndices.BufferData(
		std::vector<int>{
			// h�ts� lap
			0, 1, 2,
			2, 3, 0,
			// el�ls� lap
			4, 6, 5,
			6, 4, 7,
			// bal
			0, 3, 4,
			4, 3, 7,
			// jobb
			1, 5, 2,
			5, 6, 2,
			// als�
			1, 0, 4,
			1, 4, 5,
			// fels�
			3, 2, 6,
			3, 6, 7,
	}
	);

	// geometria VAO-ban val� regisztr�l�sa
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

	TextureFromFileAttach("assets/xpos.png", GL_TEXTURE_CUBE_MAP_POSITIVE_X);
	TextureFromFileAttach("assets/xneg.png", GL_TEXTURE_CUBE_MAP_NEGATIVE_X);
	TextureFromFileAttach("assets/ypos.png", GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
	TextureFromFileAttach("assets/yneg.png", GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);
	TextureFromFileAttach("assets/zpos.png", GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
	TextureFromFileAttach("assets/zneg.png", GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

}

void CMyApp::InitShaders()
{
	// a shadereket t�rol� program l�trehoz�sa az OpenGL-hez hasonl� m�don:
	m_program.AttachShaders({
		{ GL_VERTEX_SHADER, "myVert.vert"}, // == "myVert.vert"_vs
		{ GL_FRAGMENT_SHADER, "myFrag.frag"}  // == "myFrag.frag"_fs
	});

	// attributomok osszerendelese a VAO es shader kozt
	m_program.BindAttribLocations({
		{ 0, "vs_in_pos" },				// VAO 0-as csatorna menjen a vs_in_pos-ba
		{ 1, "vs_in_norm" },			// VAO 1-es csatorna menjen a vs_in_norm-ba
		{ 2, "vs_in_tex" },				// VAO 2-es csatorna menjen a vs_in_tex-be
	});

	m_program.LinkProgram();

	// shader program r�vid l�trehoz�sa, egyetlen f�ggv�nyh�v�ssal a fenti h�rom:
	m_programSkybox.Init(
		{
			{ GL_VERTEX_SHADER, "skybox.vert" },
			{ GL_FRAGMENT_SHADER, "skybox.frag" }
		},
		{
			{ 0, "vs_in_pos" },				// VAO 0-as csatorna menjen a vs_in_pos-ba
		}
	);

	m_programAxes.Init({ "axes.frag"_fs, "axes.vert"_vs });
}

bool CMyApp::Init()
{
	// t�rl�si sz�n legyen k�kes
	glClearColor(0.125f, 0.25f, 0.5f, 1.0f);

	glEnable(GL_CULL_FACE); // kapcsoljuk be a hatrafele nezo lapok eldobasat
	glEnable(GL_DEPTH_TEST); // m�lys�gi teszt bekapcsol�sa (takar�s)

	InitShaders();
	InitCube();
	InitTorus();
	InitSkyBox();

	InitTalaj();
	InitErdo();

	// egy�b text�r�k bet�lt�se
	m_woodTexture.FromFile("assets/wood.jpg");
	m_suzanneTexture.FromFile("assets/marron.jpg");
	m_talajTexture.FromFile("assets/grass.jpg");
	m_erdoTexture.FromFile("assets/forest.jpg");

	// mesh betoltese
	m_mesh_suzanne = ObjParser::parse("assets/Suzanne.obj");
	m_mesh_suzanne->initBuffers();
	m_mesh_henger = ObjParser::parse("assets/henger.obj");
	m_mesh_henger->initBuffers();
	
	// kamera
	m_camera.SetProj(45.0f, 640.0f / 480.0f, 0.01f, 1000.0f);

	return true;
}

void CMyApp::Clean()
{
	glDeleteTextures(1, &m_skyboxTexture);

	delete m_mesh_suzanne;
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
	// t�r�lj�k a frampuffert (GL_COLOR_BUFFER_BIT) �s a m�lys�gi Z puffert (GL_DEPTH_BUFFER_BIT)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 viewProj = m_camera.GetViewProj();

	m_program.Use();

	//Suzanne
	glm::mat4 suzanneWorld = glm::mat4(1.0f);
	m_program.SetTexture("texImage", 0, m_suzanneTexture);
	m_program.SetUniform("MVP", viewProj * suzanneWorld);
	m_program.SetUniform("world", suzanneWorld);
	m_program.SetUniform("worldIT", glm::inverse(glm::transpose(suzanneWorld)));
	m_mesh_suzanne->draw();

	// talaj
	glm::mat4 talajWorld = glm::mat4(1.0f);
	m_TalajVao.Bind();
	m_program.SetTexture("texImage", 0, m_talajTexture);
	m_program.SetUniform("MVP", viewProj * talajWorld);
	m_program.SetUniform("world", talajWorld);
	m_program.SetUniform("worldIT", glm::inverse(glm::transpose(talajWorld)));
	glDrawElements(GL_TRIANGLES, 6 * 20 * 20, GL_UNSIGNED_INT, nullptr);

	// erd� h�tt�r
	m_ErdoVao.Bind();
	m_program.SetTexture("texImage", 0, m_erdoTexture);
	glm::mat4 erdoWorld = glm::translate(glm::vec3(0, 4, 0)) * glm::scale(glm::vec3(20, 10, 20));
	m_program.SetUniform("MVP", viewProj * erdoWorld);
	m_program.SetUniform("world", erdoWorld);
	m_program.SetUniform("worldIT", glm::inverse(glm::transpose(erdoWorld)));
	glDrawElements(GL_TRIANGLES, 4*6, GL_UNSIGNED_INT, nullptr);

	// kock�k
	//m_program.Use(); nem h�vjuk meg �jra, hisz ugyanazt a shadert haszn�lj�k
	m_CubeVao.Bind();
	m_program.SetTexture("texImage", 0, m_woodTexture);
	glm::mat4 cubeWorld;

	float time = SDL_GetTicks() / 1000.0f * 2 * float(M_PI) / 10;
	for (int i = 0; i < 10; ++i)
	{
		cubeWorld =
			glm::rotate(time + 2 * glm::pi<float>() / 10 * i, glm::vec3(0, 1, 0))*
			glm::translate(glm::vec3(10 + 5 * sin(time), 0, 0))*
			glm::rotate((i + 1)*time, glm::vec3(0, 1, 0));
		m_program.SetUniform("MVP", viewProj * cubeWorld);
		m_program.SetUniform("world", cubeWorld);
		m_program.SetUniform("worldIT", glm::inverse(glm::transpose(cubeWorld)));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
	}

	// t�rusz
	static glm::vec3 trans = glm::vec3(0, 3, 0);
	ImGui::DragFloat3("translate", &trans[0], 0.1f);

	m_TorusVao.Bind();
	//m_program.SetTexture("texImage", 0, m_suzanneTexture);
	glm::mat4 torusWorld = glm::translate(trans) * glm::scale(glm::vec3(0.3f));
	m_program.SetUniform("MVP", viewProj * torusWorld);
	m_program.SetUniform("world", torusWorld);
	m_program.SetUniform("worldIT", glm::inverse(glm::transpose(torusWorld)));
	glDrawElements(GL_TRIANGLES, 6 * N * M, GL_UNSIGNED_INT, nullptr);

	m_program.Unuse();

	// skybox
	// ments�k el az el�z� Z-test eredm�nyt, azaz azt a rel�ci�t, ami alapj�n update-elj�k a pixelt.
	GLint prevDepthFnc;
	glGetIntegerv(GL_DEPTH_FUNC, &prevDepthFnc);

	// most kisebb-egyenl�t haszn�ljunk, mert mindent kitolunk a t�voli v�g�s�kokra
	glDepthFunc(GL_LEQUAL);

	m_SkyboxVao.Bind();
	m_programSkybox.Use();
	m_programSkybox.SetUniform("MVP", viewProj * glm::translate( m_camera.GetEye()) );
	
	// cube map text�ra be�ll�t�sa 0-�s mintav�telez�re �s annak a shaderre be�ll�t�sa
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_skyboxTexture);
	glUniform1i(m_programSkybox.GetLocation("skyboxTexture"), 0);
	// az el�z� h�rom sor <=> m_programSkybox.SetCubeTexture("skyboxTexture", 0, m_skyboxTexture);

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
	m_programSkybox.Unuse();

	// v�g�l �ll�tsuk vissza
	glDepthFunc(prevDepthFnc);


	// 1. feladat: k�sz�ts egy vertex shader-fragment shader p�rt, ami t�rolt geometria _n�lk�l_ kirajzol egy tetsz�leges poz�ci�ba egy XYZ tengely-h�rmast,
	//			   ahol az X piros, az Y z�ld a Z pedig k�k!
	m_programAxes.Use();
	m_programAxes.SetUniform("MVP", viewProj * glm::scale(glm::vec3(3.0f)));
	glDrawArrays(GL_LINES, 0, 6);
	m_programAxes.Unuse();

	//ImGui Testwindow
	ImGui::ShowTestWindow();
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

// a k�t param�terbe az �j ablakm�ret sz�less�ge (_w) �s magass�ga (_h) tal�lhat�
void CMyApp::Resize(int _w, int _h)
{
	glViewport(0, 0, _w, _h );

	m_camera.Resize(_w, _h);
}
