#include "ChunkVAO.h"

void ChunkVAO::bind()
{
	chunkVao.Bind();
}

void ChunkVAO::unBind()
{
	chunkVao.Unbind();
}

void ChunkVAO::create(Chunk chunk, Chunk* xPositive, Chunk* xNegative, Chunk* zPositive, Chunk* zNegative)
{
	for (int i = 0; i < Chunk::CHUNKSIZE; i++)
	{
		for (int j = 0; j < Chunk::CHUNKHEIGHT; j++)
		{
			for (int k = 0; k < Chunk::CHUNKSIZE; k++)
			{
				Block block = chunk.getBlock(i, j, k);
				if (block.getType() != AIR) {
					if (k < Chunk::CHUNKSIZE - 1 && chunk.getBlock(i, j, k + 1).getType() == AIR) {
						addSide(block, FRONT);
					}
					else if (k == Chunk::CHUNKSIZE - 1 && (zPositive == nullptr || zPositive->getBlock(i, j, 0).getType() == AIR)) {
						addSide(block, FRONT);
					}

					if (k > 0 && chunk.getBlock(i, j, k - 1).getType() == AIR) {
						addSide(block, BACK);
					}
					else if (k == 0 && (zNegative == nullptr || zNegative->getBlock(i, j, Chunk::CHUNKSIZE - 1).getType() == AIR)) {
						addSide(block, BACK);
					}

					if (i < Chunk::CHUNKSIZE - 1 && chunk.getBlock(i + 1, j, k).getType() == AIR) {
						addSide(block, RIGHT);
					}
					else if (i == Chunk::CHUNKSIZE - 1 && (xPositive == nullptr || xPositive->getBlock(0, j, k).getType() == AIR)) {
						addSide(block, RIGHT);
					}

					if (i > 0 && chunk.getBlock(i - 1, j, k).getType() == AIR) {
						addSide(block, LEFT);
					}
					else if (i == 0 && (xNegative == nullptr || xNegative->getBlock(Chunk::CHUNKSIZE - 1, j, k).getType() == AIR)) {
						addSide(block, LEFT);
					}

					if (j < Chunk::CHUNKHEIGHT - 1 && chunk.getBlock(i, j + 1, k).getType() == AIR) {
						addSide(block, TOP);
					}
					else if (j == Chunk::CHUNKHEIGHT - 1) {
						addSide(block, TOP);
					}

					if (j > 0 && chunk.getBlock(i, j - 1, k).getType() == AIR) {
						addSide(block, BOTTOM);
					}
					else if (j == 0) {
						addSide(block, BOTTOM);
					}
				}
			}
		}
	}

	chunkVertexBuffer.BufferData(vertices);
	chunkIndices.BufferData(indices);
	chunkVao.Init(
		{
			{ CreateAttribute<0, glm::vec3,	0,	sizeof(Vertex) >, chunkVertexBuffer },
			{ CreateAttribute<1, glm::vec3, (sizeof(glm::vec3)), sizeof(Vertex)>, chunkVertexBuffer },
			{ CreateAttribute<2, glm::vec2, (2 * sizeof(glm::vec3)), sizeof(Vertex)>, chunkVertexBuffer },
			{ CreateAttribute<3, glm::vec2, (2 * sizeof(glm::vec3) + sizeof(glm::vec2)), sizeof(Vertex)>, chunkVertexBuffer },
		},
		chunkIndices
		);
}

int ChunkVAO::getIndicesCount()
{
	return indices.size();
}

glm::vec2 ChunkVAO::getTextureAtlasOffset(BlockType type, BlockFace face) {
	if (type == STONE) {
		return glm::vec2(1, 15);
	}
	if (type == BEDROCK) {
		return glm::vec2(1, 14);
	}
	if (type == DIRT) {
		return glm::vec2(2, 15);
	}
	if (type == GRASS) {
		if (face == TOP) {
			return glm::vec2(0, 15);
		}
		if (face == BOTTOM) {
			return glm::vec2(2, 15);
		}
		return glm::vec2(3, 15);
	}
	if (type == TRUNK) {
		return glm::vec2(4, 14);
	}
	if (type == CANOPY) {
		return glm::vec2(5, 12);
	}
	return glm::vec2(0, 0);
}

void ChunkVAO::addSide(Block block, BlockFace face)
{
	glm::vec2 atlasOffset = getTextureAtlasOffset(block.getType(), face);
	int index = vertices.size();
	switch (face) {
	case FRONT: {
		vertices.push_back({ glm::vec3(block.getX() - 0.5, block.getY() - 0.5, block.getZ() + 0.5), glm::vec3(0, 0, 1), glm::vec2(0, 0), atlasOffset });
		vertices.push_back({ glm::vec3(block.getX() + 0.5, block.getY() - 0.5, block.getZ() + 0.5), glm::vec3(0, 0, 1), glm::vec2(1, 0), atlasOffset });
		vertices.push_back({ glm::vec3(block.getX() - 0.5, block.getY() + 0.5, block.getZ() + 0.5), glm::vec3(0, 0, 1), glm::vec2(0, 1), atlasOffset });
		vertices.push_back({ glm::vec3(block.getX() + 0.5, block.getY() + 0.5, block.getZ() + 0.5), glm::vec3(0, 0, 1), glm::vec2(1, 1), atlasOffset });
		break;
	}
	case BACK: {
		vertices.push_back({ glm::vec3(block.getX() + 0.5, block.getY() - 0.5, block.getZ() - 0.5), glm::vec3(0, 0, -1), glm::vec2(0, 0), atlasOffset });
		vertices.push_back({ glm::vec3(block.getX() - 0.5, block.getY() - 0.5, block.getZ() - 0.5), glm::vec3(0, 0, -1), glm::vec2(1, 0), atlasOffset });
		vertices.push_back({ glm::vec3(block.getX() + 0.5, block.getY() + 0.5, block.getZ() - 0.5), glm::vec3(0, 0, -1), glm::vec2(0, 1), atlasOffset });
		vertices.push_back({ glm::vec3(block.getX() - 0.5, block.getY() + 0.5, block.getZ() - 0.5), glm::vec3(0, 0, -1), glm::vec2(1, 1), atlasOffset });
		break;
	}
	case RIGHT: {
		vertices.push_back({ glm::vec3(block.getX() + 0.5, block.getY() - 0.5, block.getZ() + 0.5), glm::vec3(1, 0, 0), glm::vec2(0, 0), atlasOffset });
		vertices.push_back({ glm::vec3(block.getX() + 0.5, block.getY() - 0.5, block.getZ() - 0.5), glm::vec3(1, 0, 0), glm::vec2(1, 0), atlasOffset });
		vertices.push_back({ glm::vec3(block.getX() + 0.5, block.getY() + 0.5, block.getZ() + 0.5), glm::vec3(1, 0, 0), glm::vec2(0, 1), atlasOffset });
		vertices.push_back({ glm::vec3(block.getX() + 0.5, block.getY() + 0.5, block.getZ() - 0.5), glm::vec3(1, 0, 0), glm::vec2(1, 1), atlasOffset });
		break;
	}
	case LEFT: {
		vertices.push_back({ glm::vec3(block.getX() - 0.5, block.getY() - 0.5, block.getZ() - 0.5), glm::vec3(-1, 0, 0), glm::vec2(0, 0), atlasOffset });
		vertices.push_back({ glm::vec3(block.getX() - 0.5, block.getY() - 0.5, block.getZ() + 0.5), glm::vec3(-1, 0, 0), glm::vec2(1, 0), atlasOffset });
		vertices.push_back({ glm::vec3(block.getX() - 0.5, block.getY() + 0.5, block.getZ() - 0.5), glm::vec3(-1, 0, 0), glm::vec2(0, 1), atlasOffset });
		vertices.push_back({ glm::vec3(block.getX() - 0.5, block.getY() + 0.5, block.getZ() + 0.5), glm::vec3(-1, 0, 0), glm::vec2(1, 1), atlasOffset });
		break;
	}
	case TOP: {
		vertices.push_back({ glm::vec3(block.getX() - 0.5, block.getY() + 0.5, block.getZ() + 0.5), glm::vec3(0, 1, 0), glm::vec2(0, 0), atlasOffset });
		vertices.push_back({ glm::vec3(block.getX() + 0.5, block.getY() + 0.5, block.getZ() + 0.5), glm::vec3(0, 1, 0), glm::vec2(1, 0), atlasOffset });
		vertices.push_back({ glm::vec3(block.getX() - 0.5, block.getY() + 0.5, block.getZ() - 0.5), glm::vec3(0, 1, 0), glm::vec2(0, 1), atlasOffset });
		vertices.push_back({ glm::vec3(block.getX() + 0.5, block.getY() + 0.5, block.getZ() - 0.5), glm::vec3(0, 1, 0), glm::vec2(1, 1), atlasOffset });
		break;
	}
	case BOTTOM: {
		vertices.push_back({ glm::vec3(block.getX() - 0.5, block.getY() - 0.5, block.getZ() - 0.5), glm::vec3(0, -1, 0), glm::vec2(0, 0), atlasOffset });
		vertices.push_back({ glm::vec3(block.getX() + 0.5, block.getY() - 0.5, block.getZ() - 0.5), glm::vec3(0, -1, 0), glm::vec2(1, 0), atlasOffset });
		vertices.push_back({ glm::vec3(block.getX() - 0.5, block.getY() - 0.5, block.getZ() + 0.5), glm::vec3(0, -1, 0), glm::vec2(0, 1), atlasOffset });
		vertices.push_back({ glm::vec3(block.getX() + 0.5, block.getY() - 0.5, block.getZ() + 0.5), glm::vec3(0, -1, 0), glm::vec2(1, 1), atlasOffset });
		break;
	}
	}

	indices.push_back(index + 0);
	indices.push_back(index + 1);
	indices.push_back(index + 2);
	indices.push_back(index + 1);
	indices.push_back(index + 3);
	indices.push_back(index + 2);
}


