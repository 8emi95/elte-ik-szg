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
	// shaderekhez sz�ks�ges v�ltoz�k
	ProgramObject		m_program;		// shaderek programja

	Texture2D			m_textureMetal;

	VertexArrayObject	m_vao_ground;			// VAO objektum
	IndexBuffer			m_gpuBufferIndices;		// indexek
	ArrayBuffer			m_gpuBufferPos;	// poz�ci�k t�mbje
	ArrayBuffer			m_gpuBufferNormal;	// norm�lisok t�mbje
	ArrayBuffer			m_gpuBufferTex;	// text�rakoordin�t�k t�mbje

	void DrawGround();
	void DrawSuzanne();

	//g�mbh�z sz�ks�ges v�ltoz�k/fvek
	glm::vec3 GetUV(float u, float v);
	glm::vec3 GetUV_henger(float u, float v);
	void DrawOrb();
	VertexArrayObject	m_vao_orb;			// VAO objektum
	IndexBuffer			m_orbBufferIndices;		// indexek
	ArrayBuffer			m_orbBufferPos;	// poz�ci�k t�mbje
	ArrayBuffer			m_orbBufferNormal;	// norm�lisok t�mbje
	ArrayBuffer			m_orbBufferTex;	// text�rakoordin�t�k t�mbje
	std::vector<glm::vec3>		m_orbPos{};
	std::vector<glm::vec3>		m_orbNorm{};
	std::vector<glm::vec2>		m_orbTex{};
	std::vector<int>			m_orbTriangles{};
	Texture2D			m_textureOrb;
	int N = 50; // a g�mb felbont�sa
	int M = 50;
	//g�mb v�ge

	//csonkagula
	void DrawCone();
	VertexArrayObject	m_vao_cone;			// VAO objektum
	IndexBuffer			m_coneBufferIndices;		// indexek
	ArrayBuffer			m_coneBufferPos;	// poz�ci�k t�mbje
	ArrayBuffer			m_coneBufferNormal;	// norm�lisok t�mbje
	ArrayBuffer			m_coneBufferTex;	// text�rakoordin�t�k t�mbje
	std::vector<glm::vec3>		m_conePos{};
	std::vector<glm::vec3>		m_coneNorm{};
	std::vector<glm::vec2>		m_coneTex{};
	std::vector<int>			m_coneTriangles{};
	Texture2D			m_textureCone;
	//csonkagula v�ge

	//csonkagula teto
	void DrawConeTop();
	VertexArrayObject	m_vao_cone_top;			// VAO objektum
	IndexBuffer			m_conetopBufferIndices;		// indexek
	ArrayBuffer			m_conetopBufferPos;	// poz�ci�k t�mbje
	ArrayBuffer			m_conetopBufferNormal;	// norm�lisok t�mbje
	ArrayBuffer			m_conetopBufferTex;	// text�rakoordin�t�k t�mbje
	std::vector<glm::vec3>		m_conetopPos{};
	std::vector<glm::vec3>		m_conetopNorm{};
	std::vector<glm::vec2>		m_conetopTex{};
	std::vector<int>			m_conetopTriangles{};
	Texture2D			m_textureConeTop;
	//csonkagulateto v�ge

	//henger
	void DrawCylinder(glm::mat4 &matWorld);
	VertexArrayObject	m_vao_cylinder;			// VAO objektum
	IndexBuffer			m_cylinderBufferIndices;		// indexek
	ArrayBuffer			m_cylinderBufferPos;	// poz�ci�k t�mbje
	ArrayBuffer			m_cylinderBufferNormal;	// norm�lisok t�mbje
	ArrayBuffer			m_cylinderBufferTex;	// text�rakoordin�t�k t�mbje
	std::vector<glm::vec3>		m_cylinderPos{};
	std::vector<glm::vec3>		m_cylinderNorm{};
	std::vector<glm::vec2>		m_cylinderTex{};
	std::vector<int>			m_cylinderTriangles{};
	Texture2D			m_textureCylinder;
	//henger v�ge

	//henger teteje
	//void DrawCylinderTop();
	VertexArrayObject	m_vao_cylinder_top;			// VAO objektum
	IndexBuffer			m_cylindertopBufferIndices;		// indexek
	ArrayBuffer			m_cylindertopBufferPos;	// poz�ci�k t�mbje
	ArrayBuffer			m_cylindertopBufferNormal;	// norm�lisok t�mbje
	ArrayBuffer			m_cylindertopBufferTex;	// text�rakoordin�t�k t�mbje
	std::vector<glm::vec3>		m_cylindertopPos{};
	std::vector<glm::vec3>		m_cylindertopNorm{};
	std::vector<glm::vec2>		m_cylindertopTex{};
	std::vector<int>			m_cylindertopTriangles{};
	Texture2D			m_textureCylinderTop;
	//henger teteje v�ge

	//henger alja
	//void DrawCylinderBottom();
	VertexArrayObject	m_vao_cylinder_bottom;			// VAO objektum
	IndexBuffer			m_cylinderbottomBufferIndices;		// indexek
	ArrayBuffer			m_cylinderbottomBufferPos;	// poz�ci�k t�mbje
	ArrayBuffer			m_cylinderbottomBufferNormal;	// norm�lisok t�mbje
	ArrayBuffer			m_cylinderbottomBufferTex;	// text�rakoordin�t�k t�mbje
	std::vector<glm::vec3>		m_cylinderbottomPos{};
	std::vector<glm::vec3>		m_cylinderbottomNorm{};
	std::vector<glm::vec2>		m_cylinderbottomTex{};
	std::vector<int>			m_cylinderbottomTriangles{};
	Texture2D			m_textureCylinderBottom;
	//henger alja v�ge

	void kapu();


	std::unique_ptr<Mesh>	m_mesh;

	gCamera				m_camera;

	glm::vec4			m_wallColor{ 1 };

	//skybox
	VertexArrayObject	m_skyboxVAO;			// VAO objektum
	IndexBuffer			m_skyboxIBO;		// indexek
	ArrayBuffer			m_skyboxVBO;	// poz�ci�k t�mbje
	GLuint				m_skyboxTexture;
	ProgramObject		m_programSkybox;	// skybox shaderek

	void TextureFromFileAttach(const char* filename, GLuint role) const;


};

