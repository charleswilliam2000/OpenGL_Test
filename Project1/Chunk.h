#ifndef CHUNK_H
#define CHUNK_H

#include "stdafx.h"
#include "Shape.h"
#include "Buffers.h"

#include <array>
#include <vector>
#include <memory>

namespace Chunk_Constants {
    constexpr size_t CHUNK_SIZE = 32;
}

struct x_Bitfield {
	uint32_t x : 32;

	inline void setX(uint32_t in_x) { 
		x |= (1 << in_x);
	}

	inline bool getX(uint32_t in_x) const {
		return (x & (1 << in_x)) != 0; // Mask the bit at in_x and check if it's non-zero (i.e., set to 1).
	}
};

struct Chunk {
    using Blocks = std::array<std::array<x_Bitfield, Chunk_Constants::CHUNK_SIZE>, Chunk_Constants::CHUNK_SIZE>;
public:
	std::unique_ptr<Blocks> blocks{};
	Chunk() : blocks(std::make_unique<Blocks>()) {}
};

namespace Chunk_Methods {
	inline uint32_VEC worldToBlock(float x, float y, float z) {
		int out_x = floor(x + 16);
		int out_y = floor(y + 16);
		int out_z = floor(z + 16);

		return uint32_VEC(out_x, out_y, out_z);
	}
	std::array<bool, 6> getAdjacentState(const Chunk::Blocks& blocks, const float_VEC& block_coordinate);

	inline bool checkValidBlock(const float_VEC& block_coordinate) {
		if (block_coordinate.x < -16.0f || block_coordinate.y < -16.0f || block_coordinate.z < -16.0f) {
			std::cerr << "\nOne coordinate is smaller than 0! Coordinates: "
				<< block_coordinate.x << " - " << block_coordinate.y << " - " << block_coordinate.z;
			throw std::runtime_error("\nOne coordinate is smaller than 0!");
		}

		if (block_coordinate.x >= 16.0f || block_coordinate.y >= 16.0f || block_coordinate.z >= 16.0f) {
			std::cerr << "\nOne coordinate is larger than 32! Coordinates: "
				<< block_coordinate.x << " - " << block_coordinate.y << " - " << block_coordinate.z;
			throw std::runtime_error("\nOne coordinate is larger than 32!");
		}
		return true;
	}

	template<size_t arr_size>
	std::array<uint32_t, 6> getFaceIndices(const int& face, const std::array<uint32_t, arr_size>& index_array) {
		std::array<uint32_t, 6> face_indices{};

		int offset = 6 * face;
		for (int i = 0; i < 6; i++) {
			face_indices[i] = index_array[i + offset];
		}
		return face_indices;
	}

	template<size_t arr_size>
	std::vector<uint32_t> generateIndexArray(const Chunk::Blocks& blocks, const uint32_VEC& block_coordinate, 
		const std::array<uint32_t, arr_size>& index_array, std::vector<uint32_t>& indices) {
		indices.clear();
		std::array<bool, 6> adjacent_state{};

		for (int face = 0; face < 6; face++) {
			uint32_VEC adjacent_coordinate = block_coordinate.getAdjacentCoordinate(static_cast<Faces>(face));

			if (adjacent_coordinate == block_coordinate) adjacent_state[face] = true; //Skip
			else {
				adjacent_state[face] = blocks[adjacent_coordinate.y][adjacent_coordinate.z].getX(adjacent_coordinate.x);

				if (!adjacent_state[face]) {
					const auto& face_indices = getFaceIndices(face, index_array);
					indices.insert(indices.end(), face_indices.begin(), face_indices.end());
				}
			}
		}
		return indices;
	}
   
	void insertBlock(Chunk::Blocks& blocks, const uint32_VEC& block_coordinate);
	void insertBlocks(Chunk::Blocks& blocks, const std::vector<float_VEC>& block_coordinates);
	void generateChunk(Chunk::Blocks& blocks, const std::vector<float_VEC>& block_coordinates, std::vector<BufferObjects>& cubes);
}



#endif // CHUNK_H
