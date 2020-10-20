#pragma once

#include "Includes/PerlinNoise.hpp"
class NoiseGenerator
{
public:
	NoiseGenerator(int seed, int min, int max, int size);
	int getNoise(int x, int z);
private:
	static const double FREQUENCY;
	static const int OCTAVES = 8;
	int seed;
	int min;
	int max;
	int size;
	double fs;
	siv::PerlinNoise perlin;
};

