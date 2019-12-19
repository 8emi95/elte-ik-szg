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
#include <random>

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
	void initGround();
	void initSphere();
	void initDisc();
	void initCone();
	void initHalfSphere();
	void initCylinder();


	
	glm::vec3 GetDiscPos(float u, float v);
	glm::vec3 GetConePos(float u, float v);
	glm::vec3 GetHalfSpherePos(float u, float v);
	glm::vec3 GetCylinderPos(float u, float v);

	glm::vec3 GetUV(float u, float v);

	void renderAquarium();
	void renderPyramid(const glm::mat4 &world = glm::mat4(1.0f));
	void renderGoldfish(const glm::mat4 &world);
	void renderBubble(const glm::mat4 &world);
	void renderCube(const glm::mat4 &world, const Texture2D &tex);
	void renderDisc(const glm::mat4 &world, const Texture2D &tex);
	void renderCone(const glm::mat4 &world, const Texture2D &tex);
	void renderClosedCone(const glm::mat4 &world, const Texture2D &tex);
	void renderHalfSphere(const glm::mat4 &world, const Texture2D &tex);
	void renderClosedHalfSphere(const glm::mat4 &world, const Texture2D &tex);
	void renderCylinder(const glm::mat4 &world, const Texture2D &tex);






	// shaderekhez szükséges változók
	ProgramObject		m_program;		// shaderek programja
	ProgramObject		m_axesProgram;
	ProgramObject		m_pointProgram;

	Texture2D			m_textureMetal;
	Texture2D			m_textureWall;
	Texture2D			m_textureFish;
	Texture2D			m_textureEarth;
	Texture2D			m_textureShell;


	VertexArrayObject	m_vaoGround;			// VAO objektum
	VertexArrayObject	m_vaoSphere;// VAO objektum
	VertexArrayObject	m_vaoDisc;// VAO objektum
	VertexArrayObject	m_vaoCone;// VAO objektum
	VertexArrayObject	m_vaoHalfSphere;// VAO objektum
	VertexArrayObject	m_vaoCylinder;// VAO objektum

	std::unique_ptr<Mesh>	m_mesh;

	gCamera				m_camera;

	const int N = 20;
	const int M = 20;

	int fishNum = 10;
	float fishRadius = 10.0f;
	glm::vec3 lightColor = glm::vec3(0.6, 0.6, 0.2);
	glm::vec3 lightDir = glm::vec3(0, -1, 0);
	
	const int bubbleNum = 10;
	
	std::vector<glm::vec3> bubblePositions;

};

