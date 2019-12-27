#include "MyApp.h"
#include "GLUtils.hpp"

#include <math.h>

CMyApp::CMyApp(void)
{
	m_vaoID = 0;
	m_vboID = 0;
	m_ibID = 0;

	m_programID = 0;
}


CMyApp::~CMyApp(void)
{
}

bool CMyApp::Init()
{
	// törlési szín legyen kékes
	glClearColor(0.125f, 0.25f, 0.5f, 1.0f);

	//glEnable(GL_CULL_FACE); // kapcsoljuk be a hatrafele nezo lapok eldobasat
	glEnable(GL_DEPTH_TEST); // mélységi teszt bekapcsolása (takarás)
	//glCullFace(GL_BACK); // GL_BACK: a kamerától "elfelé" nézõ lapok, GL_FRONT: a kamera felé nézõ lapok

	//
	// geometria letrehozasa
	//

	Vertex vert[] =
	{ 
		//          x,  y, z             R, G, B
		//{glm::vec3(-1, -1, 0), glm::vec3(1, 0, 0)}, // bottom left
		//{glm::vec3( 1, -1, 0), glm::vec3(0, 1, 0)}, // bottom right
		//{glm::vec3(-1,  1, 0), glm::vec3(0, 0, 1)}, // top left
		//{glm::vec3( 1,  1, 0), glm::vec3(1, 1, 1)}, // top right

		/*
		{ glm::vec3(1, 3, 0), glm::vec3(1, 0, 0) },
		{ glm::vec3(0, 2, 0), glm::vec3(1, 0, 0) },
		{ glm::vec3(1, 2, 0), glm::vec3(1, 0, 0) },

		{ glm::vec3(1, 3, 0), glm::vec3(0, 1, 0) },
		{ glm::vec3(0, 3, 0), glm::vec3(0, 1, 0) },
		{ glm::vec3(0, 2, 0), glm::vec3(0, 1, 0) },
		// 1

		{ glm::vec3(2, 1, 0), glm::vec3(0, 0, 1) },
		{ glm::vec3(1, 0, 0), glm::vec3(0, 0, 1) },
		{ glm::vec3(2, 0, 0), glm::vec3(0, 0, 1) },

		{ glm::vec3(2, 1, 0), glm::vec3(1, 1, 0) },
		{ glm::vec3(1, 1, 0), glm::vec3(1, 1, 0) },
		{ glm::vec3(1, 0, 0), glm::vec3(1, 1, 0) },
		// 2

		{ glm::vec3(1, 2, 0), glm::vec3(1, 0, 1) },
		{ glm::vec3(1, 1, 0), glm::vec3(1, 0, 1) },
		{ glm::vec3(2, 1, 0), glm::vec3(1, 0, 1) },

		{ glm::vec3(2, 1, 0), glm::vec3(0, 1, 1) },
		{ glm::vec3(2, 2, 0), glm::vec3(0, 1, 1) },
		{ glm::vec3(1, 2, 0), glm::vec3(0, 1, 1) },
		// 3

		{ glm::vec3(2, 3, 0), glm::vec3(1, 1, 1) },
		{ glm::vec3(1, 3, 0), glm::vec3(1, 1, 1) },
		{ glm::vec3(1, 2, 0), glm::vec3(1, 1, 1) },

		{ glm::vec3(1, 2, 0), glm::vec3(0.5f, 0, 0) },
		{ glm::vec3(2, 2, 0), glm::vec3(0.5f, 0, 0) },
		{ glm::vec3(2, 3, 0), glm::vec3(0.5f, 0, 0) },
		// 4

		{ glm::vec3(3, 3, 0), glm::vec3(0, 0.5f, 0) },
		{ glm::vec3(2, 3, 0), glm::vec3(0, 0.5f, 0) },
		{ glm::vec3(2, 2, 0), glm::vec3(0, 0.5f, 0) },

		{ glm::vec3(2, 2, 0), glm::vec3(0, 0, 0.5f) },
		{ glm::vec3(3, 2, 0), glm::vec3(0, 0, 0.5f) },
		{ glm::vec3(3, 3, 0), glm::vec3(0, 0, 0.5f) },
		// 5

		{ glm::vec3(1, 1, -1), glm::vec3(0.5f, 0.5f, 0) },
		{ glm::vec3(1, 0, -1), glm::vec3(0.5f, 0.5f, 0) },
		{ glm::vec3(0, 0, -1), glm::vec3(0.5f, 0.5f, 0) },

		{ glm::vec3(0, 0, -1), glm::vec3(0.5f, 0, 0.5f) },
		{ glm::vec3(0, 1, -1), glm::vec3(0.5f, 0, 0.5f) },
		{ glm::vec3(1, 1, -1), glm::vec3(0.5f, 0, 0.5f) },
		// 6

		{ glm::vec3(2, 1, -1), glm::vec3(0, 0.5f, 0.5f) },
		{ glm::vec3(2, 0, -1), glm::vec3(0, 0.5f, 0.5f) },
		{ glm::vec3(1, 0, -1), glm::vec3(0, 0.5f, 0.5f) },

		{ glm::vec3(1, 0, -1), glm::vec3(0.5f, 0.5f, 0.5f) },
		{ glm::vec3(1, 1, -1), glm::vec3(0.5f, 0.5f, 0.5f) },
		{ glm::vec3(2, 1, -1), glm::vec3(0.5f, 0.5f, 0.5f) },
		// 7

		{ glm::vec3(1, 2, -1), glm::vec3(0.5f, 1, 1) },
		{ glm::vec3(2, 2, -1), glm::vec3(0.5f, 1, 1) },
		{ glm::vec3(2, 1, -1), glm::vec3(0.5f, 1, 1) },

		{ glm::vec3(2, 1, -1), glm::vec3(1, 0.5f, 1) },
		{ glm::vec3(1, 1, -1), glm::vec3(1, 0.5f, 1) },
		{ glm::vec3(1, 2, -1), glm::vec3(1, 0.5f, 1) },
		// 8

		{ glm::vec3(2, 3, -1), glm::vec3(1, 1, 0.5f) },
		{ glm::vec3(2, 2, -1), glm::vec3(1, 1, 0.5f) },
		{ glm::vec3(1, 2, -1), glm::vec3(1, 1, 0.5f) },

		{ glm::vec3(1, 2, -1), glm::vec3(0.5f, 0.5f, 1) },
		{ glm::vec3(1, 3, -1), glm::vec3(0.5f, 0.5f, 1) },
		{ glm::vec3(2, 3, -1), glm::vec3(0.5f, 0.5f, 1) },
		// 9

		{ glm::vec3(3, 3, -1), glm::vec3(0.5f, 1, 0.5f) },
		{ glm::vec3(3, 2, -1), glm::vec3(0.5f, 1, 0.5f) },
		{ glm::vec3(2, 2, -1), glm::vec3(0.5f, 1, 0.5f) },

		{ glm::vec3(2, 2, -1), glm::vec3(0.5f, 0.5f, 1) },
		{ glm::vec3(2, 3, -1), glm::vec3(0.5f, 0.5f, 1) },
		{ glm::vec3(3, 3, -1), glm::vec3(0.5f, 0.5f, 1) },
		// 10

		{ glm::vec3(0, 0, 0), glm::vec3(0.5f, 0.2f, 1) },
		{ glm::vec3(0, 1, 0), glm::vec3(0.5f, 0.2f, 1) },
		{ glm::vec3(0, 1, -1), glm::vec3(0.5f, 0.2f, 1) },

		{ glm::vec3(0, 1, -1), glm::vec3(0.5f, 0.2f, 0.2f) },
		{ glm::vec3(0, 0, -1), glm::vec3(0.5f, 0.2f, 0.2f) },
		{ glm::vec3(0, 0, 0), glm::vec3(0.5f, 0.2f, 0.2f) },
		// 11

		{ glm::vec3(0, 1, 0), glm::vec3(0.8f, 0.4f, 1) },
		{ glm::vec3(1, 1, 0), glm::vec3(0.8f, 0.4f, 1) },
		{ glm::vec3(1, 1, -1), glm::vec3(0.8f, 0.4f, 1) },

		{ glm::vec3(1, 1, -1), glm::vec3(0.5f, 1, 0.2f) },
		{ glm::vec3(0, 1, -1), glm::vec3(0.5f, 1, 0.2f) },
		{ glm::vec3(0, 1, 0), glm::vec3(0.5f, 1, 0.2f) },
		// 12

		{ glm::vec3(1, 1, 0), glm::vec3(0.5f, 0, 1) },
		{ glm::vec3(1, 2, 0), glm::vec3(0.5f, 0, 1) },
		{ glm::vec3(1, 2, -1), glm::vec3(0.5f, 0, 1) },

		{ glm::vec3(1, 2, -1), glm::vec3(0.5f, 1, 0) },
		{ glm::vec3(1, 1, -1), glm::vec3(0.5f, 1, 0) },
		{ glm::vec3(1, 1, 0), glm::vec3(0.5f, 1, 0) },
		// 13

		{ glm::vec3(1, 2, 0), glm::vec3(0, 0.5f, 1) },
		{ glm::vec3(1, 3, 0), glm::vec3(0, 0.5f, 1) },
		{ glm::vec3(1, 3, -1), glm::vec3(0, 0.5f, 1) },

		{ glm::vec3(1, 3, -1), glm::vec3(1, 0.5f, 0) },
		{ glm::vec3(1, 2, -1), glm::vec3(1, 0.5f, 0) },
		{ glm::vec3(1, 2, 0), glm::vec3(1, 0.5f, 0) },
		// 14

		{ glm::vec3(1, 3, 0), glm::vec3(0, 1, 0.5f) },
		{ glm::vec3(2, 3, 0), glm::vec3(0, 1, 0.5f) },
		{ glm::vec3(2, 3, -1), glm::vec3(0, 1, 0.5f) },

		{ glm::vec3(2, 3, -1), glm::vec3(1, 0, 0.5f) },
		{ glm::vec3(1, 3, -1), glm::vec3(1, 0, 0.5f) },
		{ glm::vec3(1, 3, 0), glm::vec3(1, 0, 0.5f) },
		// 15

		{ glm::vec3(2, 3, 0), glm::vec3(1, 0.75f, 1) },
		{ glm::vec3(3, 3, 0), glm::vec3(1, 0.75f, 1) },
		{ glm::vec3(3, 3, -1), glm::vec3(1, 0.75f, 1) },

		{ glm::vec3(3, 3, -1), glm::vec3(0.75f, 1, 1) },
		{ glm::vec3(2, 3, -1), glm::vec3(0.75f, 1, 1) },
		{ glm::vec3(2, 3, 0), glm::vec3(0.75f, 1, 1) },
		// 16

		{ glm::vec3(3, 2, -1), glm::vec3(0, 0.75f, 0) },
		{ glm::vec3(3, 3, -1), glm::vec3(0, 0.75f, 0) },
		{ glm::vec3(3, 3, 0), glm::vec3(0, 0.75f, 0) },

		{ glm::vec3(3, 3, 0), glm::vec3(0, 0, 0.75f) },
		{ glm::vec3(3, 2, 0), glm::vec3(0, 0, 0.75f) },
		{ glm::vec3(3, 2, -1), glm::vec3(0, 0, 0.75f) },
		// 17

		{ glm::vec3(3, 2, -1), glm::vec3(1, 1, 0.75f) },
		{ glm::vec3(3, 2, 0), glm::vec3(1, 1, 0.75f) },
		{ glm::vec3(2, 2, 0), glm::vec3(1, 1, 0.75f) },

		{ glm::vec3(2, 2, 0), glm::vec3(0.75f, 0, 0) },
		{ glm::vec3(2, 2, -1), glm::vec3(0.75f, 0, 0) },
		{ glm::vec3(3, 2, -1), glm::vec3(0.75f, 0, 0) },
		// 18

		{ glm::vec3(2, 2, -1), glm::vec3(0.75f, 0, 1) },
		{ glm::vec3(2, 2, 0), glm::vec3(0.75f, 0, 1) },
		{ glm::vec3(2, 1, 0), glm::vec3(0.75f, 0, 1) },

		{ glm::vec3(2, 1, 0), glm::vec3(0.75f, 1, 0) },
		{ glm::vec3(2, 1, -1), glm::vec3(0.75f, 1, 0) },
		{ glm::vec3(2, 2, -1), glm::vec3(0.75f, 1, 0) },
		// 19

		{ glm::vec3(2, 1, -1), glm::vec3(0, 0.75f, 1) },
		{ glm::vec3(2, 1, 0), glm::vec3(0, 0.75f, 1) },
		{ glm::vec3(2, 0, 0), glm::vec3(0, 0.75f, 1) },

		{ glm::vec3(2, 0, 0), glm::vec3(1, 0.75f, 0) },
		{ glm::vec3(2, 0, -1), glm::vec3(1, 0.75f, 0) },
		{ glm::vec3(2, 1, -1), glm::vec3(1, 0.75f, 0) },
		// 20

		{ glm::vec3(2, 0, -1), glm::vec3(0, 1, 0.75f) },
		{ glm::vec3(2, 0, 0), glm::vec3(0, 1, 0.75f) },
		{ glm::vec3(1, 0, 0), glm::vec3(0, 1, 0.75f) },

		{ glm::vec3(1, 0, 0), glm::vec3(1, 0, 0.75f) },
		{ glm::vec3(1, 0, -1), glm::vec3(1, 0, 0.75f) },
		{ glm::vec3(2, 0, -1), glm::vec3(1, 0, 0.75f) },
		// 21

		{ glm::vec3(1, 0, -1), glm::vec3(0.75f, 0.75f, 0) },
		{ glm::vec3(1, 0, 0), glm::vec3(0.75f, 0.75f, 0) },
		{ glm::vec3(0, 0, 0), glm::vec3(0.75f, 0.75f, 0) },

		{ glm::vec3(0, 0, 0), glm::vec3(0.75f, 0.75f, 0.75f) },
		{ glm::vec3(0, 0, -1), glm::vec3(0.75f, 0.75f, 0.75f) },
		{ glm::vec3(1, 0, -1), glm::vec3(0.75f, 0.75f, 0.75f) },
		// 22
		*/
		// top right cube:

		// top square
		{glm::vec3(-1, 2, 1), glm::vec3(1, 0, 0)}, // 0
		{ glm::vec3(0,  2,  1), glm::vec3(1, 0, 0) }, // 1
		{ glm::vec3(-1,  2,  0), glm::vec3(1, 0, 0) }, // 2

		{ glm::vec3(0,  2,  1), glm::vec3(0, 1, 0) }, // 1
		{ glm::vec3(-1,  2,  0), glm::vec3(0, 1, 0) }, // 2
		{ glm::vec3(0,  2,  0), glm::vec3(0, 1, 0) }, // 3

		// bottom square
		{ glm::vec3(-1,  1,  1), glm::vec3(0, 0, 1) }, // 4
		{ glm::vec3(-1,  1,  0), glm::vec3(0, 0, 1) }, // 6
		{ glm::vec3(0,  1,  0), glm::vec3(0, 0, 1) }, // 7

		{ glm::vec3(-1,  1,  1), glm::vec3(1, 1, 1) }, // 4
		{ glm::vec3(0,  1,  1), glm::vec3(1, 1, 1) }, // 5
		{ glm::vec3(0,  1,  0), glm::vec3(1, 1, 1) }, // 7

		// rear square
		{ glm::vec3(0,  2,  1), glm::vec3(1, 0, 0) }, // 1
		{ glm::vec3(-1,  1,  1), glm::vec3(1, 0, 0) }, // 4
		{ glm::vec3(0,  1,  1), glm::vec3(1, 0, 0) }, // 5

		{ glm::vec3(-1,  2,  1), glm::vec3(0, 1, 0) }, // 0
		{ glm::vec3(0,  2,  1), glm::vec3(0, 1, 0) }, // 1
		{ glm::vec3(-1,  1,  1), glm::vec3(0, 1, 0) }, // 4

		// front square
		{ glm::vec3(-1,  2,  0), glm::vec3(0, 0, 1) }, // 2
		{ glm::vec3(0,  2,  0), glm::vec3(0, 0, 1) }, // 3
		{ glm::vec3(-1,  1,  0), glm::vec3(0, 0, 1) }, // 6

		{ glm::vec3(0,  2,  0), glm::vec3(1, 1, 1) }, // 3
		{ glm::vec3(-1,  1,  0), glm::vec3(1, 1, 1) }, // 6
		{ glm::vec3(0,  1,  0), glm::vec3(1, 1, 1) }, // 7

		// left square
		{ glm::vec3(0,  2,  1), glm::vec3(1, 0, 0) }, // 1
		{ glm::vec3(0,  2,  0), glm::vec3(1, 0, 0) }, // 3
		{ glm::vec3(0,  1,  0), glm::vec3(1, 0, 0) }, // 7

		{ glm::vec3(0,  2,  1), glm::vec3(0, 1, 0) }, // 1
		{ glm::vec3(0,  1,  1), glm::vec3(0, 1, 0) }, // 5
		{ glm::vec3(0,  1,  0), glm::vec3(0, 1, 0) }, // 7

		// right square
		{ glm::vec3(-1,  2,  1), glm::vec3(0, 0, 1) }, // 0
		{ glm::vec3(-1,  2,  0), glm::vec3(0, 0, 1) }, // 2
		{ glm::vec3(-1,  1,  1), glm::vec3(0, 0, 1) }, // 4

		{ glm::vec3(-1,  2,  0), glm::vec3(1, 1, 1) }, // 2
		{ glm::vec3(-1,  1,  1), glm::vec3(1, 1, 1) }, // 4
		{ glm::vec3(-1,  1,  0), glm::vec3(1, 1, 1) }, // 6


		// top middle cube:

		// top square
		{ glm::vec3(0,  2,  1), glm::vec3(1, 0, 0) }, // 1
		{ glm::vec3(0,  2,  0), glm::vec3(1, 0, 0) }, // 3
		{ glm::vec3(1,  2,  1), glm::vec3(1, 0, 0) }, // 8

		{ glm::vec3(0,  2,  0), glm::vec3(0, 1, 0) }, // 3
		{ glm::vec3(1,  2,  1), glm::vec3(0, 1, 0) }, // 8
		{ glm::vec3(1,  2,  0), glm::vec3(0, 1, 0) }, // 10

		// bottom square
		{ glm::vec3(0,  1,  1), glm::vec3(0, 0, 1) }, // 5
		{ glm::vec3(0,  1,  0), glm::vec3(0, 0, 1) }, // 7
		{ glm::vec3(1,  1,  1), glm::vec3(0, 0, 1) }, // 12

		{ glm::vec3(0,  1,  0), glm::vec3(1, 1, 1) }, // 7
		{ glm::vec3(1,  1,  1), glm::vec3(1, 1, 1) }, // 12
		{ glm::vec3(1,  1,  0), glm::vec3(1, 1, 1) }, // 14

		// rear square
		{ glm::vec3(0,  2,  1), glm::vec3(1, 0, 0) }, // 1
		{ glm::vec3(0,  1,  1), glm::vec3(1, 0, 0) }, // 5
		{ glm::vec3(1,  2,  1), glm::vec3(1, 0, 0) }, // 8

		{ glm::vec3(0,  1,  1), glm::vec3(0, 1, 0) }, // 5
		{ glm::vec3(1,  2,  1), glm::vec3(0, 1, 0) }, // 8
		{ glm::vec3(1,  1,  1), glm::vec3(0, 1, 0) }, // 12

		// front square
		{ glm::vec3(0,  2,  0), glm::vec3(0, 0, 1) }, // 3
		{ glm::vec3(0,  1,  0), glm::vec3(0, 0, 1) }, // 7
		{ glm::vec3(1,  2,  0), glm::vec3(0, 0, 1) }, // 10

		{ glm::vec3(0,  1,  0), glm::vec3(1, 1, 1) }, // 7
		{ glm::vec3(1,  2,  0), glm::vec3(1, 1, 1) }, // 10
		{ glm::vec3(1,  1,  0), glm::vec3(1, 1, 1) }, // 14

		// left square = top right cube right square
		{ glm::vec3(-1,  2,  1), glm::vec3(0, 0, 1) }, // 0
		{ glm::vec3(-1,  2,  0), glm::vec3(0, 0, 1) }, // 2
		{ glm::vec3(-1,  1,  1), glm::vec3(0, 0, 1) }, // 4

		{ glm::vec3(-1,  2,  0), glm::vec3(1, 1, 1) }, // 2
		{ glm::vec3(-1,  1,  1), glm::vec3(1, 1, 1) }, // 4
		{ glm::vec3(-1,  1,  0), glm::vec3(1, 1, 1) }, // 6

		// right square
		{ glm::vec3(1,  2,  1), glm::vec3(1, 0, 0) }, // 8
		{ glm::vec3(1,  2,  0), glm::vec3(1, 0, 0) }, // 10
		{ glm::vec3(1,  1,  1), glm::vec3(1, 0, 0) }, // 12

		{ glm::vec3(1,  2,  0), glm::vec3(0, 1, 0) }, // 10
		{ glm::vec3(1,  1,  1), glm::vec3(0, 1, 0) }, // 12
		{ glm::vec3(1,  1,  0), glm::vec3(0, 1, 0) }, // 14


		// top left cube:

		// top square
		{ glm::vec3(1,  2,  1), glm::vec3(0, 0, 1) }, // 8
		{ glm::vec3(2,  2,  1), glm::vec3(0, 0, 1) }, // 9
		{ glm::vec3(1,  2,  0), glm::vec3(0, 0, 1) }, // 10

		{ glm::vec3(2,  2,  1), glm::vec3(1, 1, 1) }, // 9
		{ glm::vec3(1,  2,  0), glm::vec3(1, 1, 1) }, // 10
		{ glm::vec3(2,  2,  0), glm::vec3(1, 1, 1) }, // 11

		// bottom square
		{ glm::vec3(1,  1,  1), glm::vec3(1, 0, 0) }, // 12
		{ glm::vec3(2,  1,  1), glm::vec3(1, 0, 0) }, // 13
		{ glm::vec3(1,  1,  0), glm::vec3(1, 0, 0) }, // 14

		{ glm::vec3(2,  1,  1), glm::vec3(0, 1, 0) }, // 13
		{ glm::vec3(1,  1,  0), glm::vec3(0, 1, 0) }, // 14
		{ glm::vec3(2,  1,  0), glm::vec3(0, 1, 0) }, // 15

		// rear square
		{ glm::vec3(1,  2,  1), glm::vec3(0, 0, 1) }, // 8
		{ glm::vec3(2,  2,  1), glm::vec3(0, 0, 1) }, // 9
		{ glm::vec3(1,  1,  1), glm::vec3(0, 0, 1) }, // 12

		{ glm::vec3(2,  2,  1), glm::vec3(1, 1, 1) }, // 9
		{ glm::vec3(1,  1,  1), glm::vec3(1, 1, 1) }, // 12
		{ glm::vec3(2,  1,  1), glm::vec3(1, 1, 1) }, // 13

		// front square
		{ glm::vec3(1,  2,  0), glm::vec3(1, 0, 0) }, // 10
		{ glm::vec3(2,  2,  0), glm::vec3(1, 0, 0) }, // 11
		{ glm::vec3(1,  1,  0), glm::vec3(1, 0, 0) }, // 14

		{ glm::vec3(2,  2,  0), glm::vec3(0, 1, 0) }, // 11
		{ glm::vec3(1,  1,  0), glm::vec3(0, 1, 0) }, // 14
		{ glm::vec3(2,  1,  0), glm::vec3(0, 1, 0) }, // 15

		// left square = top middle cube right square
		{ glm::vec3(1,  2,  1), glm::vec3(1, 0, 0) }, // 8
		{ glm::vec3(1,  2,  0), glm::vec3(1, 0, 0) }, // 10
		{ glm::vec3(1,  1,  1), glm::vec3(1, 0, 0) }, // 12

		{ glm::vec3(1,  2,  0), glm::vec3(0, 1, 0) }, // 10
		{ glm::vec3(1,  1,  1), glm::vec3(0, 1, 0) }, // 12
		{ glm::vec3(1,  1,  0), glm::vec3(0, 1, 0) }, // 14

		// right square
		{ glm::vec3(2,  2,  1), glm::vec3(0, 0, 1) }, // 9
		{ glm::vec3(2,  2,  0), glm::vec3(0, 0, 1) }, // 11
		{ glm::vec3(2,  1,  1), glm::vec3(0, 0, 1) }, // 13

		{ glm::vec3(2,  2,  0), glm::vec3(1, 1, 1) }, // 11
		{ glm::vec3(2,  1,  1), glm::vec3(1, 1, 1) }, // 13
		{ glm::vec3(2,  1,  0), glm::vec3(1, 1, 1) }, // 15

		// middle middle cube:

		// top square = top middle cube bottom square
		{ glm::vec3(0,  1,  1), glm::vec3(0, 0, 1) }, // 5
		{ glm::vec3(0,  1,  0), glm::vec3(0, 0, 1) }, // 7
		{ glm::vec3(1,  1,  1), glm::vec3(0, 0, 1) }, // 12

		{ glm::vec3(0,  1,  0), glm::vec3(1, 1, 1) }, // 7
		{ glm::vec3(1,  1,  1), glm::vec3(1, 1, 1) }, // 12
		{ glm::vec3(1,  1,  0), glm::vec3(1, 1, 1) }, // 14

		// bottom square
		{ glm::vec3(0,  0,  1), glm::vec3(1, 0, 0) }, // 16
		{ glm::vec3(1,  0,  1), glm::vec3(1, 0, 0) }, // 17
		{ glm::vec3(0,  0,  0), glm::vec3(1, 0, 0) }, // 18

		{ glm::vec3(1,  0,  1), glm::vec3(0, 1, 0) }, // 17
		{ glm::vec3(0,  0,  0), glm::vec3(0, 1, 0) }, // 18
		{ glm::vec3(1,  0,  0), glm::vec3(0, 1, 0) }, // 19

		// rear square
		{ glm::vec3(0,  1,  1), glm::vec3(0, 0, 1) }, // 5
		{ glm::vec3(1,  1,  1), glm::vec3(0, 0, 1) }, // 12
		{ glm::vec3(0,  0,  1), glm::vec3(0, 0, 1) }, // 16

		{ glm::vec3(1,  1,  1), glm::vec3(1, 1, 1) }, // 12
		{ glm::vec3(0,  0,  1), glm::vec3(1, 1, 1) }, // 16
		{ glm::vec3(1,  0,  1), glm::vec3(1, 1, 1) }, // 17

		// front square
		{ glm::vec3(0,  1,  0), glm::vec3(1, 0, 0) }, // 7
		{ glm::vec3(1,  1,  0), glm::vec3(1, 0, 0) }, // 14
		{ glm::vec3(0,  0,  0), glm::vec3(1, 0, 0) }, // 18

		{ glm::vec3(1,  1,  0), glm::vec3(0, 1, 0) }, // 14
		{ glm::vec3(0,  0,  0), glm::vec3(0, 1, 0) }, // 18
		{ glm::vec3(1,  0,  0), glm::vec3(0, 1, 0) }, // 19

		// left square
		{ glm::vec3(0,  1,  1), glm::vec3(0, 0, 1) }, // 5
		{ glm::vec3(0,  1,  0), glm::vec3(0, 0, 1) }, // 7
		{ glm::vec3(0,  0,  1), glm::vec3(0, 0, 1) }, // 16

		{ glm::vec3(0,  1,  0), glm::vec3(1, 1, 1) }, // 7
		{ glm::vec3(0,  0,  1), glm::vec3(1, 1, 1) }, // 16
		{ glm::vec3(0,  0,  0), glm::vec3(1, 1, 1) }, // 18

		// right square
		{ glm::vec3(1,  1,  1), glm::vec3(1, 0, 0) }, // 12
		{ glm::vec3(1,  1,  0), glm::vec3(1, 0, 0) }, // 14
		{ glm::vec3(1,  0,  1), glm::vec3(1, 0, 0) }, // 17

		{ glm::vec3(1,  1,  0), glm::vec3(0, 1, 0) }, // 14
		{ glm::vec3(1,  0,  1), glm::vec3(0, 1, 0) }, // 17
		{ glm::vec3(1,  0,  0), glm::vec3(0, 1, 0) }, // 19


		// bottom middle cube:

		// top square = middle middle cube bottom square
		{ glm::vec3(0,  0,  1), glm::vec3(1, 0, 0) }, // 16
		{ glm::vec3(1,  0,  1), glm::vec3(1, 0, 0) }, // 17
		{ glm::vec3(0,  0,  0), glm::vec3(1, 0, 0) }, // 18

		{ glm::vec3(1,  0,  1), glm::vec3(0, 1, 0) }, // 17
		{ glm::vec3(0,  0,  0), glm::vec3(0, 1, 0) }, // 18
		{ glm::vec3(1,  0,  0), glm::vec3(0, 1, 0) }, // 19

		// bottom square
		{ glm::vec3(0, -1,  1), glm::vec3(0, 0, 1) }, // 20
		{ glm::vec3(1, -1,  1), glm::vec3(0, 0, 1) }, // 21
		{ glm::vec3(0, -1,  0), glm::vec3(0, 0, 1) }, // 22

		{ glm::vec3(1, -1,  1), glm::vec3(1, 1, 1) }, // 21
		{ glm::vec3(0, -1,  0), glm::vec3(1, 1, 1) }, // 22
		{ glm::vec3(1, -1,  0), glm::vec3(1, 1, 1) }, // 23

		// rear square
		{ glm::vec3(0,  0,  1), glm::vec3(1, 0, 0) }, // 16
		{ glm::vec3(1,  0,  1), glm::vec3(1, 0, 0) }, // 17
		{ glm::vec3(0, -1,  1), glm::vec3(1, 0, 0) }, // 20

		{ glm::vec3(1,  0,  1), glm::vec3(0, 1, 0) }, // 17
		{ glm::vec3(0, -1,  1), glm::vec3(0, 1, 0) }, // 20
		{ glm::vec3(1, -1,  1), glm::vec3(0, 1, 0) }, // 21

		// front square
		{ glm::vec3(0,  0,  0), glm::vec3(0, 0, 1) }, // 18
		{ glm::vec3(1,  0,  0), glm::vec3(0, 0, 1) }, // 19
		{ glm::vec3(0, -1,  0), glm::vec3(0, 0, 1) }, // 22

		{ glm::vec3(1,  0,  0), glm::vec3(1, 1, 1) }, // 19
		{ glm::vec3(0, -1,  0), glm::vec3(1, 1, 1) }, // 22
		{ glm::vec3(1, -1,  0), glm::vec3(1, 1, 1) }, // 23

		// left square
		{ glm::vec3(0,  0,  1), glm::vec3(1, 0, 0) }, // 16
		{ glm::vec3(0,  0,  0), glm::vec3(1, 0, 0) }, // 18
		{ glm::vec3(0, -1,  1), glm::vec3(1, 0, 0) }, // 20

		{ glm::vec3(0,  0,  0), glm::vec3(0, 1, 0) }, // 18
		{ glm::vec3(0, -1,  1), glm::vec3(0, 1, 0) }, // 20
		{ glm::vec3(0, -1,  0), glm::vec3(0, 1, 0) }, // 22

		// right square
		{ glm::vec3(1,  0,  1), glm::vec3(0, 0, 1) }, // 17
		{ glm::vec3(1,  0,  0), glm::vec3(0, 0, 1) }, // 19
		{ glm::vec3(1, -1,  1), glm::vec3(0, 0, 1) }, // 21

		{ glm::vec3(1,  0,  0), glm::vec3(1, 1, 1) }, // 19
		{ glm::vec3(1, -1,  1), glm::vec3(1, 1, 1) }, // 21
		{ glm::vec3(1, -1,  0), glm::vec3(1, 1, 1) }, // 23

	};

	// indexpuffer adatai
    GLushort indices[]=
    {
		// 1. háromszög
        //0,1,2,
		// 2. háromszög
        //2,1,3,

		0,1,2,
		3,4,5,
		6,7,8,
		9,10,11,
		12,13,14,
		15,16,17,
		18,19,20,
		21,22,23,
		24,25,26,
		27,28,29,
		30,31,32,
		33,34,35,
		36,37,38,
		39,40,41,
		42,43,44,
		45,46,47,
		48,49,50,
		51,52,53,
		54,55,56,
		57,58,59,
		60,61,62,
		63,64,65,
		66,67,68,
		69,70,71,
		72,73,74,
		75,76,77,
		78,79,80,
		81,82,83,
		84,85,86,
		87,88,89,
		90,91,92,
		93,94,95,
		96,97,98,
		99,100,101,
		102,103,104,
		105,106,107,
		108,109,110,
		111,112,113,
		114,115,116,
		117,118,119,
		120,121,122,
		123,124,125,
		126,127,128,
		129,130,131,
		132,133,134,
		135,136,137,
		138,139,140,
		141,142,143,
		144,145,146,
		147,148,149,
		150,151,152,
		153,154,155,
		156,157,158,
		159,160,161,
		162,163,164,
		165,166,167,
		168,169,170,
		171,172,173,
		174,175,176,
		177,178,179,

    };

	// 1 db VAO foglalasa
	glGenVertexArrays(1, &m_vaoID);
	// a frissen generált VAO beallitasa aktívnak
	glBindVertexArray(m_vaoID);
	
	// hozzunk létre egy új VBO erõforrás nevet
	glGenBuffers(1, &m_vboID); 
	glBindBuffer(GL_ARRAY_BUFFER, m_vboID); // tegyük "aktívvá" a létrehozott VBO-t
	// töltsük fel adatokkal az aktív VBO-t
	glBufferData( GL_ARRAY_BUFFER,	// az aktív VBO-ba töltsünk adatokat
				  sizeof(vert),		// ennyi bájt nagyságban
				  vert,	// errõl a rendszermemóriabeli címrõl olvasva
				  GL_STATIC_DRAW);	// úgy, hogy a VBO-nkba nem tervezünk ezután írni és minden kirajzoláskor felhasnzáljuk a benne lévõ adatokat
	

	// VAO-ban jegyezzük fel, hogy a VBO-ban az elsõ 3 float sizeof(Vertex)-enként lesz az elsõ attribútum (pozíció)
	glEnableVertexAttribArray(0); // ez lesz majd a pozíció
	glVertexAttribPointer(
		0,				// a VB-ben található adatok közül a 0. "indexû" attribútumait állítjuk be
		3,				// komponens szam
		GL_FLOAT,		// adatok tipusa
		GL_FALSE,		// normalizalt legyen-e
		sizeof(Vertex),	// stride (0=egymas utan)
		0				// a 0. indexû attribútum hol kezdõdik a sizeof(Vertex)-nyi területen belül
	); 

	// a második attribútumhoz pedig a VBO-ban sizeof(Vertex) ugrás után sizeof(glm::vec3)-nyit menve újabb 3 float adatot találunk (szín)
	glEnableVertexAttribArray(1); // ez lesz majd a szín
	glVertexAttribPointer(
		1,
		3, 
		GL_FLOAT,
		GL_FALSE,
		sizeof(Vertex),
		(void*)(sizeof(glm::vec3)) );

	// index puffer létrehozása
	glGenBuffers(1, &m_ibID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindVertexArray(0); // feltöltüttük a VAO-t, kapcsoljuk le
	glBindBuffer(GL_ARRAY_BUFFER, 0); // feltöltöttük a VBO-t is, ezt is vegyük le
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // feltöltöttük a VBO-t is, ezt is vegyük le

	//
	// shaderek betöltése
	//
	GLuint vs_ID = loadShader(GL_VERTEX_SHADER,		"myVert.vert");
	GLuint fs_ID = loadShader(GL_FRAGMENT_SHADER,	"myFrag.frag");

	// a shadereket tároló program létrehozása
	m_programID = glCreateProgram();

	// adjuk hozzá a programhoz a shadereket
	glAttachShader(m_programID, vs_ID);
	glAttachShader(m_programID, fs_ID);

	// VAO-beli attribútumok hozzárendelése a shader változókhoz
	// FONTOS: linkelés elõtt kell ezt megtenni!
	glBindAttribLocation(	m_programID,	// shader azonosítója, amibõl egy változóhoz szeretnénk hozzárendelést csinálni
							0,				// a VAO-beli azonosító index
							"vs_in_pos");	// a shader-beli változónév
	glBindAttribLocation( m_programID, 1, "vs_in_col");

	// illesszük össze a shadereket (kimenõ-bemenõ változók összerendelése stb.)
	glLinkProgram(m_programID);

	// linkeles ellenorzese
	GLint infoLogLength = 0, result = 0;

	glGetProgramiv(m_programID, GL_LINK_STATUS, &result);
	glGetProgramiv(m_programID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if ( GL_FALSE == result )
	{
		std::vector<char> ProgramErrorMessage( infoLogLength );
		glGetProgramInfoLog(m_programID, infoLogLength, NULL, &ProgramErrorMessage[0]);
		fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);
		
		char* aSzoveg = new char[ProgramErrorMessage.size()];
		memcpy( aSzoveg, &ProgramErrorMessage[0], ProgramErrorMessage.size());

		std::cout << "[app.Init()] Sáder Huba panasza: " << aSzoveg << std::endl;

		delete aSzoveg;
	}

	// mar nincs ezekre szukseg
	glDeleteShader( vs_ID );
	glDeleteShader( fs_ID );

	//
	// egyéb inicializálás
	//

	// vetítési mátrix létrehozása
	m_matProj = glm::perspective( 45.0f, 640/480.0f, 1.0f, 1000.0f );

	// shader-beli transzformációs mátrixok címének lekérdezése
	m_loc_mvp = glGetUniformLocation( m_programID, "MVP");

	return true;
}

void CMyApp::Clean()
{
	glDeleteBuffers(1, &m_vboID);
	glDeleteBuffers(1, &m_ibID);
	glDeleteVertexArrays(1, &m_vaoID);

	glDeleteProgram( m_programID );
}

void CMyApp::Update()
{
	// nézeti transzformáció beállítása
	m_matView = glm::lookAt(glm::vec3( 5,  0,  5),		// honnan nézzük a színteret
							glm::vec3( 0,  0,  0),		// a színtér melyik pontját nézzük
							glm::vec3( 0,  1,  0));		// felfelé mutató irány a világban
}


void CMyApp::Render()
{
	// töröljük a frampuffert (GL_COLOR_BUFFER_BIT) és a mélységi Z puffert (GL_DEPTH_BUFFER_BIT)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// shader bekapcsolasa
	glUseProgram( m_programID );

	// shader parameterek beállítása
	/*

	GLM transzformációs mátrixokra példák:
		glm::rotate<float>( szög, glm::vec3(tengely_x, tengely_y, tengely_z) ) <- tengely_{xyz} körüli elforgatás
		glm::translate<float>( glm::vec3(eltol_x, eltol_y, eltol_z) ) <- eltolás
		glm::scale<float>( glm::vec3(s_x, s_y, s_z) ) <- léptékezés

	*/
	m_matWorld = glm::mat4(1.0f);

	glm::mat4 mvp = m_matProj * m_matView * m_matWorld;

	// majd küldjük át a megfelelõ mátrixot!
	glUniformMatrix4fv( m_loc_mvp,// erre a helyre töltsünk át adatot
						1,			// egy darab mátrixot
						GL_FALSE,	// NEM transzponálva
						&(mvp[0][0]) ); // innen olvasva a 16 x sizeof(float)-nyi adatot

	// kapcsoljuk be a VAO-t (a VBO jön vele együtt)
	glBindVertexArray(m_vaoID);

	// kirajzolás
	glDrawElements(	GL_TRIANGLES,		// primitív típus
					6*6*5,					// hany csucspontot hasznalunk a kirajzolashoz
					GL_UNSIGNED_SHORT,	// indexek tipusa
					0);					// indexek cime

	// VAO kikapcsolasa
	glBindVertexArray(0);

	// shader kikapcsolasa
	glUseProgram( 0 );
}

void CMyApp::KeyboardDown(SDL_KeyboardEvent& key)
{
}

void CMyApp::KeyboardUp(SDL_KeyboardEvent& key)
{
}

void CMyApp::MouseMove(SDL_MouseMotionEvent& mouse)
{

}

void CMyApp::MouseDown(SDL_MouseButtonEvent& mouse)
{
}

void CMyApp::MouseUp(SDL_MouseButtonEvent& mouse)
{
}

void CMyApp::MouseWheel(SDL_MouseWheelEvent& wheel)
{
}

// a két paraméterbe az új ablakméret szélessége (_w) és magassága (_h) található
void CMyApp::Resize(int _w, int _h)
{
	glViewport(0, 0, _w, _h);

	m_matProj = glm::perspective(  45.0f,		// 90 fokos nyilasszog
									_w/(float)_h,	// ablakmereteknek megfelelo nezeti arany
									0.01f,			// kozeli vagosik
									100.0f);		// tavoli vagosik
}