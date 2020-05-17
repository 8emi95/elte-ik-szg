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
#include <string>

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
	GLuint m_programID; // shaderek programja

	// OpenGL-es dolgok
	GLuint m_vaoID; // vertex array object erõforrás azonosító
	GLuint m_vboID; // vertex buffer object erõforrás azonosító
	GLuint m_ibID;  // index buffer object erõforrás azonosító

	// transzformációs mátrixok
	glm::mat4 m_matWorld;
	glm::mat4 m_matView;
	glm::mat4 m_matProj;

	// mátrixok helye a shaderekben
	GLuint	m_loc_mvp; // a három mátrixunk szorzatát adjuk át a hatékonyság érdekében
	GLuint	c_loc_intp_end; //shadernek a szinErtek
	GLuint	f_loc_intp_val; //shadernek interpolációs érték

	struct Vertex
	{
		glm::vec3 p;
		glm::vec3 c;
	};

	glm::vec3 GetRandomGombPont();			//kiad a gömb besejébõl egy pontot
	glm::vec3 gombPontok[7];				//tömb a 7 random pontnak, ami alapján ki lehet rajzolni 7 oktaédert
	glm::vec3 szinErtek;					//a kiválasztott szín lesz
	unsigned int elozoIdo = 0;
	int szinInterpolaciosErtek;				//az aktuális interpolációs érték, [0-szinvaltasPeriodus] közötti 
	const int szinvaltasPeriodus = 5000;	//5 mp-es periódus
	bool gombLenyomva = false;				//1,2,3 gombok
};

