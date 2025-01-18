#ifndef WORLD_H
#define WORLD_H

#include <iostream>
#include <unordered_set>

#include "Chunk.h"

struct World {
	using Chunks = std::vector<Chunk>;
private:
	void generateTerrain(Chunk::Blocks& chunkTerrain);
public:
	Chunks _chunks;

	void generateChunk(size_t numChunks);
};


#endif // WORLD_H