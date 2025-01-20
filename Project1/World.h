#ifndef WORLD_H
#define WORLD_H

#include <iostream>
#include <thread>
#include <future>

#include "Chunk.h"
#include "PerlinNoise.hpp"

struct World {
	using Chunks = std::vector<std::pair<WorldChunk, ChunkMesh>>;
private:
	void generateTerrain(const siv::PerlinNoise& perlin, WorldChunk::Blocks& chunkTerrain, const float_VEC& chunkOffset) const;
public:
	Chunks _chunks;
	void generateChunk(size_t numChunks);
};


#endif // WORLD_H