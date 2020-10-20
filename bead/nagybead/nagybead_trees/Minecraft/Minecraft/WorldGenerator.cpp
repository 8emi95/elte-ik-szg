#include "WorldGenerator.h"
#include <time.h>
#include "NoiseGenerator.h"

float random_between_two_float(float min, float max)
{
	return (min + 1) + (((float)rand()) / (float)RAND_MAX) * (max - (min + 1));
}

std::vector<std::vector<Chunk>> WorldGenerator::generateWorld(int dimension)
{
	srand(time(0));
	NoiseGenerator noiseGeneratorStone(rand(), 40, 50, dimension * Chunk::CHUNKSIZE);
	NoiseGenerator noiseGeneratorDirt(rand(), 45, 55, dimension * Chunk::CHUNKSIZE);
	std::vector<std::vector<Chunk>> world = std::vector<std::vector<Chunk>>(dimension);
	for (int i = 0; i < dimension; i++)
	{
		for (int j = 0; j < dimension; j++)
		{
			world[i].push_back(Chunk(i, j));
			for (int x = 0; x < Chunk::CHUNKSIZE; x++)
			{
				for (int z = 0; z < Chunk::CHUNKSIZE; z++)
				{
					float random = random_between_two_float(0, 100);
					int heightStone = noiseGeneratorStone.getNoise(
						world[i][j].getX() * Chunk::CHUNKSIZE + x, 
						world[i][j].getZ() * Chunk::CHUNKSIZE + z
					);
					int heightDirt = noiseGeneratorDirt.getNoise(
						world[i][j].getX() * Chunk::CHUNKSIZE + x,
						world[i][j].getZ() * Chunk::CHUNKSIZE + z
					);
					for (int y = 0; y < std::max(heightStone, heightDirt); y++)
					{
						//Block currentBlock = world[i][j].getBlock(x, y, z);
						if (y == 0) {
							world[i][j].getBlock(x, y, z).setBlockType(BEDROCK);
						}
						else if (y < heightStone) {
							world[i][j].getBlock(x, y, z).setBlockType(STONE);
						}
						else if (y < heightDirt - 1) {
							world[i][j].getBlock(x, y, z).setBlockType(DIRT);
						}
						else {
							world[i][j].getBlock(x, y, z).setBlockType(GRASS);
						}
					}
					if (random < 2 &&
						x > 0 &&
						z > 0 &&
						x < Chunk::CHUNKSIZE - 1 &&
						z < Chunk::CHUNKSIZE - 1 &&
						world[i][j].getBlock(x, heightDirt - 1, z).getType() == GRASS) { // GRASS LEGYEN ALATTA

						// 3-high tree trunk
						world[i][j].getBlock(x, heightDirt, z).setBlockType(TRUNK);
						world[i][j].getBlock(x, heightDirt + 1, z).setBlockType(TRUNK);
						world[i][j].getBlock(x, heightDirt + 2, z).setBlockType(TRUNK);

						// 2-high tree canopy
						world[i][j].getBlock(x - 1, heightDirt + 3, z - 1).setBlockType(CANOPY);
						world[i][j].getBlock(x + 0, heightDirt + 3, z - 1).setBlockType(CANOPY);
						world[i][j].getBlock(x + 1, heightDirt + 3, z - 1).setBlockType(CANOPY);

						world[i][j].getBlock(x - 1, heightDirt + 3, z + 0).setBlockType(CANOPY);
						world[i][j].getBlock(x + 0, heightDirt + 3, z + 0).setBlockType(CANOPY);
						world[i][j].getBlock(x + 1, heightDirt + 3, z + 0).setBlockType(CANOPY);

						world[i][j].getBlock(x - 1, heightDirt + 3, z + 1).setBlockType(CANOPY);
						world[i][j].getBlock(x + 0, heightDirt + 3, z + 1).setBlockType(CANOPY);
						world[i][j].getBlock(x + 1, heightDirt + 3, z + 1).setBlockType(CANOPY);
						
						world[i][j].getBlock(x + 0, heightDirt + 4, z + 0).setBlockType(CANOPY);
					}
				}
			}
		}
	}
	return world;
}
