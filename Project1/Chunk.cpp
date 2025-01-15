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

void Chunk::getVisibleBlocks(std::vector<uint8_VEC>& visibleBlocks) {
	for (uint8_t y = 0; y < Chunk_Constants::Dimension_Size; y++) {
		for (uint8_t z = 0; z < Chunk_Constants::Dimension_Size; z++) {
			for (uint8_t x = 0; x < Chunk_Constants::Dimension_Size; x++) {
				if (blocks[y][z].attribute[x].isHidden()) continue;
				else visibleBlocks.emplace_back(x, y, z);
			}
		}
	}
}

void Chunk::updateVisibilityMask(Block& currBlock, const uint8_VEC& blockCoord) {
	for (int face = 0; face < 6; face++) {
		Faces currFace = static_cast<Faces>(face);
		uint8_VEC adjacentCoord = blockCoord.getAdjacentCoordinate(currFace);
		if (adjacentCoord == blockCoord) currBlock.attribute[adjacentCoord.x].setFaceVisible(currFace);
		else {
			Block& adjacentBlock = blocks[adjacentCoord.y][adjacentCoord.z];
			if (adjacentBlock.getX(adjacentCoord.x)) {
				currBlock.attribute[blockCoord.x].setFaceHidden(currFace);
				(face % 2 == 0)
					? adjacentBlock.attribute[adjacentCoord.x].setFaceHidden(static_cast<Faces>(face + 1))
					: adjacentBlock.attribute[adjacentCoord.x].setFaceHidden(static_cast<Faces>(face - 1));
			}
			else currBlock.attribute[blockCoord.x].setFaceVisible(currFace);
		}
	}
}


Chunk::Chunk()  {
	for (int i = 0; i < 10; i++) {
		auto start = std::chrono::high_resolution_clock::now();

		constexpr size_t NUM_BLOCKS = Chunk_Constants::Dimension_Size;
		Chunk_Data data;

		for (uint8_t y = 0; y < NUM_BLOCKS; y++) {
			for (uint8_t z = 0; z < NUM_BLOCKS; z++) {
				for (uint8_t x = 0; x < NUM_BLOCKS; x++) {
					uint8_VEC blockPos = { x, y, z };
					Block& currBlock = blocks[y][z];

					currBlock.setX(x);

					if (x != 0 || y != 0 || z != 0 ||
						x != NUM_BLOCKS - 1 || y != NUM_BLOCKS - 1 || z != NUM_BLOCKS - 1)
						continue;

					updateVisibilityMask(currBlock, blockPos);

					if (x == 0) addFace(data, blockPos, FACE_DATA[0]); // WEST
					if (y == 0) addFace(data, blockPos, FACE_DATA[1]); // BOTTOM
					if (z == 0) addFace(data, blockPos, FACE_DATA[2]); // NORTH

					if (x == NUM_BLOCKS - 1) addFace(data, blockPos, FACE_DATA[3]); // EAST
					if (y == NUM_BLOCKS - 1) addFace(data, blockPos, FACE_DATA[4]); // TOP
					if (z == NUM_BLOCKS - 1) addFace(data, blockPos, FACE_DATA[5]); // SOUTH
				}
			}
		}

		chunkData = BufferObjects(data.chunk_vertices, Attributes_Details::objectAttributes, data.chunk_indices);

		auto end = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
		std::cout << "chunk generation time: " << duration << " milliseconds.\n";
	}
}

namespace Chunk_Methods {

	std::array<bool, 6> getAdjacentState(const Chunk::Blocks& blocks, const float_VEC& block_coordinate) {
		std::array<bool, 6> adjacent_state{};

		for (int i = 0; i < 6; i++) {
			float_VEC adjacent_coordinate = block_coordinate.getAdjacentCoordinate(static_cast<Faces>(i));
			const uint32_VEC& blockCoord = worldToBlock(adjacent_coordinate.x, adjacent_coordinate.y, adjacent_coordinate.z);
			adjacent_state[i] = blocks[blockCoord.y][blockCoord.z].getX(blockCoord.x);
		}
		return adjacent_state;
	}
}
