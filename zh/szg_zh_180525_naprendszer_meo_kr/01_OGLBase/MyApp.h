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
#include <map>

#include "gCamera.h"

#include "ProgramObject.h"
#include "BufferObject.h"
#include "VertexArrayObject.h"
#include "TextureObject.h"

template<class T = uint32_t>
struct drawType
{
	drawType(int flag = GL_UNSIGNED_INT) : flag(flag) {}

	using type = T;
	int flag;
};

class CMyApp
{
	drawType<> dt{};

	enum Type
	{
		HalfSphere,
		Cylinder,
		Circle
	};

public:
	CMyApp(void);
	~CMyApp(void);

	bool Init();
	void Clean();

	glm::vec3 GetUVHalfSphere(float u, float v);
	glm::vec3 GetUVCylinder(float u, float v);

	std::vector<decltype(dt)::type> CMyApp::Triangulate(size_t N, size_t M);

	void Register(std::string object);
	void CreateGeometry();

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
	// segédeljárások
	void TextureFromFileAttach(const char* filename, GLuint role) const;

	// shaderekhez szükséges változók
	ProgramObject		m_program;			// shaderek programja
	ProgramObject		m_programSkybox;	// skybox shaderek

	struct obj
	{
		TextureObject<>		m_texture;
		VertexArrayObject	m_vao;				// VAO objektum
		IndexBuffer			m_gpuBufferIndices;	// indexek
		ArrayBuffer			m_gpuBuffer;		// pozíciók tömbje
		size_t idxNum;
		void draw(ProgramObject &program, glm::mat4 &proj, const glm::mat4 &world, int flag);
	};

	std::map<std::string, obj> objects;

	gCamera				m_camera;
	
	// nyers OGL azonosítók
	GLuint				m_skyboxTexture;
	
	struct Vertex
	{
		glm::vec3 pos;
		glm::vec3 norm;
		glm::vec2 uv;
	};

	unsigned int N = 20;
	unsigned int M = 10;
};

