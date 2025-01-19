#ifndef WORLD_H
#define WORLD_H

#include <iostream>
#include <algorithm>

#include "Chunk.h"
#include "PerlinNoise.hpp"

class PerlinNoise {
private:
	double fade(double t) const { // Smooth function
		return t * t * t * (t * (t * 6 - 15) + 10);
	}
	double linear_interpolate(double a, double b, double t) const { // interpolate vectors in terrain to ensure smooth transition
		return a + t * (b - a);
	}

	double grad(int hash, double x, double y, double z) const {
		int h = hash & 15; // Convert hash to one of 4 gradient directions
		double u = h < 8 ? x : y;
		double v = h < 4 ? y : (h == 12 || h == 14 ? x : z);
		return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
	}
	std::vector<uint8_t> permutation{};
public:
	PerlinNoise(uint32_t seed) {
		permutation.resize(256);
		std::iota(permutation.begin(), permutation.end(), 0); // Fill with 0, 1, 2, ..., 255
		std::default_random_engine engine(seed);
		std::shuffle(permutation.begin(), permutation.end(), engine);
		permutation.insert(permutation.end(), permutation.begin(), permutation.end());
	}

	double noise(double x, double y, double z) const {
		//Three integers coordinates
		int 
			X = static_cast<int>(std::floor(x)) & 255,
			Y = static_cast<int>(std::floor(y)) & 255,
			Z = static_cast<int>(std::floor(z)) & 255;

		double
			relative_X = x - std::floor(x),
			relative_Y = z - std::floor(y),
			relative_Z = z - std::floor(z),

			u = fade(relative_X),
			v = fade(relative_Y),
			w = fade(relative_Z);

		int
			aaa = permutation[permutation[permutation[X] + Y] + Z],
			aba = permutation[permutation[permutation[X] + Y + 1] + Z],
			aab = permutation[permutation[permutation[X] + Y] + Z + 1],
			abb = permutation[permutation[permutation[X] + Y + 1] + Z + 1],
			baa = permutation[permutation[permutation[X + 1] + Y] + Z],
			bba = permutation[permutation[permutation[X + 1] + Y + 1] + Z],
			bab = permutation[permutation[permutation[X + 1] + Y] + Z + 1],
			bbb = permutation[permutation[permutation[X + 1] + Y + 1] + Z + 1];

		double x1, x2, y1, y2;
		x1 = linear_interpolate(grad(aaa, relative_X, relative_Y, relative_Z), grad(baa, relative_X - 1, relative_Y, relative_Z), u);
		x2 = linear_interpolate(grad(aba, relative_X, relative_Y - 1, relative_Z), grad(bba, relative_X - 1, relative_Y - 1, relative_Z), u);
		y1 = linear_interpolate(x1, x2, v);

		x1 = linear_interpolate(grad(aab, relative_X, relative_Y, relative_Z - 1), grad(bab, relative_X - 1, relative_Y, relative_Z - 1), u);
		x2 = linear_interpolate(grad(abb, relative_X, relative_Y - 1, relative_Z - 1), grad(bbb, relative_X - 1, relative_Y - 1, relative_Z - 1), u);
		y2 = linear_interpolate(x1, x2, v);

		return (linear_interpolate(y1, y2, w) + 1.0) / 2.0; // Normalize to [0, 1]
	}

	double multi3D_Noise(double x, double y, double z, int octaves, double persistence, double lacunarity, double initialFrequency = 1.0, double initialAmplitude = 1.0) const;
};

struct World {
	using Chunks = std::vector<Chunk>;
private:
	void generateTerrain(const siv::PerlinNoise& perlin, Chunk::Blocks& chunkTerrain, const float_VEC& chunkOffset) const;
public:
	Chunks _chunks;
	void generateChunk(size_t numChunks);
};


#endif // WORLD_H