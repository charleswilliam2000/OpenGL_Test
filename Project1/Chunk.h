#ifndef CHUNK_H
#define CHUNK_H

#include "stdafx.h"
#include "Shape.h"
#include "Buffers.h"

#include <array>
#include <vector>
#include <future>

namespace Chunk_Constants {
    constexpr size_t Dimension_Size = 16;
}

struct Block_Attribute {
	uint8_t visibility_mask : 6;  // 6 bits for visibility (1 per face)
	uint8_t padding : 2;

	inline void setFaceVisible(Faces face) { visibility_mask |= (1 << static_cast<int>(face)); }
	inline void setFaceHidden(Faces face) { visibility_mask &= ~(1 << static_cast<int>(face)); }
	inline bool isFaceVisible(Faces face) const { return (visibility_mask & (1 << static_cast<int>(face))) != 0; }
	inline bool isHidden() const { return visibility_mask == 0; }
};

struct Block {
	uint16_t x : 16;
	Block_Attribute attribute[16];

	inline void setX(uint16_t in_x) { x |= (1 << in_x); }
	inline bool getX(uint16_t in_x) const { return (x & (1 << in_x)) != 0; }

};

inline uint32_VEC worldToBlock(float x, float y, float z) {
	x = floor(x + 8);
	y = floor(y + 8);
	z = floor(z + 8);

	return uint32_VEC(x, y, z);
}

struct Chunk {
	using Blocks = std::array<std::array<Block, Chunk_Constants::Dimension_Size>, Chunk_Constants::Dimension_Size>;
private:
	bool checkValidBlock(const uint8_VEC& block_coordinate);
	void updateVisibilityMask(Block& currBlock, const uint8_VEC& blockCoord);
	void insertBlocks(const std::vector<uint8_VEC>& block_coordinates, std::vector<uint8_VEC>& visibleBlocks);

	void generateVertexArray(const Block& block, const uint8_VEC& blockCoordinate, std::array<float, 32>& face_vertex, int face) const {
		size_t arr_index = 0;
		for (int vertex = 0; vertex < 4; vertex++) {
			int index = face * 32 + vertex * 8;

			//Positions
			face_vertex[arr_index++] = Shapes::textured_cube_vertices[index] + blockCoordinate.x;
			face_vertex[arr_index++] = Shapes::textured_cube_vertices[index + 1] + blockCoordinate.y;
			face_vertex[arr_index++] = Shapes::textured_cube_vertices[index + 2] + blockCoordinate.z;
			index += 3;

			//Normals
			face_vertex[arr_index++] = Shapes::textured_cube_vertices[index];
			face_vertex[arr_index++] = Shapes::textured_cube_vertices[index + 1];
			face_vertex[arr_index++] = Shapes::textured_cube_vertices[index + 2];
			index += 3;

			//UV
			face_vertex[arr_index++] = Shapes::textured_cube_vertices[index];
			face_vertex[arr_index++] = Shapes::textured_cube_vertices[index + 1];
		}
	}
	void generateIndexArray(const Block& block, std::array<uint32_t, 6>& block_indices, int face, uint64_t vertexOffset) const {
		for (int i = 0; i < 6; i++) {
			int faceOffset = face * 6 + i;
			uint32_t currIndex = static_cast<uint32_t>(Shapes::cube_indices[faceOffset] + vertexOffset);
			block_indices[i] = currIndex;
		}
	}
	void generateChunk(const std::vector<uint8_VEC>& block_coordinates, std::vector<uint32_t>& chunk_indices, std::vector<float>& chunk_vertex);
public:

	Blocks blocks{};
	BufferObjects chunkData{};

	Chunk(const std::vector<uint8_VEC>& cubeCoordinates);
};

namespace Chunk_Methods {
	std::array<bool, 6> getAdjacentState(const Chunk::Blocks& blocks, const float_VEC& block_coordinate);
}



#endif // CHUNK_H
