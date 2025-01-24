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
	std::array<uint32_t, 6> indices;
	std::array<Vertex, 4> vertices;
	FACES face;
};

constexpr std::array<Face_Data, 6> FACE_DATA = { 
	{
		{ 
			{ 0, 2, 1, 0, 3, 2 },
			{ //12 in binary: 1100
				Vertex(0, 0, 1, 0, 2, 0), 
				Vertex(0, 1, 1, 0, 3, 0), 
				Vertex(0, 1, 0, 0, 0, 0), 
				Vertex(0, 0, 0, 0, 1, 0)
			},
			{FACES::WEST},
		},
		{					
			{0, 2, 1, 0, 3, 2},
			{ //10 in binary: 1010
				Vertex(0, 0, 1, 1, 1, 0),
				Vertex(0, 0, 0, 1, 0, 0),
				Vertex(1, 0, 0, 1, 2, 0),
				Vertex(1, 0, 1, 1, 3, 0)
			},
			{FACES::BOTTOM},
		},
		{ 
			{0, 2, 1, 0, 3, 2},
			{ //9 in binary: 1001
				Vertex(0, 0, 0, 2, 1, 0),
				Vertex(0, 1, 0, 2, 0, 0),
				Vertex(1, 1, 0, 2, 2, 0),
				Vertex(1, 0, 0, 2, 3, 0)
			},
			{FACES::NORTH},
		},
		{ 
			{0, 2, 1, 0, 3, 2},
			{
				Vertex(1, 0, 1, 3, 1, 0),
				Vertex(1, 0, 0, 3, 3, 0),
				Vertex(1, 1, 0, 3, 2, 0),
				Vertex(1, 1, 1, 3, 0, 0)
			},
			{FACES::EAST},
		},
		{ 
			{0, 2, 1, 0, 3, 2},
			{
				Vertex(0, 1, 1, 4, 0, 0),
				Vertex(1, 1, 1, 4, 2, 0),
				Vertex(1, 1, 0, 4, 3, 0),
				Vertex(0, 1, 0, 4, 1, 0)
			},
			{FACES::TOP},
		},
		{
			{0, 2, 1, 0, 3, 2},
			{
				Vertex(0, 0, 1, 5, 1, 0),
				Vertex(1, 0, 1, 5, 3, 0),
				Vertex(1, 1, 1, 5, 2, 0),
				Vertex(0, 1, 1, 5, 0, 0)
			},
			{FACES::SOUTH},
		}
	}
};

struct Chunk_Data {
	using Vertices = std::vector<Vertex>;
	Vertices chunk_vertices;

	using Indices = std::vector<uint32_t>;
	Indices chunk_indices;
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
	std::pair<uint32_t, uint32_t> numVerticesIndices = { 0, 0 };
	BufferObjects chunkData{};
	float_VEC pos{};

	ChunkMesh() {}
	void addFace(Chunk_Data& data, const uint8_VEC& blockWorldPos, const Face_Data& faceData, const BLOCK_ID& type, uint32_t& vertexOffset) const;
	Chunk_Data generate(float_VEC in_pos, const WorldChunk& worldChunk);
};


#endif // CHUNK_H
