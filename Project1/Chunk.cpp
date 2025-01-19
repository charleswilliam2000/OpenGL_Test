#include "Chunk.h"

bool Chunk::checkValidBlock(const uint8_VEC& block_coordinate) {
	if (block_coordinate.x < 0 || block_coordinate.y < 0 || block_coordinate.z < 0) {
		std::cerr << "\nOne coordinate is smaller than 0! Coordinates: "
			<< block_coordinate.x << " - " << block_coordinate.y << " - " << block_coordinate.z;
		throw std::runtime_error("\nOne coordinate is smaller than 0!");
	}

	if (block_coordinate.x >= 16 || block_coordinate.y >= 16 || block_coordinate.z >= 16) {
		std::cerr << "\nOne coordinate is larger than 32! Coordinates: "
			<< block_coordinate.x << " - " << block_coordinate.y << " - " << block_coordinate.z;
		throw std::runtime_error("\nOne coordinate is larger than 32!");
	}
	return true;
}

uint32_t Chunk::getVisibleFaces(
	const uint8_VEC& block_coordinate, 
	const uint8_VEC& chunkMinBounds, 
	const uint8_VEC& chunkMaxBounds, 
	const std::function<bool(const FACES& face, uint8_VEC pos)>& getNeighborChunkBlock) const
{
	
	uint8_t
		x = block_coordinate.x,
		y = block_coordinate.y,
		z = block_coordinate.z;

	uint32_t numVisibleFaces = 0;

	if (x == chunkMinBounds.x) { // West
		if (!getNeighborChunkBlock(FACES::WEST, block_coordinate)) {
			numVisibleFaces *= 10;
			numVisibleFaces += 1;
		}
	}
	else if (!getBlock(x - 1, y, z)) { 
		numVisibleFaces *= 10;
		numVisibleFaces += 1;
	}

	if (y == chunkMinBounds.y) { // Bottom
		if (!getNeighborChunkBlock(FACES::BOTTOM, block_coordinate)) {
			numVisibleFaces *= 10;
			numVisibleFaces += 2;
		}
	}
	else if (!getBlock(x, y - 1, z)) { 
		numVisibleFaces *= 10;
		numVisibleFaces += 2;
	}

	if (z == chunkMinBounds.z) { // North
		if (!getNeighborChunkBlock(FACES::NORTH, block_coordinate)) {
			numVisibleFaces *= 10;
			numVisibleFaces += 3;
		}
	}
	else if (!getBlock(x, y, z - 1)) { 
		numVisibleFaces *= 10;
		numVisibleFaces += 3;
	}

	if (x == chunkMaxBounds.x) { // East
		if (!getNeighborChunkBlock(FACES::EAST, block_coordinate)) {
			numVisibleFaces *= 10;
			numVisibleFaces += 4;
		}
	}
	else if (!getBlock(x + 1, y, z)) {  
		numVisibleFaces *= 10;
		numVisibleFaces += 4;
	}

	if (y == chunkMaxBounds.y) { // Top
		if (!getNeighborChunkBlock(FACES::TOP, block_coordinate)) {
			numVisibleFaces *= 10;
			numVisibleFaces += 5;
		}
	}
	else if (!getBlock(x, y + 1, z)) { 
		numVisibleFaces *= 10;
		numVisibleFaces += 5;
	}

	if (z == chunkMaxBounds.y) { // South
		if (!getNeighborChunkBlock(FACES::SOUTH, block_coordinate)) {
			numVisibleFaces *= 10;
			numVisibleFaces += 6;
		}
	}
	else if (!getBlock(x, y, z + 1)) {
		numVisibleFaces *= 10;
		numVisibleFaces += 6;
	}

	return numVisibleFaces;
}

Chunk_Data Chunk::generate(float_VEC in_pos) {
	pos = in_pos;

	uint8_VEC chunkMin = { 0, 0, 0 };
	uint8_VEC chunkMax = { 15, 15, 15 };

	Chunk_Data data;
	uint32_t vertexOffset = 0;

	auto getNeighborChunkBlock = [&](const FACES& face, uint8_VEC pos) -> bool {
		switch (face) {
		case FACES::WEST: 
			return neightborChunks.neighbors[static_cast<int>(face)] && neightborChunks.neighbors[static_cast<int>(face)]->getBlock(15, pos.y, pos.z);
		case FACES::BOTTOM:
			return neightborChunks.neighbors[static_cast<int>(face)] && neightborChunks.neighbors[static_cast<int>(face)]->getBlock(pos.x, 15, pos.z);
		case FACES::NORTH:
			return neightborChunks.neighbors[static_cast<int>(face)] && neightborChunks.neighbors[static_cast<int>(face)]->getBlock(pos.x, pos.y, 15);
		case FACES::EAST:
			return neightborChunks.neighbors[static_cast<int>(face)] && neightborChunks.neighbors[static_cast<int>(face)]->getBlock(0, pos.y, pos.z);
		case FACES::TOP:
			return neightborChunks.neighbors[static_cast<int>(face)] && neightborChunks.neighbors[static_cast<int>(face)]->getBlock(pos.x, 0, pos.z);
		case FACES::SOUTH:
			return neightborChunks.neighbors[static_cast<int>(face)] && neightborChunks.neighbors[static_cast<int>(face)]->getBlock(pos.x, pos.y, 0);
		default:
			throw std::runtime_error("\nUnable to deduce face!");
		}
		};

	for (uint8_t y = 0; y < Chunk_Constants::Dimension_1DSize; y++) {
		for (uint8_t z = 0; z < Chunk_Constants::Dimension_1DSize; z++) {
			for (uint8_t x = 0; x < Chunk_Constants::Dimension_1DSize; x++) {

				if (!getBlock(x, y, z)) 
					continue;

				uint8_VEC blockCoordinate = { x, y, z }; 
				uint32_t visibleFaces = getVisibleFaces(blockCoordinate, chunkMin, chunkMax, getNeighborChunkBlock);

				if (visibleFaces == 0) 
					continue;

				while (visibleFaces != 0) {
					uint32_t currFace = (visibleFaces % 10) - 1; // Minus one because of added offset 
					addFace(data, blockCoordinate, FACE_DATA[currFace], vertexOffset);
					visibleFaces /= 10;
				}


			}
		}
	}
	return data;
}