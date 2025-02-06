#ifndef CHUNK_H
#define CHUNK_H

#include <functional>

#include "Frustum.h"
#include "Buffers.h"

#include "PerlinNoise.hpp"

namespace ChunkConstants {
    constexpr size_t Dimension_1DSize = 16;
	constexpr size_t Dimension_2DSize = 256;
	constexpr size_t Dimension_3DSize = 4096;
}

struct ChunkData {
	using Vertices = std::vector<Vertex>;
	Vertices chunk_vertices;

	using Indices = std::vector<uint32_t>;
	Indices chunk_indices;
};

enum class BLOCK_ID : uint8_t {
	AIR  = 0,
	DIRT = 1,
	GRASS = 2,
	STONE = 3
};

struct Block_ID {
	uint64_t ID : 64; // 4 bits for every one block (Thus, 4 x 16 = 64 bits)
	inline void setID(BLOCK_ID id, uint64_t x) {
		if (x < ChunkConstants::Dimension_1DSize) {
			uint64_t mask = ~(0xFULL); 
			uint64_t shiftAmount = static_cast<uint64_t>(4) * x;

			ID &= ~(mask << shiftAmount); // Reset existing ID in the block
			ID |= (static_cast<uint64_t>(id) << shiftAmount); // Set the new ID
		}
		else throw std::runtime_error("\nAssigned X exceeded chunk limit");
		
	}

	inline BLOCK_ID getID(uint64_t x) const {
		if (x < ChunkConstants::Dimension_1DSize) {
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
	using Blocks = std::array<std::array<Block_ID, ChunkConstants::Dimension_1DSize>, ChunkConstants::Dimension_1DSize>;
	using SolidBlocks = std::vector<std::pair<uint8_VEC, BLOCK_ID>>;

	Blocks blocks{};
	SolidBlocks solidBlocks{};

	struct NeighborChunks {
		std::array<WorldChunk*, 6> neighbors;

		NeighborChunks() {
			neighbors.fill(nullptr);
		}
		//Six neighbors WEST / BOTTOM / NORTH / EAST / TOP / SOUTH
	} neighborChunks;

	bool isBlockSolid(uint8_t x, uint8_t y, uint8_t z) const {
		return blocks[y][z].getID(x) != BLOCK_ID::AIR;
	}

	uint32_t getVisibleFaces(
		const uint8_VEC& block_coordinate,
		const uint8_VEC& chunkMinBounds,
		const uint8_VEC& chunkMaxBounds,
		const std::function<bool(const FACES& face, uint8_VEC pos)>& getNeighborChunkBlock
	) const;

	void generate(
		const siv::PerlinNoise& perlin, const float_VEC& chunkOffset, 
		const std::array<uint8_t, ChunkConstants::Dimension_2DSize>& heightmap, 
		int chunkY
	);
};

struct ChunkMesh {
	ChunkData chunkData{};
	BufferObjects chunkBuffers{};
	float_VEC pos{};
	std::pair<uint32_t, uint32_t> numVerticesIndices = { 0, 0 };

	ChunkMesh() {}

	void addFace(const uint8_VEC& blockWorldPos, const Face_Data& faceData, const BLOCK_ID& type, uint32_t& vertexOffset);
	void generate(const WorldChunk& worldChunk);
	AABB getBoundingBox() const {
		float_VEC worldMin = pos;
		float_VEC worldMax = worldMin + float_VEC(static_cast<float>(ChunkConstants::Dimension_1DSize));

		return { worldMin, worldMax };
	}
};


#endif // CHUNK_H
