#pragma once
#include <glm/glm.hpp>
#include "includes/ProgramObject.h"
#include "includes/BufferObject.h"
#include "includes/VertexArrayObject.h"
#include "Chunk.h"
class ChunkVAO
{
public:
	ChunkVAO() {}
	void bind();
	void unBind();
	void create(Chunk chunk, Chunk* xPositive, Chunk* xNegative, Chunk* zPositive, Chunk* zNegative);
	int getIndicesCount();
protected:
	struct Vertex {
		glm::vec3 point;
		glm::vec3 normal;
		glm::vec2 textureUV;
		glm::vec2 textureAtlasOffset;
	};

	enum BlockFace {FRONT, BACK, RIGHT, LEFT, TOP, BOTTOM};
private:
	VertexArrayObject	chunkVao;		
	IndexBuffer			chunkIndices;	
	ArrayBuffer			chunkVertexBuffer;

	std::vector<Vertex> vertices;
	std::vector<int> indices;

	void addSide(Block block, BlockFace face);
	glm::vec2 getTextureAtlasOffset(BlockType type, BlockFace face);
};

