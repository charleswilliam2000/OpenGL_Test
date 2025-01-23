#include "Chunk.h"

uint32_t WorldChunk::getVisibleFaces(
	const uint8_VEC& block_coordinate, 
	const uint8_VEC& chunkMinBounds, 
	const uint8_VEC& chunkMaxBounds, 
	const std::function<bool(const FACES& face, uint8_VEC pos)>& getNeighborChunkBlock) const
{
	
	uint8_t
		x = block_coordinate.x,
		y = block_coordinate.y,
		z = block_coordinate.z;

	uint32_t numVisibleFaces = 0;

	if (x == chunkMinBounds.x) { // West
		if (!getNeighborChunkBlock(FACES::WEST, block_coordinate)) {
			numVisibleFaces *= 10;
			numVisibleFaces += 1;
		}
	}
	else if (!isBlockSolid(x - 1, y, z)) {
		numVisibleFaces *= 10;
		numVisibleFaces += 1;
	}

	if (y == chunkMinBounds.y) { // Bottom
		if (!getNeighborChunkBlock(FACES::BOTTOM, block_coordinate)) {
			numVisibleFaces *= 10;
			numVisibleFaces += 2;
		}
	}
	else if (!isBlockSolid(x, y - 1, z)) {
		numVisibleFaces *= 10;
		numVisibleFaces += 2;
	}

	if (z == chunkMinBounds.z) { // North
		if (!getNeighborChunkBlock(FACES::NORTH, block_coordinate)) {
			numVisibleFaces *= 10;
			numVisibleFaces += 3;
		}
	}
	else if (!isBlockSolid(x, y, z - 1)) {
		numVisibleFaces *= 10;
		numVisibleFaces += 3;
	}

	if (x == chunkMaxBounds.x) { // East
		if (!getNeighborChunkBlock(FACES::EAST, block_coordinate)) {
			numVisibleFaces *= 10;
			numVisibleFaces += 4;
		}
	}
	else if (!isBlockSolid(x + 1, y, z)) {
		numVisibleFaces *= 10;
		numVisibleFaces += 4;
	}

	if (y == chunkMaxBounds.y) { // Top
		if (!getNeighborChunkBlock(FACES::TOP, block_coordinate)) {
			numVisibleFaces *= 10;
			numVisibleFaces += 5;
		}
	}
	else if (!isBlockSolid(x, y + 1, z)) {
		numVisibleFaces *= 10;
		numVisibleFaces += 5;
	}

	if (z == chunkMaxBounds.y) { // South
		if (!getNeighborChunkBlock(FACES::SOUTH, block_coordinate)) {
			numVisibleFaces *= 10;
			numVisibleFaces += 6;
		}
	}
	else if (!isBlockSolid(x, y, z + 1)) {
		numVisibleFaces *= 10;
		numVisibleFaces += 6;
	}

	return numVisibleFaces;
}

void ChunkMesh::addFace(Chunk_Data& data, const uint8_VEC& blockWorldPos, const Face_Data& faceData, const BLOCK_ID& type, uint32_t& vertexOffset) const {
	auto getGrassTexture = [](FACES face) -> GLuint {
		switch (face) {
		case FACES::TOP:
			return 2;
		case FACES::BOTTOM:
			return 0;
		default:
			return 1;
		}
		};

	for (const auto& vertex : faceData.vertices) {
		std::array<GLuint, 3> vertex_pos =
		{
			GLuint(static_cast<GLuint>(blockWorldPos.x) + vertex.x),
			GLuint(static_cast<GLuint>(blockWorldPos.y) + vertex.y),
			GLuint(static_cast<GLuint>(blockWorldPos.z) + vertex.z)
		};

		GLuint  textureIndex = 0;
		switch (type) {
		case BLOCK_ID::GRASS:
			textureIndex = getGrassTexture(faceData.face);
			break;
		case BLOCK_ID::DIRT:
			textureIndex = 0;
			break;
		case BLOCK_ID::STONE:
			textureIndex = 3;
			break;
		default:
			std::cerr << "\nInvalid block ID! ID was" << static_cast<int>(type);
			throw std::runtime_error("\nEncountered undefined ID");
		}
		data.chunk_vertices.emplace_back(vertex_pos[0], vertex_pos[1], vertex_pos[2], vertex.normals, vertex.uv, textureIndex);
	}
	for (const auto& indice : faceData.indices) {
		data.chunk_indices.push_back(vertexOffset + indice);
	}
	vertexOffset += 4;
}

Chunk_Data ChunkMesh::generate(float_VEC in_pos, const WorldChunk& worldChunk) {
	pos = in_pos;

	uint8_VEC chunkMin = { 0, 0, 0 };
	uint8_VEC chunkMax = { 15, 15, 15 };

	Chunk_Data data;
	uint32_t vertexOffset = 0;

	auto getNeighborChunkBlock = [&](const FACES& face, uint8_VEC pos) -> bool {
		switch (face) {
		case FACES::WEST: 
			return worldChunk.neighborChunks.neighbors[static_cast<int>(face)] && worldChunk.neighborChunks.neighbors[static_cast<int>(face)]->isBlockSolid(15, pos.y, pos.z);
		case FACES::BOTTOM:
			return worldChunk.neighborChunks.neighbors[static_cast<int>(face)] && worldChunk.neighborChunks.neighbors[static_cast<int>(face)]->isBlockSolid(pos.x, 15, pos.z);
		case FACES::NORTH:
			return worldChunk.neighborChunks.neighbors[static_cast<int>(face)] && worldChunk.neighborChunks.neighbors[static_cast<int>(face)]->isBlockSolid(pos.x, pos.y, 15);
		case FACES::EAST:
			return worldChunk.neighborChunks.neighbors[static_cast<int>(face)] && worldChunk.neighborChunks.neighbors[static_cast<int>(face)]->isBlockSolid(0, pos.y, pos.z);
		case FACES::TOP:
			return worldChunk.neighborChunks.neighbors[static_cast<int>(face)] && worldChunk.neighborChunks.neighbors[static_cast<int>(face)]->isBlockSolid(pos.x, 0, pos.z);
		case FACES::SOUTH:
			return worldChunk.neighborChunks.neighbors[static_cast<int>(face)] && worldChunk.neighborChunks.neighbors[static_cast<int>(face)]->isBlockSolid(pos.x, pos.y, 0);
		default:
			throw std::runtime_error("\nUnable to deduce face!");
		}
		};

	for (uint8_t y = 0; y < Chunk_Constants::Dimension_1DSize; y++) {
		for (uint8_t z = 0; z < Chunk_Constants::Dimension_1DSize; z++) {
			for (uint8_t x = 0; x < Chunk_Constants::Dimension_1DSize; x++) {

				if (!worldChunk.isBlockSolid(x, y, z)) 
					continue;

				uint8_VEC blockCoordinate = { x, y, z }; 
				uint32_t visibleFaces = worldChunk.getVisibleFaces(blockCoordinate, chunkMin, chunkMax, getNeighborChunkBlock);

				if (visibleFaces == 0) 
					continue;

				BLOCK_ID blockType = worldChunk.blocks[y][z].getID(static_cast<uint64_t>(x));
				while (visibleFaces != 0) {
					uint32_t currFace = (visibleFaces % 10) - 1; // Minus one because of added offset 
					addFace(data, blockCoordinate, FACE_DATA[currFace], blockType, vertexOffset);
					visibleFaces /= 10;
				}


			}
		}
	}
	return data;
}