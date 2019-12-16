#include "Mesh_OGL3.h"

Mesh::Mesh(void)
{
}

Mesh::~Mesh(void)
{
}

void Mesh::initBuffers()
{
	size = indices.size();

	gpuBufferPos.BufferData(array_pos);
	gpuBufferNorm.BufferData(array_norm);
	gpuBufferTex.BufferData(array_tex);

	gpuBufferIndices.BufferData(indices);

	// geometria VAO-ban való regisztrálása
	vao.Init(
		{
			{ CreateAttribute<0,glm::vec3>, gpuBufferPos },
			{ CreateAttribute<1,glm::vec3>, gpuBufferNorm },
			{ CreateAttribute<2,glm::vec2>, gpuBufferTex }
		},
		gpuBufferIndices
	);
}

void Mesh::draw()
{
	vao.Bind();
	glDrawElements(GL_TRIANGLES, size, GL_UNSIGNED_INT, 0);
	vao.Unbind();

}