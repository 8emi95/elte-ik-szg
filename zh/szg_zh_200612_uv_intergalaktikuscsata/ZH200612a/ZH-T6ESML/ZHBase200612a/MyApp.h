#pragma once

// C++ includes
#include <memory>

// GLEW
#include <GL/glew.h>

// SDL
#include <SDL.h>
#include <SDL_opengl.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>

#include "includes/gCamera.h"

#include "includes/ProgramObject.h"
#include "includes/BufferObject.h"
#include "includes/VertexArrayObject.h"
#include "includes/TextureObject.h"

// mesh
#include "includes/ObjParser_OGL3.h"

class CMyApp
{
public:
	CMyApp(void);
	~CMyApp(void);

	bool Init();
	void Clean();

	void Update();
	void Render();

	void KeyboardDown(SDL_KeyboardEvent&);
	void KeyboardUp(SDL_KeyboardEvent&);
	void MouseMove(SDL_MouseMotionEvent&);
	void MouseDown(SDL_MouseButtonEvent&);
	void MouseUp(SDL_MouseButtonEvent&);
	void MouseWheel(SDL_MouseWheelEvent&);
	void Resize(int, int);

protected:
	ProgramObject		m_MeshProgram;		// mesh shader
	ProgramObject		m_SkyboxProgram;	// skybox shader
	ProgramObject		m_AxesProgram;		// axes shader

	// kocka
	VertexArrayObject	m_CubeVao;			// VAO
	IndexBuffer			m_CubeIndices;		// index buffer
	ArrayBuffer			m_CubeVertexBuffer;	// VBO
	// skybox kocka
	VertexArrayObject	m_SkyboxVao;
	IndexBuffer			m_SkyboxIndices;	
	ArrayBuffer			m_SkyboxPos;
	// gömb
	const int N = 20;
	const int M = 20;
	VertexArrayObject	m_TorusVao;
	IndexBuffer			m_TorusIndices;
	ArrayBuffer			m_TorusVertexBuffer;



	// sphere
	VertexArrayObject	m_SphereVao;
	IndexBuffer			m_SphereIndices;
	ArrayBuffer			m_SphereVertexBuffer;



	gCamera				m_camera;

	Texture2D			m_WoodTexture;
	Texture2D			m_MarronTexture;
	
	// sun
	Texture2D			m_sunTexture;
	// planet
	Texture2D			m_planetTexture;
	// moon
	Texture2D			m_moonTexture;
	// mesh
	Texture2D			m_meshTexture;

	// nyers OGL azonosítók
	GLuint				m_skyboxTexture = 0;

	struct Vertex
	{
		glm::vec3 p;
		glm::vec3 n;
		glm::vec2 t;
	};

	// mesh adatok
	Mesh *m_SuzanneMesh = nullptr;

	// death star, destroyer, nebulon
	Mesh* m_destroyerMesh = nullptr;
	Mesh* m_nebulonMesh = nullptr;

	Mesh* m_deathStar = nullptr;

	// a jobb olvashatóság kedvéért
	void InitShaders();
	void InitCube();
	void InitTorus();
	void InitSkyBox();

	// sphere
	void InitSphere();

	struct Destroyer {
		glm::vec3 pos;
	};

	int numDestroyer = 0;
	int numNebulon = 0;
};

