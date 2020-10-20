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
	// shaderekhez szükséges változók
	ProgramObject		m_program;			// mesh shader
	ProgramObject		m_programSkybox;	// skybox shader
	ProgramObject		m_programAxes;		// axes shader

	// kocka
	VertexArrayObject	m_CubeVao;			// VAO
	IndexBuffer			m_CubeIndices;		// index buffer
	ArrayBuffer			m_CubeVertexBuffer;	// VBO
	// skybox kocka
	VertexArrayObject	m_SkyboxVao;
	IndexBuffer			m_SkyboxIndices;	
	ArrayBuffer			m_SkyboxPos;		
	// tórusz
	VertexArrayObject	m_TorusVao;				// VAO
	IndexBuffer			m_TorusIndices;			// index buffer
	ArrayBuffer			m_TorusVertexBuffer;	// VBO
	const int N = 20, M = 20;


	// talaj
	VertexArrayObject	m_TalajVao;				// VAO
	IndexBuffer			m_TalajIndices;			// index buffer
	ArrayBuffer			m_TalajVertexBuffer;	// VBO

	// erdö háttér
	VertexArrayObject	m_ErdoVao;				// VAO
	IndexBuffer			m_ErdoIndices;			// index buffer
	ArrayBuffer			m_ErdoVertexBuffer;		// VBO


	gCamera				m_camera;

	Texture2D			m_woodTexture;
	Texture2D			m_suzanneTexture;
	Texture2D			m_talajTexture;
	Texture2D			m_erdoTexture;

	// nyers OGL azonosítók
	GLuint				m_skyboxTexture;

	struct Vertex
	{
		glm::vec3 p;
		glm::vec3 n;
		glm::vec2 t;
	};

	// mesh adatok
	Mesh *m_mesh_suzanne;
	Mesh* m_mesh_henger;

	// a jobb olvashatóság kedvéért
	void InitShaders();
	void InitCube();
	void InitTorus();
	void InitSkyBox();

	void InitTalaj();
	void InitErdo();
};

