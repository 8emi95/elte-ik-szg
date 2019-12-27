#include "MyApp.h"
#include "GLUtils.hpp"
#include <time.h>
#include <math.h>

std::vector<glm::vec3> CMyApp::VerticesOfCube(int lengthOfSide) {

	std::vector<glm::vec3> vertices;
	for (int i = 0; i < 2; ++i) {
		for (int j = 0; j < 2; ++j) {
			for (int k = 0; k < 2; ++k) {
				double x = (pow(-1, i)) * ((float)lengthOfSide / 2);
				double y = (pow(-1, j)) * ((float)lengthOfSide / 2);
				double z = (pow(-1, k)) * ((float)lengthOfSide / 2);
				vertices.push_back(glm::vec3(x, y, z));
			}
		}
	}
	return vertices;
}

CMyApp::CMyApp(void)
{
	m_vaoID = 0;
	m_vboID = 0;
	m_ibID = 0;
	m_programID = 0;

	cam_z = 27;
	unitSide = 1;
	cubeSide = 2;
	positionsOfElementsL = { glm::vec3(0, 0, 0), glm::vec3(0, unitSide, 0), glm::vec3(0, 2 * unitSide, 0), 
							 glm::vec3(0, 3 * unitSide, 0), glm::vec3(unitSide, 0, 0) };

	m_rotateAboutOwnX = glm::mat4();
	shouldRotate = false;
}

CMyApp::~CMyApp(void)
{
}

bool CMyApp::Init()
{
	glClearColor(0.369f, 0.102f, 0.102f, 1.0f);

	glEnable(GL_CULL_FACE); 
	glEnable(GL_DEPTH_TEST); 
	glCullFace(GL_BACK); 

	positions = VerticesOfCube(cubeSide); //a geometria alapjat szolgaltato 2 oldalhosszu, origo kozeppontu kocka csucsainak meghatarozasa

	std::vector<glm::vec3> positionsOfUnitCube = VerticesOfCube(unitSide); //az L betu alapjat jelento egysegkocka letrehozasa 
																		   //(szinten az origot tekintve kozeppontnak)
	
	//kocka vertexeinek letrehozasa
	Vertex vert[] =
	{
		//        (x, y, z)                      (r, g, b)
		{ positionsOfUnitCube[0], glm::vec3(1.000f, 0.973f, 0.863f) },
		{ positionsOfUnitCube[1], glm::vec3(1.0f, 0.922f, 0.804f) },
		{ positionsOfUnitCube[2], glm::vec3(1.0f, 0.894f, 0.769f) },
		{ positionsOfUnitCube[3], glm::vec3(1.0f, 0.871f, 0.678f) },

		{ positionsOfUnitCube[4], glm::vec3(0.98f, 0.871f, 0.703f) },
		{ positionsOfUnitCube[5], glm::vec3(0.961f, 0.871f, 0.702f) },
		{ positionsOfUnitCube[6], glm::vec3(0.871f, 0.722f, 0.529f) },
		{ positionsOfUnitCube[7], glm::vec3(0.824f, 0.706f, 0.549f) },
	};

	GLushort indices[] =
	{
		//elolap:
		4, 6, 2, 4, 2, 0,
		//jobb oldallap:
		0, 2, 3, 0, 3, 1,
		//fedlap:
		5, 4, 0, 5, 0, 1,

		//hatlap:
		5, 3, 7, 5, 1, 3,
		//bal oldallap:
		4, 7, 6, 4, 5, 7,
		//alaplap:
		7, 2, 6, 7, 3, 2

	};

	glGenVertexArrays(1, &m_vaoID);
	glBindVertexArray(m_vaoID);

	glGenBuffers(1, &m_vboID);
	glBindBuffer(GL_ARRAY_BUFFER, m_vboID);
	
	glBufferData(GL_ARRAY_BUFFER, sizeof(vert),	vert, GL_STATIC_DRAW);	

	glEnableVertexAttribArray(0); 
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);

	glEnableVertexAttribArray(1); 
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(glm::vec3)));

	glGenBuffers(1, &m_ibID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindVertexArray(0); 
	glBindBuffer(GL_ARRAY_BUFFER, 0); 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); 

	GLuint vs_ID = loadShader(GL_VERTEX_SHADER, "myVert.vert");
	GLuint fs_ID = loadShader(GL_FRAGMENT_SHADER, "myFrag.frag");

	m_programID = glCreateProgram();

	glAttachShader(m_programID, vs_ID);
	glAttachShader(m_programID, fs_ID);

	glBindAttribLocation(m_programID, 0, "vs_in_pos");	
	glBindAttribLocation(m_programID, 1, "vs_in_col");

	glLinkProgram(m_programID);

	GLint infoLogLength = 0, result = 0;

	glGetProgramiv(m_programID, GL_LINK_STATUS, &result);
	glGetProgramiv(m_programID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (GL_FALSE == result)
	{
		std::vector<char> ProgramErrorMessage(infoLogLength);
		glGetProgramInfoLog(m_programID, infoLogLength, NULL, &ProgramErrorMessage[0]);
		fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);

		char* errorMessage = new char[ProgramErrorMessage.size()];
		memcpy(errorMessage, &ProgramErrorMessage[0], ProgramErrorMessage.size());

		std::cout << "[app.Init()] Shader error: " << errorMessage << std::endl;

		delete errorMessage;
	}

	glDeleteShader(vs_ID);
	glDeleteShader(fs_ID);

	m_matProj = glm::perspective(45.0f, 640 / 480.0f, 1.0f, 1000.0f);
	m_loc_mvp = glGetUniformLocation(m_programID, "MVP");

	return true;
}

void CMyApp::Clean()
{
	glDeleteBuffers(1, &m_vboID);
	glDeleteBuffers(1, &m_ibID);
	glDeleteVertexArrays(1, &m_vaoID);

	glDeleteProgram(m_programID);
}

void CMyApp::Update()
{
	m_matView = glm::lookAt(glm::vec3(0, 0, cam_z), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));	
}

void CMyApp::DrawL(glm::vec3 &position, glm::mat4 &rotateAboutOwnX, glm::mat4 &rotateInXY) {

	glm::mat4 mvp;

	for (size_t i = 0; i < positionsOfElementsL.size(); ++i) {
		m_matWorld = rotateInXY  * glm::translate<float>(position) * rotateAboutOwnX *  glm::translate<float>(positionsOfElementsL[i]);
		mvp = m_matProj * m_matView * m_matWorld;
		glUniformMatrix4fv(m_loc_mvp, 1, GL_FALSE, &(mvp[0][0]));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);
	}
}

void CMyApp::DrawGeometry(glm::vec3 &position) {
	DrawL(position, glm::mat4(), glm::mat4());
};

void CMyApp::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_programID);
	glBindVertexArray(m_vaoID);

	std::vector<glm::vec3>::iterator it = positions.begin();

	//1. RESZFELADAT: L forma kirajzolasa

	//DrawGeometry(glm::vec3());
	

	//2. RESZFELADAT: az L-ekbol kirakott formacio kirajzolasa

	//while (it != positions.end()) {
	//	DrawGeometry(*it);
	//	++it;
	//}


	//3. RESZFELADAT: forgatas egy XY síkban fekvo, origo kozeppontu, 10 sugaru korpalya menten

	//while (it != positions.end()) {
	//	DrawL(
	//		glm::vec3(10, 0, 0) + *it,
	//		glm::mat4(),
	//		glm::rotate<float>(SDL_GetTicks() / 11000.0f * 2 * M_PI, glm::vec3(0, 0, 1))
	//	);
	//	++it;
	//}
	

	//4. RESZFELADAT: a space lenyomasaval a formacio sajat X tengelye korul valo forgatasanak ki-bekapcsolasa

	if (shouldRotate) {
		m_rotateAboutOwnX = glm::rotate<float>(SDL_GetTicks() / 1000.0f * 2 * M_PI, glm::vec3(1, 0, 0));
	}
	while (it != positions.end()) {
		DrawL(
			glm::vec3(10, 0, 0) + *it,
			m_rotateAboutOwnX,
			glm::rotate<float>(SDL_GetTicks() / 11000.0f * 2 * M_PI, glm::vec3(0, 0, 1))
		);
		++it;
	}
	
	glBindVertexArray(0);
	glUseProgram(0);
}

void CMyApp::KeyboardDown(SDL_KeyboardEvent& key)
{
	switch (key.keysym.sym) {
	case SDLK_SPACE:
		shouldRotate = !shouldRotate;
		break;
	}
}

void CMyApp::Resize(int _w, int _h)
{
	glViewport(0, 0, _w, _h);
	m_matProj = glm::perspective(45.0f, _w / (float)_h, 0.01f, 100.0f);
}