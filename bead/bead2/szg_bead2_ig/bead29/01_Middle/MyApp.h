#pragma once

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
#include "gShaderProgram.h"
#include "gVertexBuffer.h"
#include "Mesh_OGL3.h"

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
	GLuint m_programID; // shaderek programja

						// OpenGL-es dolgok
	GLuint m_vaoID; // vertex array object erõforrás azonosító
	GLuint m_vboID; // vertex buffer object erõforrás azonosító

					// transzformációs mátrixok
	glm::mat4 m_matWorld;
	glm::mat4 m_matView;
	glm::mat4 m_matProj;

	// mátrixok helye a shaderekben
	GLuint	m_loc_world;
	GLuint	m_loc_view;
	GLuint	m_loc_proj;

	GLuint	m_loc_texture1;
	GLuint	m_loc_texture2;
	GLuint	m_loc_t;

	GLuint m_atomicTextureID; // fájlból betöltött textúra azonosítója
	GLuint m_donotTextureID; // fájlból betöltött textúra azonosítója

	Mesh			*m_mesh;
	Mesh			*m_mesh2;

	int textChange = 0;

	int startTick = 0;

	struct Vertex
	{
		glm::vec3 p;
		glm::vec3 c;

	};
};

