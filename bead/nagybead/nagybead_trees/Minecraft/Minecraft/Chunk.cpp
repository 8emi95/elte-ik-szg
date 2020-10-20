#include "Chunk.h"
#include <stdexcept>
int Chunk::idCounter = 0;


Chunk::Chunk(int x, int z) : x(x), z(z) {
	id = idCounter;
	idCounter++;
	data = std::vector<std::vector<std::vector<Block>>>(CHUNKSIZE);
	for (int i = 0; i < CHUNKSIZE; i++)
	{
		data[i] = std::vector<std::vector<Block>>(CHUNKHEIGHT);
		for (int j = 0; j < CHUNKHEIGHT; j++)
		{
			for (int k = 0; k < CHUNKSIZE; k++)
			{
				data[i][j].push_back(Block(CHUNKSIZE * x + i, j, CHUNKSIZE * z + k, AIR));
			}
		}
	}
	firstUpdate = true;
}

bool Chunk::update() {
	if (firstUpdate == true) {
		firstUpdate = false;
		return true;
	}
	return false;
}

Block& Chunk::getBlock(int x, int y, int z)
{
	if (x < 0 || x > CHUNKSIZE - 1 || z < 0 || z > CHUNKSIZE - 1 || y < 0 || y > CHUNKHEIGHT - 1) {
		throw std::logic_error("Out of bound");
	}
	return data[x][y][z];
}
