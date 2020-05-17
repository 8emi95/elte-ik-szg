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

//#include "includes/gCamera.h"

#include "includes/ProgramObject.h"
#include "includes/BufferObject.h"
#include "includes/VertexArrayObject.h"
#include "includes/TextureObject.h"

// mesh
#include "includes/ObjParser_OGL3.h"
#include "SpaceShipCamera.h"

class CMyApp
{
public:
	CMyApp(void);
	~CMyApp(void);

	void initAkvarium();
	void initViz();

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
	ProgramObject		m_program;			// mesh shader
	ProgramObject		water_program;			// mesh shader

	//VertexArrayObject	m_CubeVao;			// VAO
	//IndexBuffer			m_CubeIndices;		// index buffer
	//ArrayBuffer			m_CubeVertexBuffer;	// VBO
	//VertexArrayObject	m_SkyboxVao;
	//IndexBuffer			m_SkyboxIndices;	
	//ArrayBuffer			m_SkyboxPos;		



	const float m = 3.0;
	const float h = 6.0;
	const int xsd = 60;
	const int zsd = 60;

	SpaceShipCamera		m_camera;

	Mesh*				akvarium;
	Mesh*				hal;
	Mesh*				korall;
	Mesh*				viz;

	Texture2D			waterTexture;
	Texture2D			halTexture;
	Texture2D			korallTexture;

	struct Vertex
	{
		glm::vec3 p;
		glm::vec3 n;
		glm::vec2 t;
	};

	// mesh ad

	// a jobb olvashatóság kedvéért
	void InitShaders();
};

