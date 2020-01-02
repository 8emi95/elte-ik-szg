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
	glm::vec3 GetBubiPos(float u, float v);
	glm::vec3 GetBubiNorm(float u, float v);
	glm::vec2 GetBubiTex(float u, float v);
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
	ProgramObject		m_gombProgram;
	ProgramObject		m_bubiProgram;

	Texture2D			m_textureMetalAkvarium;
	Texture2D			m_textureMetalHal;
	Texture2D			m_textureMetalPiramis;

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

	VertexArrayObject	bubi_vao;
	IndexBuffer			bubi_BufferIndices;
	ArrayBuffer			bubi_BufferPos;
	ArrayBuffer			bubi_BufferNorm;
	ArrayBuffer			bubi_BufferText;


	static const int N = 40;
	static const int M = 40;

	int halDb = 5;

	int korSugar = 15;

	int korIdo = 6;

	int rand_db = 0;

	float bUp = 0.f;
	float kulonbseg = 0.f;

	std::vector<glm::vec3>		m_bubi_db{};

	float t = SDL_GetTicks() / 1000.f;

	//std::unique_ptr<Mesh>	m_mesh;

	gCamera				m_camera;

	const int kMaxPointCount = 10;

	std::vector<glm::vec3>		m_controlPoints{ {-10,0,-10}, {10,0,-10} };
	std::vector<std::string>	m_pointName{ "Point 1", "Point 2" };
};

