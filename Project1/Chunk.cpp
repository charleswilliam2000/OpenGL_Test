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

Chunk::Chunk() {
	constexpr size_t NUM_BLOCKS = Chunk_Constants::Dimension_Size; 
	Chunk_Data data;

	for (uint8_t y = 0; y < NUM_BLOCKS; y++) {
		for (uint8_t z = 0; z < NUM_BLOCKS; z++) {
			for (uint8_t x = 0; x < NUM_BLOCKS; x++) {
				uint8_VEC blockPos = { x, y, z };
				Block& currBlock = blocks[y][z];

				currBlock.setX(x);

				if (!isBoundaryBlock(blockPos))
					continue;
				else {
					if (x == 0) addFace(data, blockPos, FACE_DATA[0]); // WEST
					if (y == 0) addFace(data, blockPos, FACE_DATA[1]); // BOTTOM
					if (z == 0) addFace(data, blockPos, FACE_DATA[2]); // NORTH

					if (x == NUM_BLOCKS - 1) addFace(data, blockPos, FACE_DATA[3]); // EAST
					if (y == NUM_BLOCKS - 1) addFace(data, blockPos, FACE_DATA[4]); // TOP
					if (z == NUM_BLOCKS - 1) addFace(data, blockPos, FACE_DATA[5]); // SOUTH
				}
			}
		}
	}

	chunkData = BufferObjects(data.chunk_vertices, Attributes_Details::objectAttributes, data.chunk_indices);
}

namespace Chunk_Methods {

	std::array<bool, 6> getAdjacentState(const Chunk::Blocks& blocks, const uint8_VEC& block_coordinate) {
		std::array<bool, 6> adjacent_state{};

		for (int i = 0; i < 6; i++) {
			uint8_VEC adjacent_coordinate = block_coordinate.getAdjacentCoordinate(static_cast<Faces>(i));
			adjacent_state[i] = blocks[block_coordinate.y][block_coordinate.z].getX(block_coordinate.x);
		}
		return adjacent_state;
	}
}
