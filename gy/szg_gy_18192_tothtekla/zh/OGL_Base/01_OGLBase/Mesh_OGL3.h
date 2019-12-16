#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "BufferObject.h"
#include "VertexArrayObject.h"

class Mesh
{
public:

	Mesh(void);
	~Mesh(void);

	void initBuffers();
	void draw();

	void addVertex(glm::vec3 pos, glm::vec3 norm, glm::vec2 tex) {
		array_pos.push_back(pos);
		array_norm.push_back(norm);
		array_tex.push_back(tex);
	}

	void addIndex(unsigned int index) {
		indices.push_back(index);
	}

private:

	ArrayBuffer gpuBufferPos;
	ArrayBuffer gpuBufferNorm;
	ArrayBuffer gpuBufferTex;
	IndexBuffer gpuBufferIndices;

	VertexArrayObject	vao;
	int					size;

	std::vector<glm::vec3> array_pos;
	std::vector<glm::vec3> array_norm;
	std::vector<glm::vec2> array_tex;
	std::vector<unsigned int> indices;
};
