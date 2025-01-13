#ifndef CHUNK_H
#define CHUNK_H

#include "stdafx.h"
#include "Shape.h"
#include "Buffers.h"

#include <array>
#include <vector>
#include <memory>

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
	bool checkValidBlock(const float_VEC& block_coordinate);
	void updateVisibilityMask(Block& currBlock, const uint32_VEC& blockCoord);
	void insertBlocks(const std::vector<float_VEC>& block_coordinates);

	void generateVertexArray(const Block& block, const uint32_VEC& blockCoordinate, std::vector<float>& block_vertices, int face) const;
	void generateIndexArray(const Block& block, const uint32_VEC& blockCoordinat, std::vector<uint32_t>& indices, int face, uint64_t vertexOffset) const;
	void generateChunk(const std::vector<float_VEC>& block_coordinates, std::vector<uint32_t>& chunk_indices, std::vector<float>& chunk_vertex);
public:

	Blocks blocks{};
	BufferObjects chunkData{};

	Chunk(const std::vector<float_VEC>& cubeCoordinates);
};

namespace Chunk_Methods {
	std::array<bool, 6> getAdjacentState(const Chunk::Blocks& blocks, const float_VEC& block_coordinate);
}



#endif // CHUNK_H
