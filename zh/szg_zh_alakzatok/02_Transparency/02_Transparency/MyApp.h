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
	// shaderekhez szükséges változók
	ProgramObject		m_program;		// shaderek programja

	Texture2D			m_textureMetal;

	VertexArrayObject	m_vao_ground;			// VAO objektum
	IndexBuffer			m_gpuBufferIndices;		// indexek
	ArrayBuffer			m_gpuBufferPos;	// pozíciók tömbje
	ArrayBuffer			m_gpuBufferNormal;	// normálisok tömbje
	ArrayBuffer			m_gpuBufferTex;	// textúrakoordináták tömbje

	void DrawGround();
	void DrawSuzanne();

	//gömbhöz szükséges változók/fvek
	glm::vec3 GetUV(float u, float v);
	glm::vec3 GetUV_henger(float u, float v);
	void DrawOrb();
	VertexArrayObject	m_vao_orb;			// VAO objektum
	IndexBuffer			m_orbBufferIndices;		// indexek
	ArrayBuffer			m_orbBufferPos;	// pozíciók tömbje
	ArrayBuffer			m_orbBufferNormal;	// normálisok tömbje
	ArrayBuffer			m_orbBufferTex;	// textúrakoordináták tömbje
	std::vector<glm::vec3>		m_orbPos{};
	std::vector<glm::vec3>		m_orbNorm{};
	std::vector<glm::vec2>		m_orbTex{};
	std::vector<int>			m_orbTriangles{};
	Texture2D			m_textureOrb;
	int N = 50; // a gömb felbontása
	int M = 50;
	//gömb vége

	//csonkagula
	void DrawCone();
	VertexArrayObject	m_vao_cone;			// VAO objektum
	IndexBuffer			m_coneBufferIndices;		// indexek
	ArrayBuffer			m_coneBufferPos;	// pozíciók tömbje
	ArrayBuffer			m_coneBufferNormal;	// normálisok tömbje
	ArrayBuffer			m_coneBufferTex;	// textúrakoordináták tömbje
	std::vector<glm::vec3>		m_conePos{};
	std::vector<glm::vec3>		m_coneNorm{};
	std::vector<glm::vec2>		m_coneTex{};
	std::vector<int>			m_coneTriangles{};
	Texture2D			m_textureCone;
	//csonkagula vége

	//csonkagula teto
	void DrawConeTop();
	VertexArrayObject	m_vao_cone_top;			// VAO objektum
	IndexBuffer			m_conetopBufferIndices;		// indexek
	ArrayBuffer			m_conetopBufferPos;	// pozíciók tömbje
	ArrayBuffer			m_conetopBufferNormal;	// normálisok tömbje
	ArrayBuffer			m_conetopBufferTex;	// textúrakoordináták tömbje
	std::vector<glm::vec3>		m_conetopPos{};
	std::vector<glm::vec3>		m_conetopNorm{};
	std::vector<glm::vec2>		m_conetopTex{};
	std::vector<int>			m_conetopTriangles{};
	Texture2D			m_textureConeTop;
	//csonkagulateto vége

	//henger
	void DrawCylinder(glm::mat4 &matWorld);
	VertexArrayObject	m_vao_cylinder;			// VAO objektum
	IndexBuffer			m_cylinderBufferIndices;		// indexek
	ArrayBuffer			m_cylinderBufferPos;	// pozíciók tömbje
	ArrayBuffer			m_cylinderBufferNormal;	// normálisok tömbje
	ArrayBuffer			m_cylinderBufferTex;	// textúrakoordináták tömbje
	std::vector<glm::vec3>		m_cylinderPos{};
	std::vector<glm::vec3>		m_cylinderNorm{};
	std::vector<glm::vec2>		m_cylinderTex{};
	std::vector<int>			m_cylinderTriangles{};
	Texture2D			m_textureCylinder;
	//henger vége

	//henger teteje
	//void DrawCylinderTop();
	VertexArrayObject	m_vao_cylinder_top;			// VAO objektum
	IndexBuffer			m_cylindertopBufferIndices;		// indexek
	ArrayBuffer			m_cylindertopBufferPos;	// pozíciók tömbje
	ArrayBuffer			m_cylindertopBufferNormal;	// normálisok tömbje
	ArrayBuffer			m_cylindertopBufferTex;	// textúrakoordináták tömbje
	std::vector<glm::vec3>		m_cylindertopPos{};
	std::vector<glm::vec3>		m_cylindertopNorm{};
	std::vector<glm::vec2>		m_cylindertopTex{};
	std::vector<int>			m_cylindertopTriangles{};
	Texture2D			m_textureCylinderTop;
	//henger teteje vége

	//henger alja
	//void DrawCylinderBottom();
	VertexArrayObject	m_vao_cylinder_bottom;			// VAO objektum
	IndexBuffer			m_cylinderbottomBufferIndices;		// indexek
	ArrayBuffer			m_cylinderbottomBufferPos;	// pozíciók tömbje
	ArrayBuffer			m_cylinderbottomBufferNormal;	// normálisok tömbje
	ArrayBuffer			m_cylinderbottomBufferTex;	// textúrakoordináták tömbje
	std::vector<glm::vec3>		m_cylinderbottomPos{};
	std::vector<glm::vec3>		m_cylinderbottomNorm{};
	std::vector<glm::vec2>		m_cylinderbottomTex{};
	std::vector<int>			m_cylinderbottomTriangles{};
	Texture2D			m_textureCylinderBottom;
	//henger alja vége

	void kapu();


	std::unique_ptr<Mesh>	m_mesh;

	gCamera				m_camera;

	glm::vec4			m_wallColor{ 1 };

	//skybox
	VertexArrayObject	m_skyboxVAO;			// VAO objektum
	IndexBuffer			m_skyboxIBO;		// indexek
	ArrayBuffer			m_skyboxVBO;	// pozíciók tömbje
	GLuint				m_skyboxTexture;
	ProgramObject		m_programSkybox;	// skybox shaderek

	void TextureFromFileAttach(const char* filename, GLuint role) const;


};

