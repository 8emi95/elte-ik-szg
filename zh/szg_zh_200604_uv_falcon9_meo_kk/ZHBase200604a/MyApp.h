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

	const int N = 60;
	const int M = 60;
	// gömb
	VertexArrayObject	m_SphereVao;
	IndexBuffer			m_SphereIndices;
	ArrayBuffer			m_SphereVertexBuffer;
	// paraloid
	VertexArrayObject	m_ParaboloidVao;
	IndexBuffer			m_ParaboloidIndices;
	ArrayBuffer			m_ParaboloidVertexBuffer;
	// henger
	VertexArrayObject	m_CylinderVao;
	IndexBuffer			m_CylinderIndices;
	ArrayBuffer			m_CylinderVertexBuffer;
	// kör
	VertexArrayObject	m_CircleVao;
	ArrayBuffer			m_CircleVertexBuffer;

	// paraloid
	VertexArrayObject	m_ParaboloidOuterVao;
	IndexBuffer			m_ParaboloidOuterIndices;
	ArrayBuffer			m_ParaboloidOuterVertexBuffer;
	// henger
	VertexArrayObject	m_CylinderOuterVao;
	IndexBuffer			m_CylinderOuterIndices;
	ArrayBuffer			m_CylinderOuterVertexBuffer;

	gCamera				m_camera;

	Texture2D			m_MetalTexture;
	Texture2D			m_EarthTexture;
	Texture2D			m_FalconTexture;
	Texture2D			m_Falcon2Texture;
	Texture2D			m_DragonTexture;
	Texture2D			m_Dragon2Texture;

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

	// a jobb olvashatóság kedvéért
	void InitShaders();
	void InitCube();
	void InitSphere();
	void InitParaboloid();
	void InitParaboloidOuter();
	void InitCylinder();
	void InitCylinderOuter();
	void InitCircle();
	void InitCircleOuter();
	void DrawParaboloid(glm::mat4 trafo);
	void DrawParaboloid2(glm::mat4 trafo);
	void DrawCylinder(glm::mat4 trafo);
	void DrawCylinder2(glm::mat4 trafo);
	void DrawHenger(glm::mat4 trafo);
	void DrawCircle(glm::mat4 trafo);
	void DrawHenger2(glm::mat4 trafo);
	void DrawRocket(glm::mat4 trafo);
	void InitRocket();
	void InitSkyBox();
};

