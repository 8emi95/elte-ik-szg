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
	glm::vec3 hengerUV(float u, float v);
	glm::vec3 topUV(float u, float v);
protected:
	
	// shaderekhez szükséges változók
	GLuint m_programID; // shaderek programja

	void DrawHenger(int a, int b, int c);

	// OpenGL-es dolgok
	GLuint m_vaoID; // vertex array object erõforrás azonosító
	GLuint m_vboID; // vertex buffer object erõforrás azonosító
	GLuint m_ibID;

	// transzformációs mátrixok
	glm::mat4 m_matWorld;
	glm::mat4 m_matView;
	glm::mat4 m_matProj;

	// mátrixok helye a shaderekben
	GLuint	m_loc_mvp;

	struct Vertex
	{
		glm::vec3 p;
		glm::vec3 c;
	};

	static const int N = 20;
	static const int M = 20;
};

