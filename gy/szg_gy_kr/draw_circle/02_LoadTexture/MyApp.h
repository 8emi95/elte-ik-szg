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
protected:
	// belsõ eljárások
	GLuint GenTexture();

	// shaderekhez szükséges változók
	GLuint m_programID; // shaderek programja

	// transzformációs mátrixok
	glm::mat4 m_matWorld;
	glm::mat4 m_matView;
	glm::mat4 m_matProj;

	// mátrixok helye a shaderekben
	GLuint	m_loc_mvp;

	GLuint	m_loc_texture1;
	GLuint	m_loc_texture2;
	GLuint m_loc_t;

	// OpenGL-es dolgok
	GLuint m_vaoID; // vertex array object erõforrás azonosító
	GLuint m_vboID; // vertex buffer object erõforrás azonosító
	GLuint m_ibID;  // index buffer object erõforrás azonosító
	GLuint m_atomicTextureID; // fájlból betöltött textúra azonosítója
	GLuint m_donotTextureID; // fájlból betöltött textúra azonosítója

	struct Vertex
	{
		glm::vec3 p; // pozíció
		glm::vec3 c; // szín
		glm::vec2 t; // textúra koordináták
	};

	int N = 30; // a sokszög csúcsainak száma
	bool draw_lines = false; // drótvázas legyen-e a megjelenítés
	bool draw_back_faces = false; // rajzoljunk-e hátlapokat

};

