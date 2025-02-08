#include "Chunk.h"

void ChunkMesh::addVisibleFaces(
	const WorldChunk& worldChunk,
	const BLOCK_ID& blockType,
	const uint8_VEC& blockCoordinate,
	uint32_t& vertexOffset
) 
{
	uint8_VEC chunkMin = { 0, 0, 0 };
	uint8_VEC chunkMax = { 15, 15, 15 };

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

	uint8_t
		x = blockCoordinate.x,
		y = blockCoordinate.y,
		z = blockCoordinate.z;

	if (x == chunkMin.x) { // West
		if (!getNeighborChunkBlock(FACES::WEST, {x, y, z})) {
			addFace({ x, y, z }, FACE_DATA[static_cast<int>(FACES::WEST)], blockType, vertexOffset);
		}
	}
	else if (!worldChunk.isBlockSolid(x - 1, y, z)) {
		addFace({ x, y, z }, FACE_DATA[static_cast<int>(FACES::WEST)], blockType, vertexOffset);
	}

	if (y == chunkMin.y) { // Bottom
		if (!getNeighborChunkBlock(FACES::BOTTOM, { x, y, z })) {
			addFace({ x, y, z }, FACE_DATA[static_cast<int>(FACES::BOTTOM)], blockType, vertexOffset);

		}
	}
	else if (!worldChunk.isBlockSolid(x, y - 1, z)) {
		addFace({ x, y, z }, FACE_DATA[static_cast<int>(FACES::BOTTOM)], blockType, vertexOffset);
	}

	if (z == chunkMin.z) { // North
		if (!getNeighborChunkBlock(FACES::NORTH, { x, y, z })) {
			addFace({ x, y, z }, FACE_DATA[static_cast<int>(FACES::NORTH)], blockType, vertexOffset);

		}
	}
	else if (!worldChunk.isBlockSolid(x, y, z - 1)) {
		addFace({ x, y, z }, FACE_DATA[static_cast<int>(FACES::NORTH)], blockType, vertexOffset);
	}

	if (x == chunkMax.x) { // East
		if (!getNeighborChunkBlock(FACES::EAST, { x, y, z })) {
			addFace({ x, y, z }, FACE_DATA[static_cast<int>(FACES::EAST)], blockType, vertexOffset);
		}
	}
	else if (!worldChunk.isBlockSolid(x + 1, y, z)) {
		addFace({ x, y, z }, FACE_DATA[static_cast<int>(FACES::EAST)], blockType, vertexOffset);
	}

	if (y == chunkMax.y) { // Top
		if (!getNeighborChunkBlock(FACES::TOP, { x, y, z })) {
			addFace({ x, y, z }, FACE_DATA[static_cast<int>(FACES::TOP)], blockType, vertexOffset);
		}
	}
	else if (!worldChunk.isBlockSolid(x, y + 1, z)) {
		addFace({ x, y, z }, FACE_DATA[static_cast<int>(FACES::TOP)], blockType, vertexOffset);
	}

	if (z == chunkMax.y) { // South
		if (!getNeighborChunkBlock(FACES::SOUTH, { x, y, z })) {
			addFace({ x, y, z }, FACE_DATA[static_cast<int>(FACES::SOUTH)], blockType, vertexOffset);
		}
	}
	else if (!worldChunk.isBlockSolid(x, y, z + 1)) {
		addFace({ x, y, z }, FACE_DATA[static_cast<int>(FACES::SOUTH)], blockType, vertexOffset);
	}
}

void WorldChunk::generate(
	const siv::PerlinNoise& perlin, const float_VEC& chunkOffset, 
	const std::array<uint8_t, ChunkConstants::Dimension_2DSize>& heightmap
)
{
	constexpr double scale = 30.0, persistence = 0.5, lacunuarity = 1.5;
	auto sampleSolidNoise = [&](double globalX, double globalY, double globalZ) -> double {
		double frequency = 1.0, amplitude = 1.0, solidNoise = 0.0;
		for (size_t octave = 0; octave < 4; octave++) {
			double
				sampleX = (globalX / scale) * frequency,
				sampleY = (globalY / scale) * frequency,
				sampleZ = (globalZ / scale) * frequency;

			solidNoise += perlin.noise3D_01(sampleX, sampleY, sampleZ) * amplitude;

			amplitude *= persistence;
			frequency *= lacunuarity;
		}
		return solidNoise;
	};

	auto getChunkSolidThreshold = [&](float_VEC chunkOffset) -> double {
		const double normalizedChunkY = std::tanh((chunkOffset.y / static_cast<double>(ChunkConstants::Dimension_1DSize) / 3.0)); // Normalize to [-1, 1]
		const double sampleChunkY = 0.8 * std::sin(2.5 * normalizedChunkY + 2.5);

		constexpr double minDensity = 0.3, maxDensity = 0.7;
		return (maxDensity * sampleChunkY) + minDensity; // The larger sampleChunkY, the larger the density threshold 
	};

	double chunkSolidThreshold = getChunkSolidThreshold(chunkOffset);

	for (size_t z = 0; z < ChunkConstants::Dimension_1DSize; z++) {
		for (size_t x = 0; x < ChunkConstants::Dimension_1DSize; x++) {

			auto i = z * ChunkConstants::Dimension_1DSize + x;
			const auto columnTotalHeight = heightmap[i];
			const auto columnHeight = (columnTotalHeight / 16u == 0) ? columnTotalHeight : ChunkConstants::Dimension_1DSize;

			for (size_t y = 0; y < columnHeight; y++) {
				const double globalX = chunkOffset.x + static_cast<double>(x);
				const double globalY = chunkOffset.y + static_cast<double>(y);
				const double globalZ = chunkOffset.z + static_cast<double>(z);
				const double solidNoise = sampleSolidNoise(globalX, globalY, globalZ);

				if (solidNoise >= chunkSolidThreshold) {
					if (globalY <= 0) {
						blocks[y][z].setID(BLOCK_ID::STONE, x);
						solidBlocks.emplace_back(uint8_VEC{ x, y, z }, BLOCK_ID::STONE);
					}
					else if (globalY > 0 && y < columnTotalHeight - 1) {
						blocks[y][z].setID(BLOCK_ID::DIRT, x);
						solidBlocks.emplace_back(uint8_VEC{ x, y, z }, BLOCK_ID::DIRT);
					}
					else if (globalY > 0 && y == columnTotalHeight - 1) {
						blocks[y][z].setID(BLOCK_ID::GRASS, x);
						solidBlocks.emplace_back(uint8_VEC{ x, y, z }, BLOCK_ID::GRASS);
					}
				}
				else continue;
			}
		}
	} 
}

void ChunkMesh::addFace(const uint8_VEC& blockWorldPos, const Face_Data& faceData, const BLOCK_ID& type, uint32_t& vertexOffset) {
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
		chunkData.chunk_vertices.emplace_back(vertex_pos[0], vertex_pos[1], vertex_pos[2], vertex.normals, vertex.uv, textureIndex);
	}
	for (const auto& indice : faceData.indices) {
		chunkData.chunk_indices.push_back(vertexOffset + indice);
	}
	vertexOffset += 4;
}

void ChunkMesh::generate(const WorldChunk& worldChunk) {
	uint32_t vertexOffset = 0;
	auto& solidBlocks = worldChunk.solidBlocks;
	for (const auto& solidBlock : solidBlocks) {
		uint8_VEC blockCoordinate = solidBlock.first; 
		BLOCK_ID blockType = solidBlock.second;
		addVisibleFaces(worldChunk, blockType, blockCoordinate, vertexOffset);
	}
}