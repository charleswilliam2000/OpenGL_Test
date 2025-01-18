#include "World.h"
void World::generateTerrain(Chunk::Blocks& chunkTerrain)
{
	for (uint8_t y = 0; y < Chunk_Constants::Dimension_1DSize; y++) {
		for (uint8_t z = 0; z < Chunk_Constants::Dimension_1DSize; z++) {
			for (uint8_t x = 0; x < Chunk_Constants::Dimension_1DSize; x++) {

				chunkTerrain[y][z].setX(x);
			}
		}
	}
}

void World::generateChunk(size_t numChunks)
{
	float chunk_X = .0f, chunk_Y = .0f, chunk_Z = .0f;

	_chunks.resize(numChunks);
	for (size_t i = 0; i < numChunks; i++) {
		generateTerrain(_chunks[i].blocks);
	}

	for (size_t i = 0; i < numChunks; i++) {
		switch (i) {
		case 0:
			_chunks[i].neightborChunks.north = &_chunks[i + 1];
			_chunks[i].generate({chunk_X, chunk_Y, chunk_Z});
			break;
		case 1:
			_chunks[i].neightborChunks.south = &_chunks[i - 1];
			_chunks[i].generate({ chunk_X, chunk_Y, chunk_Z - 16.0f });
			break;
		case 2:
			_chunks[i].generate({ chunk_X + 16.0f, chunk_Y, chunk_Z });
			break;
		case 3:
			_chunks[i].generate({chunk_X, chunk_Y, chunk_Z + 16.0f});
			break;
		case 4:
			_chunks[i].generate({chunk_X - 16.0f, chunk_Y, chunk_Z});
			break;
		case 5:
			_chunks[i].generate({ chunk_X - 16.0f, chunk_Y, chunk_Z - 16.0f });
			break;
		case 6:
			_chunks[i].generate({ chunk_X + 16.0f, chunk_Y, chunk_Z - 16.0f });
			break;
		case 7:
			_chunks[i].generate({ chunk_X + 16.0f, chunk_Y, chunk_Z + 16.0f });
			break;
		case 8:
			_chunks[i].generate({ chunk_X - 16.0f, chunk_Y, chunk_Z + 16.0f });
			break;
		}
	}
}
