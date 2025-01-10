#include "Chunk.h"

u8int_VEC u8int_VEC::getAdjacentCoordinate(const Faces& face) const {
	switch (face) {
	case Faces::NORTH:		return { x, y, static_cast<unsigned char>(z - 1) };
	case Faces::SOUTH:		return { x, y, static_cast<unsigned char>(z + 1) };
	case Faces::EAST:		return { static_cast<unsigned char>(x + 1), y, z };
	case Faces::WEST:		return { static_cast<unsigned char>(x - 1), y, z };
	case Faces::TOP:		return { x, static_cast<unsigned char>(y + 1), z };
	case Faces::BOTTOM:		return { x, static_cast<unsigned char>(y - 1), z };

	}
	std::cerr << "\nUnable to determine face!";
	throw std::runtime_error("\nFace not recognized!");
}

namespace Chunk_Methods {
	void insertBlock(Chunk& chunk, u8int_VEC block_coordinate) {
		uint16_t block_x = static_cast<uint16_t>(block_coordinate.x);
		uint16_t block_y = static_cast<uint16_t>(block_coordinate.y);
		uint16_t block_z = static_cast<uint16_t>(block_coordinate.z);

		if (block_x < 0 || block_y < 0 || block_z < 0) {
			std::cerr << "\nOne coordinate is smaller than 0! Coordinates: " << block_x << " - " << block_y << " - " << block_z;
			throw std::runtime_error("\nOne coordinate is smaller than 0!");
		}

		if (block_x >= Chunk_Constants::CHUNK_SIZE || block_y >= Chunk_Constants::CHUNK_SIZE || block_z >= Chunk_Constants::CHUNK_SIZE) {
			std::cerr << "\nOne coordinate is larger than 32! Coordinates: " << block_x << " - " << block_y << " - " << block_z;
			throw std::runtime_error("\nOne coordinate is larger than 32!");
		}

		chunk.block_attributes[block_y][block_z].x = block_x;

		std::array<bool, 6> adjacent_state = getAdjacentState(chunk, block_coordinate);

		uint16_t visibility_mask = 0;
		for (int i = 0; i < 6; i++) {
			if (!adjacent_state[i]) 
				visibility_mask |= (1 << i);
				
		}

		chunk.block_attributes[block_y][block_z].visibility_mask = visibility_mask;
		
		//Update visibility mask of adjacent coordinates
		updateAdjacentCoordinates(chunk, block_coordinate, adjacent_state);
	}


	void insertBlocks(Chunk& chunk, const std::vector<u8int_VEC>& block_coordinates) {
		for (const auto& block_coordinate : block_coordinates) {
			insertBlock(chunk, block_coordinate);
		}
	}
	bool isBlockExist(const Chunk& chunk, u8int_VEC block_coordinate) {
		uint16_t block_x = static_cast<uint16_t>(block_coordinate.x);
		uint16_t block_y = static_cast<uint16_t>(block_coordinate.y);
		uint16_t block_z = static_cast<uint16_t>(block_coordinate.z);

		if (block_x < 0 || block_y < 0 || block_z < 0 || 
			block_x >= Chunk_Constants::CHUNK_SIZE || block_y >= Chunk_Constants::CHUNK_SIZE || block_z >= Chunk_Constants::CHUNK_SIZE) 
			return false;
		
		return chunk.block_attributes[block_y][block_z].x == block_x;
	}
	std::array<bool, 6> getAdjacentState(const Chunk& chunk, u8int_VEC block_coordinate) {
		std::array<bool, 6> adjacent_state{};

		uint16_t block_x = static_cast<uint16_t>(block_coordinate.x);
		uint16_t block_y = static_cast<uint16_t>(block_coordinate.y);
		uint16_t block_z = static_cast<uint16_t>(block_coordinate.z);

		_ASSERT_EXPR(block_x > 0 && block_y > 0 && block_z > 0, "\nA coordinate is negative");
		_ASSERT_EXPR(block_x <= Chunk_Constants::CHUNK_SIZE && block_y <= Chunk_Constants::CHUNK_SIZE && block_z <= Chunk_Constants::CHUNK_SIZE, 
			"\nA coordinate exceeded Chunk_Constants::CHUNK_SIZE");
		
		// 6 faces
		for (int i = 0; i < 6; i++) {
			u8int_VEC adjacent_coordinate = block_coordinate.getAdjacentCoordinate(static_cast<Faces>(i));
			adjacent_state[i] = isBlockExist(chunk, adjacent_coordinate);
		}
		return adjacent_state;
	}

	void updateAdjacentCoordinates(Chunk& chunk, u8int_VEC block_coordinate, const std::array<bool, 6>& adjacent_state) {
		for (int i = 0; i < 6; i++) {
			if (adjacent_state[i]) {
				u8int_VEC adjacent_coordinate = block_coordinate.getAdjacentCoordinate(static_cast<Faces>(i));
				uint16_t adj_y = static_cast<uint16_t>(adjacent_coordinate.y);
				uint16_t adj_z = static_cast<uint16_t>(adjacent_coordinate.z);

				uint16_t adj_visibility_mask = chunk.block_attributes[adj_y][adj_z].visibility_mask;

				// Compute opposite face's bit position
				uint16_t opposite_mask = ~(1 << ((i % 2 == 0) ? (i + 1) : (i - 1)));
				adj_visibility_mask &= opposite_mask;

				chunk.block_attributes[adj_y][adj_z].visibility_mask = adj_visibility_mask;
			}
		}
	}
}
