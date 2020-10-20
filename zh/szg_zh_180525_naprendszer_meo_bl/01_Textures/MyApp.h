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
	struct Vertex
	{
		glm::vec3 p;
		glm::vec3 n;
		glm::vec2 t;
	};
protected:
	// belsõ eljárások
	GLuint GenTexture();

	// segédfüggvények
	//KÖRLAPNAK (earth.png-s textúra)
	glm::vec3 GetPos(float u, float v);
	glm::vec3 GetNorm(float u, float v);
	glm::vec2 GetTex(float u, float v);

	//Félgömb
	glm::vec3 GetPosFelgomb(float u, float v);
	glm::vec3 GetNormFelgomb(float u, float v);
	glm::vec2 GetTexFelgomb(float u, float v);

	//Henger
	glm::vec3 GetPosHenger(float u, float v);
	glm::vec3 GetNormHenger(float u, float v);
	glm::vec2 GetTexHenger(float u, float v);

	glm::vec3 GetPosHengerFej(float u, float v);
	glm::vec3 GetNormHengerFej(float u, float v);
	glm::vec2 GetTexHengerFej(float u, float v);

	glm::vec3 GetPosHengerFarok(float u, float v);
	glm::vec3 GetNormHengerFarok(float u, float v);
	glm::vec2 GetTexHengerFarok(float u, float v);


	//Kiskor
	glm::vec3 GetPosKiskor(float u, float v);
	glm::vec3 GetNormKiskor(float u, float v);
	glm::vec2 GetTexKiskor(float u, float v);


	// shaderekhez szükséges változók
	GLuint m_programID; // shaderek programja

	// OpenGL-es dolgok
	GLuint m_vaoID; // vertex array object erõforrás azonosító
	GLuint m_vboID; // vertex buffer object erõforrás azonosító
	GLuint m_ibID;  // index buffer object erõforrás azonosító

	//KISKÖR- HENGEREK ALJA
	GLuint m_vaoKiskorID; // vertex array object erõforrás azonosító
	GLuint m_vboKiskorID; // vertex buffer object erõforrás azonosító
	GLuint m_ibKiskorID;  // index buffer object erõforrás azonosító


	//FELGOMB

	GLuint m_vaoFelgombID; 
	GLuint m_vboFelgombID; 
	GLuint m_ibFelgombID;  

	//HENGER

	GLuint m_vaoHengerID;
	GLuint m_vboHengerID;
	GLuint m_ibHengerID;
	//HENGER FEJ-FAROK
	GLuint m_vaoHengerFejID;
	GLuint m_vboHengerFejID;
	GLuint m_ibHengerFejID;

	GLuint m_vaoHengerFarokID;
	GLuint m_vboHengerFarokID;
	GLuint m_ibHengerFarokID;



	GLuint m_generatedTextureID; // generált textúra erõforrás azonosító
	//TEKNÕS, FÉLGÖMB
	GLuint m_shellTextureID; // generált textúra erõforrás azonosító
	//HENGER TEXTÚRA
	GLuint m_scalesTextureID;

	GLuint m_loadedTextureID; // betöltött textúra erõforrás azonosító
	GLuint m_samplerID; // sampler object azonosító

	// transzformációs mátrixok
	glm::mat4 m_matWorld;
	glm::mat4 m_matView;
	glm::mat4 m_matProj;

	// mátrixok helye a shaderekben
	GLuint	m_loc_mvp; // a három mátrixunk szorzatát adjuk át a hatékonyság érdekében
	GLuint  m_loc_w;
	GLuint  m_loc_wit;
	GLuint	m_loc_tex;


	// NxM darab négyszöggel közelítjük a parametrikus felületünket => (N+1)x(M+1) pontban kell kiértékelni
	static const int N	= 20;
	static const int M	= 20;
};

