#ifndef CHUNK_H
#define CHUNK_H

#include "stdafx.h"
#include "Shape.h"
#include "Buffers.h"

#include <array>
#include <vector>
#include <functional>

namespace Chunk_Constants {
    constexpr size_t Dimension_1DSize = 16;
	constexpr size_t Dimension_2DSize = 256;
	constexpr size_t Dimension_3DSize = 4096;
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
					{{0, 0, 1}, 0b01101100},
					{{0, 1, 1}, 0b01111100},
					{{0, 1, 0}, 0b01001100},
					{{0, 0, 0}, 0b01011100}
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
					{{0, 0, 0}, 0b01011001},
					{{0, 1, 0}, 0b01001001},
					{{1, 1, 0}, 0b01101001},
					{{1, 0, 0}, 0b01111001}
				}
			},
				{0, 2, 1, 0, 3, 2}
		},
		{ // EAST
			{
				{
					{{1, 0, 1}, 0b01010100},
					{{1, 0, 0}, 0b01110100},
					{{1, 1, 0}, 0b01100100},
					{{1, 1, 1}, 0b01000100}
				}
			},
				{0, 2, 1, 0, 3, 2}
		},
		{ // TOP
			{
				{
					{{0, 1, 1}, 0b10000010},
					{{1, 1, 1}, 0b10100010},
					{{1, 1, 0}, 0b10110010},
					{{0, 1, 0}, 0b10010010}
				}
			},
				{0, 2, 1, 0, 3, 2}
		},
		{ // SOUTH
			{
				{
					{{0, 0, 1}, 0b01010001},
					{{1, 0, 1}, 0b01110001},
					{{1, 1, 1}, 0b01100001},
					{{0, 1, 1}, 0b01000001}
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

	inline void setFaceVisible(FACES face) { visibility_mask |= (1 << static_cast<int>(face)); }
	inline void setFaceHidden(FACES face) { visibility_mask &= ~(1 << static_cast<int>(face)); }
	inline bool isFaceVisible(FACES face) const { return (visibility_mask & (1 << static_cast<int>(face))) != 0; }
	inline bool isHidden() const { return visibility_mask == 0; }
};

enum class BLOCK_ID : uint64_t {
	AIR  = 0,
	DIRT = 1,
	GRASS = 2,
	STONE = 3
};

struct Block_ID {
	uint64_t ID : 64; // 4 bits for every one block (Thus, 4 x 16 = 64 bits)
	inline void setID(BLOCK_ID id, uint64_t x) {
		if (x < Chunk_Constants::Dimension_1DSize) {
			uint64_t mask = ~(0xFULL); 
			uint64_t shiftAmount = static_cast<uint64_t>(4) * x;

			ID &= ~(mask << shiftAmount); // Reset existing ID in the block
			ID |= (static_cast<uint64_t>(id) << shiftAmount); // Set the new ID
		}
		else throw std::runtime_error("\nAssigned X exceeded chunk limit");
		
	}

	inline BLOCK_ID getID(uint64_t x) const {
		if (x < Chunk_Constants::Dimension_1DSize) {
			uint64_t shiftAmount = static_cast<uint64_t>(4) * x;
			uint64_t mask = 0xFULL; // Mask for the lowest 4 bits

			uint64_t result = (ID >> shiftAmount) & mask; // Extract relevant 4 bits
			return static_cast<BLOCK_ID>(result);
		}
		else {
			throw std::runtime_error("\nAssigned X exceeded chunk limit");
		}
	}

	Block_ID() : ID(0) {}
};

struct WorldChunk {
	using Blocks = std::array<std::array<Block_ID, Chunk_Constants::Dimension_1DSize>, Chunk_Constants::Dimension_1DSize>;

	struct NeighborChunks {
		std::array<WorldChunk*, 6> neighbors;

		NeighborChunks() {
			neighbors.fill(nullptr);
		}
		//Six neighbors WEST / BOTTOM / NORTH / EAST / TOP / SOUTH
	} neighborChunks;

	Blocks blocks{};

	bool isBlockSolid(uint8_t x, uint8_t y, uint8_t z) const {
		return blocks[y][z].getID(x) != BLOCK_ID::AIR;
	}

	uint32_t getVisibleFaces(
		const uint8_VEC& block_coordinate,
		const uint8_VEC& chunkMinBounds,
		const uint8_VEC& chunkMaxBounds,
		const std::function<bool(const FACES& face, uint8_VEC pos)>& getNeighborChunkBlock
	) const;
};

struct ChunkMesh {
	void addFace(Chunk_Data& data, const uint8_VEC& blockWorldPos, const Face_Data& faceData, uint32_t& vertexOffset) const {
		for (const auto& vertex : faceData.vertices) {
			std::array<GLbyte, 3> vertex_pos =
			{
				GLbyte(static_cast<GLbyte>(blockWorldPos.x) + vertex.coordinates[0]),
				GLbyte(static_cast<GLbyte>(blockWorldPos.y) + vertex.coordinates[1]),
				GLbyte(static_cast<GLbyte>(blockWorldPos.z) + vertex.coordinates[2])
			};
			data.chunk_vertices.emplace_back(vertex_pos, vertex.vertex_data);
		}
		for (const auto& indice : faceData.indices) {
			data.chunk_indices.push_back(vertexOffset + indice);
		}
		vertexOffset += 4;
	}

public:
	BufferObjects chunkData{};
	float_VEC pos{};

	ChunkMesh() {}
	Chunk_Data generate(float_VEC in_pos, const WorldChunk& worldChunk);
};


#endif // CHUNK_H
