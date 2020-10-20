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

	glm::vec3 toDesc(float fi, float theta);

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
	struct Vertex
	{
		glm::vec3 p;
		glm::vec2 t;
	};

	// belsõ eljárások
	GLuint GenerateRandomTexture();

	// shaderekhez szükséges változók
	GLuint m_programID; // shaderprogram erõforrás azonosító

	// OpenGL-es dolgok
	GLuint m_floor_vaoID; // vertex array object erõforrás azonosító
	GLuint m_floor_vboID; // vertex buffer object erõforrás azonosító
	GLuint m_floor_ibID;  // index buffer object erõforrás azonosító
	GLuint m_cube_vaoID;
	GLuint m_cube_vboID;
	GLuint m_cube_ibID;
	GLuint m_generatedTextureID; // generált textúra erõforrás azonosító
	GLuint m_loadedTextureID; // betöltött textúra erõforrás azonosító

	// transzformációs mátrixok
	glm::mat4 m_matWorld;
	glm::mat4 m_matView;
	glm::mat4 m_matProj;

	// uniform változók helye a shaderekben
	GLuint	m_loc_mvp;
	GLuint  m_loc_w;
	GLuint	m_loc_tex;
	GLuint	m_loc_tex2;
	GLuint	m_loc_t;

	// a jobb olvashatóság kedvéért
	void InitCube();
	void InitFloor();
	void InitShaders();
	void InitTextures();


	float m_fi = M_PI / 2.0;
	float m_theta = M_PI / 2.0;

	glm::vec3 m_eye = glm::vec3(0, 0, 10);
	glm::vec3 m_at = m_eye + toDesc(m_fi, m_theta); //(0,0,10) + (0,0,-1) = (0,0,9)
	glm::vec3 m_up = glm::vec3(0, 1, 0);

	bool w = false, a = false, s = false, d = false;
};

