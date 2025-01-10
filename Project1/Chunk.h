#ifndef CHUNK_H
#define CHUNK_H

#include "stdafx.h"

#include <array>
#include <vector>

namespace Chunk_Constants {
    constexpr uint32_t CHUNK_SIZE = 32;
}

enum class Faces : unsigned char {
	NORTH = 0,
	SOUTH = 1,
	WEST = 2,
	EAST = 3,
	TOP = 4,
	BOTTOM = 5
};

struct block_attributes {
    uint16_t x : 6;
    uint16_t visibility_mask : 6;
    uint16_t reserved : 4;

    // Utility Method: Check if a specific face is visible
    inline bool isFaceVisible(uint8_t faceIndex) const {
        return (visibility_mask & (1 << faceIndex)) != 0;
    }

    inline void setFaceVisibility(uint8_t faceIndex, bool visible) {
		visibility_mask = (visible) ? (visibility_mask | (1 << faceIndex)) : (visibility_mask & ~(1 << faceIndex));
    }
};

struct u8int_VEC {
	unsigned char x, y, z;
	u8int_VEC getAdjacentCoordinate(const Faces& face) const;
};

struct Chunk {
	using Block_Attributes = std::array<std::array<block_attributes, 32>, 32>;

	Block_Attributes block_attributes{};
};

namespace Chunk_Methods {
	std::array<bool, 6> getAdjacentState(const Chunk& chunk, u8int_VEC block_coordinate);
	void updateAdjacentCoordinates(Chunk& chunk, u8int_VEC block_coordinate, const std::array<bool, 6>& adjacent_state);

	void insertBlock(Chunk& chunk, u8int_VEC block_coordinate);
	void insertBlocks(Chunk& chunk, const std::vector<u8int_VEC>& block_coordinates);
	bool isBlockExist(const Chunk& chunk, u8int_VEC block_coordinate);
}



#endif // CHUNK_H
