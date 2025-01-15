#ifndef CHUNK_H
#define CHUNK_H

#include "stdafx.h"
#include "Shape.h"
#include "Buffers.h"

#include <array>
#include <vector>

namespace Chunk_Constants {
    constexpr size_t Dimension_Size = 16;
}

struct Vertex {
	uint8_VEC coordinates = { 0, 0, 0 };
	uint8_t vertex_data = 0; // 4 for normals, and 2 for uv. The rest is unusued bits

	Vertex() {}
	Vertex(uint8_VEC coordinates, uint8_t vertex_data) : coordinates(coordinates), vertex_data(vertex_data) {}
};

struct Face_Data {
	uint8_VEC offsets[4];
	uint8_t vertex_data[4];
	uint32_t indices[6];
};

constexpr std::array<Face_Data, 6> FACE_DATA = {
	{
		// WEST
		{{uint8_VEC{0, 0, 0}, uint8_VEC{0, 1, 0}, uint8_VEC{0, 1, 1}, uint8_VEC{0, 0, 1}},
		 {0b00101100, 0b00111100, 0b00011100, 0b00001100},
		 {0, 2, 1, 0, 3, 2}},
		 // BOTTOM
		{{uint8_VEC{0, 0, 0}, uint8_VEC{1, 0, 0}, uint8_VEC{1, 0, 1}, uint8_VEC{0, 0, 1}},
		{0b00011010, 0b00111010, 0b00101010, 0b00001010},
		{0, 2, 1, 0, 3, 2}},
		// NORTH
		{{uint8_VEC{0, 0, 0}, uint8_VEC{1, 0, 0}, uint8_VEC{1, 1, 0}, uint8_VEC{0, 1, 0}},
		{0b00001001, 0b00101001, 0b00111001, 0b00011001},
		{0, 2, 1, 0, 3, 2}},
		// EAST
		{{uint8_VEC{1, 0, 0}, uint8_VEC{1, 1, 0}, uint8_VEC{1, 1, 1}, uint8_VEC{1, 0, 1}},
		{0b00100100, 0b00110100, 0b00010100, 0b00000100},
		{0, 2, 1, 0, 3, 2}},
		// TOP
		{{uint8_VEC{0, 1, 0}, uint8_VEC{1, 1, 0}, uint8_VEC{1, 1, 1}, uint8_VEC{0, 1, 1}},
		{0b00010010, 0b00110010, 0b00100010, 0b00000010},
		{0, 2, 1, 0, 3, 2}},
		// SOUTH
		{{uint8_VEC{0, 0, 1}, uint8_VEC{1, 0, 1}, uint8_VEC{1, 1, 1}, uint8_VEC{0, 1, 1}},
		{0b00000001, 0b00100001, 0b00110001, 0b00010001},
		{0, 2, 1, 0, 3, 2}},
	}
};

struct Chunk_Data {
	using Vertices = std::vector<Vertex>;
	Vertices chunk_vertices;

	using Indices = std::vector<uint32_t>;
	Indices chunk_indices;
};

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
	void getVisibleBlocks(std::vector<uint8_VEC>& visibleBlocks);
	
	void addFace(Chunk_Data& data, const uint8_VEC& blockPos, const Face_Data& faceData) {
		static size_t vertexOffset = 0;
		for (size_t i = 0; i < 4; i++) {
			uint8_VEC vertex_pos = 
			{
				uint8_t(blockPos.x + faceData.offsets[i].x),
				uint8_t(blockPos.y + faceData.offsets[i].y),
				uint8_t(blockPos.z + faceData.offsets[i].z)
			};
			data.chunk_vertices.emplace_back(vertex_pos, faceData.vertex_data[i]);
		}
		for (size_t i = 0; i < 6; i++) {
			data.chunk_indices.push_back(vertexOffset + faceData.indices[i]);
		}
		vertexOffset += 4;
	}

public:

	Blocks blocks{};
	BufferObjects chunkData{};

	Chunk();
};

namespace Chunk_Methods {
	std::array<bool, 6> getAdjacentState(const Chunk::Blocks& blocks, const float_VEC& block_coordinate);
}



#endif // CHUNK_H
