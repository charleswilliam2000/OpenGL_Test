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

uint32_t Chunk::getVisibleFaces(const uint8_VEC& block_coordinate, const uint8_VEC& chunkMinBounds, const uint8_VEC& chunkMaxBounds, const std::function<bool(int8_t x, int8_t y, int8_t z)>& getNeighborBlock) const
{
	
	uint8_t
		x = block_coordinate.x,
		y = block_coordinate.y,
		z = block_coordinate.z;

	uint32_t numVisibleFaces = 0;

	if (x == chunkMinBounds.x) { // West
		if (!getNeighborBlock(-1, y ,z)) {
			numVisibleFaces *= 10;
			numVisibleFaces += 1;
		}
	}
	else if (!getBlock(x - 1, y, z)) { 
		numVisibleFaces *= 10;
		numVisibleFaces += 1;
	}

	if (y == chunkMinBounds.y) {
		if (!getNeighborBlock(x, -1, z)) {
			numVisibleFaces *= 10;
			numVisibleFaces += 2;
		}
	}
	else if (!getBlock(x, y - 1, z)) { // Bottom
		numVisibleFaces *= 10;
		numVisibleFaces += 2;
	}

	if (z == chunkMinBounds.z) {
		if (!getNeighborBlock(x, y, -1)) {
			numVisibleFaces *= 10;
			numVisibleFaces += 3;
		}
	}
	else if (!getBlock(x, y, z - 1)) { // North
		numVisibleFaces *= 10;
		numVisibleFaces += 3;
	}

	if (x == chunkMaxBounds.x) {
		if (!getNeighborBlock(16, y, z)) {
			numVisibleFaces *= 10;
			numVisibleFaces += 4;
		}
	}
	else if (!getBlock(x + 1, y, z)) {  // East
		numVisibleFaces *= 10;
		numVisibleFaces += 4;
	}

	if (y == chunkMaxBounds.y) {
		if (!getNeighborBlock(x, 16, z)) {
			numVisibleFaces *= 10;
			numVisibleFaces += 5;
		}
	}
	else if (!getBlock(x, y + 1, z)) { // Top
		numVisibleFaces *= 10;
		numVisibleFaces += 5;
	}

	if (z == chunkMaxBounds.y) {
		if (!getNeighborBlock(x, y, 16)) {
			numVisibleFaces *= 10;
			numVisibleFaces += 6;
		}
	}
	else if (!getBlock(x, y, z + 1)) { // South
		numVisibleFaces *= 10;
		numVisibleFaces += 6;
	}

	return numVisibleFaces;
	
}

void Chunk::generate(float_VEC in_pos) {
	pos = in_pos;

	uint8_VEC chunkMin = { 0, 0, 0 };
	uint8_VEC chunkMax = { 15, 15, 15 };

	Chunk_Data data;
	uint32_t vertexOffset = 0;

	auto getNeighborBlock = [&](int8_t x, int8_t y, int8_t z) -> bool {

		if (x < chunkMin.x) return neightborChunks.west && neightborChunks.west->getBlock(15, y, z);
		if (x > chunkMax.x) return neightborChunks.east && neightborChunks.east->getBlock(0, y, z);
		
		if (y < chunkMin.y) return neightborChunks.bottom && neightborChunks.bottom->getBlock(x, 15, z);
		if (y > chunkMax.y) return neightborChunks.top && neightborChunks.top->getBlock(x, 0, z);
		
		if (z < chunkMin.z) return neightborChunks.north && neightborChunks.north->getBlock(x, y, 15);
		if (z > chunkMax.z) return neightborChunks.south && neightborChunks.south->getBlock(x, y, 0);
		return false;
		};

	for (uint8_t y = 0; y < Chunk_Constants::Dimension_1DSize; y++) {
		for (uint8_t z = 0; z < Chunk_Constants::Dimension_1DSize; z++) {
			for (uint8_t x = 0; x < Chunk_Constants::Dimension_1DSize; x++) {

				if (!getBlock(x, y, z)) continue;
				else {
					uint8_VEC blockCoordinate = { x, y, z }; // To query blocks array 
					uint32_t visibleFaces = getVisibleFaces(blockCoordinate, chunkMin, chunkMax, getNeighborBlock);

					if (visibleFaces == 0) continue;
					else {
						while (visibleFaces != 0) {
							uint32_t currFace = (visibleFaces % 10) - 1; // Minus one because of added offset 
							addFace(data, blockCoordinate, FACE_DATA[currFace], vertexOffset);
							visibleFaces /= 10;
						}
					}
				}


			}
		}
	}
	chunkData = BufferObjects(data.chunk_vertices, Attributes_Details::objectAttributes, data.chunk_indices);
}