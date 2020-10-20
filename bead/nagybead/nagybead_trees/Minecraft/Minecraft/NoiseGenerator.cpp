#include "NoiseGenerator.h"

const double NoiseGenerator::FREQUENCY = 4.0;

NoiseGenerator::NoiseGenerator(int seed, int min, int max, int size) : seed(seed), min(min), max(max), size(size)
{
	perlin = siv::PerlinNoise(seed);
	fs = size / FREQUENCY;
}

int interpolate(double min, double max, int outMin, int outMax, double value) {
	return outMin + (int)std::floor(((outMax - outMin) / (max - min)) * (value - min));
}

int NoiseGenerator::getNoise(int x, int z)
{
	return interpolate(0, 1, min, max, perlin.octaveNoise0_1(x / fs, z / fs, OCTAVES));
}
