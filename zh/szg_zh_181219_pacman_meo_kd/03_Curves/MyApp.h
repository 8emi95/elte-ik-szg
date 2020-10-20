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
	glm::vec3 GetTalajPos(float u, float v);
	glm::vec3 GetTalajNorm(float u, float v);
	glm::vec2 GetTalajTex(float u, float v);
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

	// shaderekhez szükséges változók
	ProgramObject		m_program;		// shaderek programja
	ProgramObject		m_axesProgram;
	ProgramObject		m_pointProgram;
	ProgramObject		gomb_program;
	ProgramObject		korlap_program;
	ProgramObject		szellem_program;
	ProgramObject		m_talajProgram;

	Texture2D			m_textureMetal;
	Texture2D			m_textureMetalGomb;

	VertexArrayObject	m_vao;			// VAO objektum
	IndexBuffer			m_gpuBufferIndices;		// indexek
	ArrayBuffer			m_gpuBufferPos;	// pozíciók tömbje
	ArrayBuffer			m_gpuBufferNormal;	// normálisok tömbje
	ArrayBuffer			m_gpuBufferTex;	// textúrakoordináták tömbje

	VertexArrayObject	gomb_vao;
	IndexBuffer			gomb_BufferIndices;
	ArrayBuffer			gomb_BufferPos;
	ArrayBuffer			gomb_BufferNorm;
	ArrayBuffer			gomb_BufferText;

	VertexArrayObject	korlap_vao;
	IndexBuffer			korlap_BufferIndices;
	ArrayBuffer			korlap_BufferPos;
	
	VertexArrayObject	szellem_vao;
	IndexBuffer			szellem_BufferIndices;
	ArrayBuffer			szellem_BufferPos;

	VertexArrayObject	talaj_vao;			// VAO objektum
	IndexBuffer			talaj_gpuBufferIndices;		// indexek
	ArrayBuffer			talaj_gpuBufferPos;	// pozíciók tömbje
	ArrayBuffer			talaj_gpuBufferNormal;	// normálisok tömbje
	ArrayBuffer			talaj_gpuBufferTex;	// textúrakoordináták tömbje

	std::unique_ptr<Mesh>	m_mesh;

	gCamera				m_camera;

	const int kMaxPointCount = 10;

	static const int N = 20;
	static const int M = 20;

	static const int I = 32;
	static const int J = 32;

	int hammTime = 2;

	int pacStepCount = 0;

	int pacx = 0;
	int pacz = 0;

	float pFace = 180.f;

	int		m_szellemCount = 10;

	std::vector<glm::vec3>		m_szellemPos{};

	std::vector<glm::vec3>		m_controlPoints{ {-10,0,-10}, {10,0,-10} };
	std::vector<std::string>	m_pointName{ "Point 1", "Point 2" };
};

