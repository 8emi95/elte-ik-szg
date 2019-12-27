#pragma once

#include <GL/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>

#include <vector>

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
		void Resize(int, int);

		void DrawGeometry(glm::vec3&);
		void DrawL(glm::vec3&, glm::mat4&, glm::mat4&);
	
	protected:

		std::vector<glm::vec3> CMyApp::VerticesOfCube(int);
		
		GLuint m_programID; 
		GLuint m_vaoID; 
		GLuint m_vboID; 
		GLuint m_ibID;  

		glm::mat4 m_matWorld;
		glm::mat4 m_matView;
		glm::mat4 m_matProj;
		GLuint	m_loc_mvp;

		GLuint cam_z;
		std::vector<glm::vec3> positionsOfElementsL;
		std::vector<glm::vec3> positions;
		glm::mat4x4 m_rotateAboutOwnX;

		bool shouldRotate;
		int unitSide;
		int cubeSide;

		struct Vertex
		{
			glm::vec3 p;
			glm::vec3 c;
		};
};

