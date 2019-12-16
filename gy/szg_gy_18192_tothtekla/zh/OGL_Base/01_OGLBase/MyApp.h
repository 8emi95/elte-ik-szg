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

#include "gCamera.h"

#include "ProgramObject.h"
#include "BufferObject.h"
#include "VertexArrayObject.h"
#include "TextureObject.h"

#include "Mesh_OGL3.h"
#include "ObjParser_OGL3.h"

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

	struct MyObject {
		VertexArrayObject	vao;				// VAO objektum
		IndexBuffer			gpuBufferIndices;	// indexek
		ArrayBuffer			gpuBufferPos;		// pozíciók tömbje
		ArrayBuffer			gpuBufferNorm;		// normál vektorok tömbje
		ArrayBuffer			gpuBufferTex;		// textúra pozíciók tömbje
		int					size;
	};

protected:
	// segédeljárások
	void TextureFromFileAttach(const char* filename, GLuint role, bool flip = true) const;

	// shaderekhez szükséges változók
	ProgramObject		m_program;			// shaderek programja
	ProgramObject		m_programSkybox;	// skybox shaderek

	VertexArrayObject	m_cube_vao;					// VAO objektum
	IndexBuffer			m_cube_gpuBufferIndices;	// indexek
	ArrayBuffer			m_cube_gpuBufferPos;		// pozíciók tömbje

	MyObject sphere;

	//VertexArrayObject	m_vao;				// VAO objektum
	//IndexBuffer			m_gpuBufferIndices;	// indexek
	//ArrayBuffer			m_gpuBufferPos;		// pozíciók tömbje

	gCamera				m_camera;

	// nyers OGL azonosítók
	GLuint				m_earthTexture;
	GLuint				m_skyboxTexture;

	Mesh* m_suzanne;
	float spheres_distance = 10.0;
	void BuildSphereGeometry();
};

