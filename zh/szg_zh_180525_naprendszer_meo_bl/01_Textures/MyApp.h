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
	// bels� elj�r�sok
	GLuint GenTexture();

	// seg�df�ggv�nyek
	//K�RLAPNAK (earth.png-s text�ra)
	glm::vec3 GetPos(float u, float v);
	glm::vec3 GetNorm(float u, float v);
	glm::vec2 GetTex(float u, float v);

	//F�lg�mb
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


	// shaderekhez sz�ks�ges v�ltoz�k
	GLuint m_programID; // shaderek programja

	// OpenGL-es dolgok
	GLuint m_vaoID; // vertex array object er�forr�s azonos�t�
	GLuint m_vboID; // vertex buffer object er�forr�s azonos�t�
	GLuint m_ibID;  // index buffer object er�forr�s azonos�t�

	//KISK�R- HENGEREK ALJA
	GLuint m_vaoKiskorID; // vertex array object er�forr�s azonos�t�
	GLuint m_vboKiskorID; // vertex buffer object er�forr�s azonos�t�
	GLuint m_ibKiskorID;  // index buffer object er�forr�s azonos�t�


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



	GLuint m_generatedTextureID; // gener�lt text�ra er�forr�s azonos�t�
	//TEKN�S, F�LG�MB
	GLuint m_shellTextureID; // gener�lt text�ra er�forr�s azonos�t�
	//HENGER TEXT�RA
	GLuint m_scalesTextureID;

	GLuint m_loadedTextureID; // bet�lt�tt text�ra er�forr�s azonos�t�
	GLuint m_samplerID; // sampler object azonos�t�

	// transzform�ci�s m�trixok
	glm::mat4 m_matWorld;
	glm::mat4 m_matView;
	glm::mat4 m_matProj;

	// m�trixok helye a shaderekben
	GLuint	m_loc_mvp; // a h�rom m�trixunk szorzat�t adjuk �t a hat�konys�g �rdek�ben
	GLuint  m_loc_w;
	GLuint  m_loc_wit;
	GLuint	m_loc_tex;


	// NxM darab n�gysz�ggel k�zel�tj�k a parametrikus fel�let�nket => (N+1)x(M+1) pontban kell ki�rt�kelni
	static const int N	= 20;
	static const int M	= 20;
};

