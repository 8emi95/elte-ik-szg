#pragma once
#include "BlockType.h"
class Block
{
private:
	int x;
	int y;
	int z;
	BlockType type;
public:
	Block(int x, int y, int z, BlockType type) : x(x), y(y), z(z), type(type) {}
	int getX() { return x; }
	int getY() { return y; }
	int getZ() { return z; }
	BlockType getType() { return type; }
	void setBlockType(BlockType _type) { type = _type; }
};

