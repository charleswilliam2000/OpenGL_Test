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

struct Face_Data {
	std::array<Vertex, 4> vertices;
	std::array<uint32_t, 6> indices;
};

constexpr std::array<Face_Data, 6> FACE_DATA = { 
	{
		{ // WEST
			{
				{
					{{0, 0, 1}, 0b00001100},
					{{0, 1, 1}, 0b00011100},
					{{0, 1, 0}, 0b00111100},
					{{0, 0, 0}, 0b00101100}
				}
			},
				{ 0, 2, 1, 0, 3, 2 }
		},
		{ // BOTTOM
			{
				{
					{{0, 0, 1}, 0b00011010},
					{{0, 0, 0}, 0b00001010},
					{{1, 0, 0}, 0b00101010},
					{{1, 0, 1}, 0b00111010}
				}

			},
				{0, 2, 1, 0, 3, 2}
		},
		{ // NORTH
			{
				{
					{{0, 0, 0}, 0b00001001},
					{{0, 1, 0}, 0b00011001},
					{{1, 1, 0}, 0b00111001},
					{{1, 0, 0}, 0b00101001}
				}
			},
				{0, 2, 1, 0, 3, 2}
		},
		{ // EAST
			{
				{
					{{1, 0, 1}, 0b00010100},
					{{1, 0, 0}, 0b00000100},
					{{1, 1, 0}, 0b00100100},
					{{1, 1, 1}, 0b00110100}
				}
			},
				{0, 2, 1, 0, 3, 2}
		},
		{ // TOP
			{
				{
					{{0, 1, 1}, 0b00000010},
					{{1, 1, 1}, 0b00100010},
					{{1, 1, 0}, 0b00110010},
					{{0, 1, 0}, 0b00010010}
				}
			},
				{0, 2, 1, 0, 3, 2}
		},
		{ // SOUTH
			{
				{
					{{0, 0, 1}, 0b00000001},
					{{1, 0, 1}, 0b00100001},
					{{1, 1, 1}, 0b00110001},
					{{0, 1, 1}, 0b00010001}
				}
			},
				{0, 2, 1, 0, 3, 2}
		}
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
	//Block_Attribute attribute[16];

	inline void setX(uint16_t in_x) { x |= (1 << in_x); }
	inline bool getX(uint16_t in_x) const { return (x & (1 << in_x)) != 0; }

};

struct Chunk {
	using Blocks = std::array<std::array<Block, Chunk_Constants::Dimension_Size>, Chunk_Constants::Dimension_Size>;
private:
	bool checkValidBlock(const uint8_VEC& block_coordinate);
	bool isBoundaryBlock(const uint8_VEC& pos) const {
		return pos.x != 0 || pos.y != 0 || pos.z != 0 ||
			pos.x != Chunk_Constants::Dimension_Size - 1 || pos.y != Chunk_Constants::Dimension_Size - 1 || pos.z != Chunk_Constants::Dimension_Size - 1;
	}
	
	void addFace(Chunk_Data& data, const uint8_VEC& blockPos, const Face_Data& faceData) const {
		static uint32_t vertexOffset = 0;
		for (const auto& vertex : faceData.vertices) {
			std::array<GLbyte, 3> vertex_pos =
			{
				GLbyte(static_cast<GLbyte>(blockPos.x) + vertex.coordinates[0]),
				GLbyte(static_cast<GLbyte>(blockPos.y) + vertex.coordinates[1]),
				GLbyte(static_cast<GLbyte>(blockPos.z) + vertex.coordinates[2])
			};
			data.chunk_vertices.emplace_back(vertex_pos, vertex.vertex_data);
		}
		for (const auto& indice : faceData.indices) {
			data.chunk_indices.push_back(vertexOffset + indice);
		}
		vertexOffset += 4;
	}

public:

	Blocks blocks{};
	BufferObjects chunkData{};

	Chunk();
};

namespace Chunk_Methods {
	std::array<bool, 6> getAdjacentState(const Chunk::Blocks& blocks, const uint8_VEC& block_coordinate);
}



#endif // CHUNK_H
