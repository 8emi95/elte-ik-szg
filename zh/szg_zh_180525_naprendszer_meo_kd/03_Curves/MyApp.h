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

#include "ProgramObject.h"
#include "BufferObject.h"
#include "VertexArrayObject.h"
#include "TextureObject.h"

#include "Mesh_OGL3.h"
#include "gCamera.h"

#include <vector>

class CMyApp
{
public:
	CMyApp(void);
	~CMyApp(void);

	bool Init();
	glm::vec3 GetPos(float u, float v);
	glm::vec3 GetNorm(float u, float v);
	glm::vec2 GetTex(float u, float v);
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
	// görbénk kiértékelése
	glm::vec3 Eval(float);
	float	m_currentParam{ 0 };

	// segédeljárások
	void TextureFromFileAttach(const char* filename, GLuint role) const;

	// shaderekhez szükséges változók
	ProgramObject		m_program;		// shaderek programja
	ProgramObject		m_axesProgram;
	ProgramObject		m_pointProgram;
	ProgramObject		korlap_program;
	ProgramObject		felgomb_program;
	ProgramObject		henger_program;
	ProgramObject		m_programSkybox;	// skybox shaderek

	Texture2D			m_textureMetal;
	Texture2D			m_textureMetalEarth;
	Texture2D			m_textureMetalScales;

	VertexArrayObject	m_vao;			// VAO objektum
	IndexBuffer			m_gpuBufferIndices;		// indexek
	ArrayBuffer			m_gpuBufferPos;	// pozíciók tömbje
	ArrayBuffer			m_gpuBufferNormal;	// normálisok tömbje
	ArrayBuffer			m_gpuBufferTex;	// textúrakoordináták tömbje

	VertexArrayObject	korlap_vao;
	IndexBuffer			korlap_BufferIndices;
	ArrayBuffer			korlap_BufferPos;
	ArrayBuffer			korlap_BufferNorm;
	ArrayBuffer			korlap_BufferText;

	VertexArrayObject	felgomb_vao;
	IndexBuffer			felgomb_BufferIndices;
	ArrayBuffer			felgomb_BufferPos;
	ArrayBuffer			felgomb_BufferNorm;
	ArrayBuffer			felgomb_BufferText;

	VertexArrayObject	henger_vao;
	IndexBuffer			henger_BufferIndices;
	ArrayBuffer			henger_BufferPos;
	ArrayBuffer			henger_BufferNorm;
	ArrayBuffer			henger_BufferText;
	
	gCamera				m_camera;

	const int kMaxPointCount = 10;

	const int hengerNum = 40;

	static const int N = 20;
	static const int M = 20;

	int runSpeed = 4;
	float amp = 0;
	bool b = true;

	glm::vec3 temp;

	std::vector<glm::vec3>		m_controlPoints{ {0,0,0} };
	std::vector<std::string>	m_pointName{ "Point 1", "Point 2" };

	// nyers OGL azonosítók
	GLuint				m_skyboxTexture;
};

