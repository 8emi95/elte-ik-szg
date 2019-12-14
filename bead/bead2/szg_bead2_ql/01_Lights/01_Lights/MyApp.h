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
protected:
	// private functions
	GLuint GenTexture();

	// shader related variables
	GLuint m_programID; // shaderek program ID

	// transformation matrices
	glm::mat4 m_matView;
	glm::mat4 m_matProj;

	// IDs of shader variables
	GLuint	m_loc_world;
	GLuint	m_loc_worldIT;
	GLuint	m_loc_mvp;
	GLuint	m_loc_texture;
	GLfloat  m_loc_plus;
	GLfloat  m_loc_forward;
	GLfloat  m_loc_right;
	GLfloat  m_loc_up;

	float	m_angleY;

	glm::vec3 m_eye = glm::vec3(14,14,14);
	glm::vec3 m_fw = glm::normalize(glm::vec3(-1,-1,-1));

	bool mouse_pressed = false;
	float plus;
	float forward;
	float right;
	float up;

	// OpenGL resource identifiers
	GLuint m_vaoID; // vertex array object resource name
	GLuint m_vboID; // vertex buffer object resource name
	GLuint m_ibID;  // index buffer object resource name
	GLuint m_textureID; // textúra resource name

	// data to be stored at each vertex
	struct Vertex
	{
		glm::vec3 p;	// vertex position
		glm::vec3 n;	// vertex normal
		glm::vec2 t;	// texture coordinates
	};

	Mesh* m_suzanne;
};

