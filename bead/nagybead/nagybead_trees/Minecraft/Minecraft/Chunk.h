#pragma once
#include <vector>
#include "Block.h"
class Chunk
{
private:
	int x;
	int z;
	int id;
	static int idCounter;
	//x, y, z indexelés
	std::vector<std::vector<std::vector<Block>>> data;
	bool firstUpdate;
public:
	static const int CHUNKSIZE = 16;
	static const int CHUNKHEIGHT = 256;

	Chunk(int x, int z);

	int getX() { return x; }
	int getZ() { return z; }
	int getId() { return id; }
	bool update();
	Block& getBlock(int x, int y, int z);
};

