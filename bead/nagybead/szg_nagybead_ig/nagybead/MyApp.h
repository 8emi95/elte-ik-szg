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

#include "gCamera.h"
#include "gShaderProgram.h"
#include "gVertexBuffer.h"
#include "Mesh_OGL3.h"

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
	// belsõ eljárások

	// gömb parametrikus leírása
	glm::vec3 GetUV(int mit, float u, float v);
	glm::vec3 CMyApp::GetUV_normals(int mit, float u, float v);
	
	// a rajzoló metódusaink
	void CMyApp::DrawMasikBolygo(glm::vec3 pos);
	void DrawEarth(glm::vec3);
	void DrawSun();
	void DrawMoon(glm::vec3); // a "holdat" mindig máshova rajzoljuk; a paraméter a pozíció

	// Textúra erõforrás azonosítók
	GLuint m_earthTextureID;
	GLuint m_earthNormalMapID;
	GLuint m_moonTextureID;
	GLuint m_moonNormalMapID;
	GLuint m_masikTextureID;
	GLuint m_masikNormalMapID;
	GLuint m_sunTextureID;


	GLuint m_sziklaTextureID;
	GLuint m_sziklaNormalMapID;


	gCamera			m_camera;
	gShaderProgram	m_program;


	gVertexBuffer	m_vb_sphere;
	gVertexBuffer	m_vb_torusz; 
	gVertexBuffer	m_vb_szikla;

	const float TORUSZR = 1.01f;
	const float TORUSZV = 0.2f;
	int N = 50; // a gömbre és tóruszra vonatkozó felbontás
	int M = 50;

	const float NAPNYUJT = 50;
	const float FOLDNYUJT = 20;
	const float HOLDNYUJT = 25;
	const float MASIKNYUJT = 10;

	const float SZIKLAMERET = 1;



	bool isCamV = false;

};

