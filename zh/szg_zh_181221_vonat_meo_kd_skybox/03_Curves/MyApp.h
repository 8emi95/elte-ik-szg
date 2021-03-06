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
	// g�rb�nk ki�rt�kel�se
	glm::vec3 Eval(float);
	float	m_currentParam{ 0 };
	// seg�delj�r�sok
	void TextureFromFileAttach(const char* filename, GLuint role) const;

	// shaderekhez sz�ks�ges v�ltoz�k
	ProgramObject		m_program;		// shaderek programja
	ProgramObject		m_axesProgram;
	ProgramObject		m_pointProgram;
	ProgramObject		m_talajProgram;
	ProgramObject		m_mozdonyProgram;
	ProgramObject		m_vagonProgram;
	ProgramObject		m_programSkybox;
	ProgramObject		m_particleProgram;

	Texture2D			m_textureMetal;

	VertexArrayObject	m_vao;			// VAO objektum
	IndexBuffer			m_gpuBufferIndices;		// indexek
	ArrayBuffer			m_gpuBufferPos;	// poz�ci�k t�mbje
	ArrayBuffer			m_gpuBufferNormal;	// norm�lisok t�mbje
	ArrayBuffer			m_gpuBufferTex;	// text�rakoordin�t�k t�mbje
	
	VertexArrayObject	talaj_vao;			// VAO objektum
	IndexBuffer			talaj_gpuBufferIndices;		// indexek
	ArrayBuffer			talaj_gpuBufferPos;	// poz�ci�k t�mbje
	ArrayBuffer			talaj_gpuBufferNormal;	// norm�lisok t�mbje
	ArrayBuffer			talaj_gpuBufferTex;	// text�rakoordin�t�k t�mbje
	
	VertexArrayObject	mozdony_vao;			// VAO objektum
	IndexBuffer			mozdony_gpuBufferIndices;		// indexek
	ArrayBuffer			mozdony_gpuBufferPos;	// poz�ci�k t�mbje
	ArrayBuffer			mozdony_gpuBufferNormal;	// norm�lisok t�mbje
	ArrayBuffer			mozdony_gpuBufferTex;	// text�rakoordin�t�k t�mbje

	VertexArrayObject	vagon_vao;			// VAO objektum
	IndexBuffer			vagon_gpuBufferIndices;		// indexek
	ArrayBuffer			vagon_gpuBufferPos;	// poz�ci�k t�mbje
	ArrayBuffer			vagon_gpuBufferNormal;	// norm�lisok t�mbje
	ArrayBuffer			vagon_gpuBufferTex;	// text�rakoordin�t�k t�mbje

	std::unique_ptr<Mesh>	m_mesh;

	gCamera				m_camera;

	static const int N = 100;
	static const int M = 100;

	const int kMaxPointCount = 10;

	std::vector<glm::vec3>		m_controlPoints{ {-10,0,-10}, {10,0,-10} };
	std::vector<std::string>	m_pointName{ "Point 1", "Point 2" };

	GLuint				m_skyboxTexture;

	int		m_particleCount = 200;

	std::vector<glm::vec3>		m_particlePos{};
	std::vector<glm::vec3>		m_particleVel{};

	VertexArrayObject	m_gpuParticleVAO;
	ArrayBuffer			m_gpuParticleBuffer;
};

