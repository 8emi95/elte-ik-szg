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
#include <map>

class CMyApp
{
public:
	CMyApp(void);
	~CMyApp(void);

	bool Init();
	glm::vec3 GetDombPos(float u, float v);
	glm::vec3 GetDombNorm(float u, float v);
	glm::vec2 GetDombTex(float u, float v);
	glm::vec3 GetErdoPos(float u, float v);
	glm::vec3 GetErdoNorm(float u, float v);
	glm::vec2 GetErdoTex(float u, float v);
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

	// shaderekhez szükséges változók
	ProgramObject		m_program;		// shaderek programja
	ProgramObject		m_axesProgram;
	ProgramObject		m_pointProgram;
	ProgramObject		domborzat_program;
	ProgramObject		erdo_program;
	ProgramObject		gomb_program;

	Texture2D			m_textureMetalDomborzat;
	Texture2D			m_textureMetalErdo;
	Texture2D			m_textureMetalGomb;
	Texture2D			m_textureMetalHenger;

	VertexArrayObject	m_vao;			// VAO objektum
	IndexBuffer			m_gpuBufferIndices;		// indexek
	ArrayBuffer			m_gpuBufferPos;	// pozíciók tömbje
	ArrayBuffer			m_gpuBufferNormal;	// normálisok tömbje
	ArrayBuffer			m_gpuBufferTex;	// textúrakoordináták tömbje

	VertexArrayObject	domborzat_vao;
	IndexBuffer			domborzat_BufferIndices;
	ArrayBuffer			domborzat_BufferPos;
	ArrayBuffer			domborzat_BufferNorm;
	ArrayBuffer			domborzat_BufferText;

	VertexArrayObject	erdo_vao;
	IndexBuffer			erdo_BufferIndices;
	ArrayBuffer			erdo_BufferPos;
	ArrayBuffer			erdo_BufferNorm;
	ArrayBuffer			erdo_BufferText;

	VertexArrayObject	gomb_vao;
	IndexBuffer			gomb_BufferIndices;
	ArrayBuffer			gomb_BufferPos;
	ArrayBuffer			gomb_BufferNorm;
	ArrayBuffer			gomb_BufferText;

	std::unique_ptr<Mesh>	m_mesh;

	gCamera				m_camera;

	const int kMaxPointCount = 10;

	static const int N = 20;
	static const int M = 20;

	float faNov = 0.5f;

	std::vector<float> fa_merete{};

	float t = SDL_GetTicks() / 1000.0f;

	std::vector<glm::vec3>		m_fa_db{};

	std::map<std::pair<int, int>, float> dombY;

	std::map<std::pair<int, int>, bool> van_e_Fa;
	std::map<std::pair<int, int>, int> fa_kora;

	std::vector<glm::vec3>		m_controlPoints{ {-10,0,-10}, {10,0,-10} };
	std::vector<std::string>	m_pointName{ "Point 1", "Point 2" };

	std::vector<glm::vec3> dombPos;

	int treeCount = 0;

	float fa_ido = 10.f;

};

