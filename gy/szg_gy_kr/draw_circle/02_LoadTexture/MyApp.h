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
	// bels� elj�r�sok
	GLuint GenTexture();

	// shaderekhez sz�ks�ges v�ltoz�k
	GLuint m_programID; // shaderek programja

	// transzform�ci�s m�trixok
	glm::mat4 m_matWorld;
	glm::mat4 m_matView;
	glm::mat4 m_matProj;

	// m�trixok helye a shaderekben
	GLuint	m_loc_mvp;

	GLuint	m_loc_texture1;
	GLuint	m_loc_texture2;
	GLuint m_loc_t;

	// OpenGL-es dolgok
	GLuint m_vaoID; // vertex array object er�forr�s azonos�t�
	GLuint m_vboID; // vertex buffer object er�forr�s azonos�t�
	GLuint m_ibID;  // index buffer object er�forr�s azonos�t�
	GLuint m_atomicTextureID; // f�jlb�l bet�lt�tt text�ra azonos�t�ja
	GLuint m_donotTextureID; // f�jlb�l bet�lt�tt text�ra azonos�t�ja

	struct Vertex
	{
		glm::vec3 p; // poz�ci�
		glm::vec3 c; // sz�n
		glm::vec2 t; // text�ra koordin�t�k
	};

	int N = 30; // a soksz�g cs�csainak sz�ma
	bool draw_lines = false; // dr�tv�zas legyen-e a megjelen�t�s
	bool draw_back_faces = false; // rajzoljunk-e h�tlapokat

};

