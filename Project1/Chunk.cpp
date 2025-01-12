#include "Chunk.h"

bool Chunk::checkValidBlock(const float_VEC& block_coordinate) {
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

void Chunk::insertBlocks(const std::vector<float_VEC>& block_coordinates) {
	for (const auto& block_coordinate : block_coordinates) {
		checkValidBlock(block_coordinate);
		const uint32_VEC& blockCoord = worldToBlock(block_coordinate.x, block_coordinate.y, block_coordinate.z);

		Block& currentBlock = blocks[blockCoord.y][blockCoord.z];
		currentBlock.setX(blockCoord.x);
	}
}

void Chunk::updateVisibilityMask(const std::vector<float_VEC>& block_coordinates) {
	for (const auto& block_coordinate : block_coordinates) {
		const uint32_VEC& blockCoord = worldToBlock(block_coordinate.x, block_coordinate.y, block_coordinate.z);
		Block& currBlock = blocks[blockCoord.y][blockCoord.z];

		for (int face = 0; face < 6; face++) {
			Faces currFace = static_cast<Faces>(face);
			uint32_VEC adjacentCoord = blockCoord.getAdjacentCoordinate(currFace);
			if (adjacentCoord == blockCoord) {
				currBlock.setFaceVisible(currFace);
				continue;
			}

			Block& adjacentBlock = blocks[adjacentCoord.y][adjacentCoord.z];
			if (adjacentBlock.getX(adjacentCoord.x)) {
				currBlock.setFaceHidden(currFace);
				(face % 2 == 0) ? adjacentBlock.setFaceHidden(static_cast<Faces>(face + 1)) : adjacentBlock.setFaceHidden(static_cast<Faces>(face - 1));
			}
			else currBlock.setFaceVisible(currFace);
		}
	}
}

void Chunk::generateVertexArray(const Block& block, const uint32_VEC& blockCoordinate, std::vector<float>& block_vertices) const {
	block_vertices.clear();

	for (int face = 0; face < 6; face++) {
		if (block.isFaceVisible(static_cast<Faces>(face))) {
			for (int vertex = 0; vertex < 4; vertex++) {
				int index = face * 32 + vertex * 8;
				
				block_vertices.push_back(Shapes::textured_cube_vertices[static_cast<std::array<float, 192Ui64>::size_type>(index)] + blockCoordinate.x);
				block_vertices.push_back(Shapes::textured_cube_vertices[static_cast<std::array<float, 192Ui64>::size_type>(index) + 1] + blockCoordinate.y);
				block_vertices.push_back(Shapes::textured_cube_vertices[static_cast<std::array<float, 192Ui64>::size_type>(index) + 2] + blockCoordinate.z);
				index += 3;

				for (int normals = 0; normals < 3; normals++) {
					int normalsOffset = index + normals;
					block_vertices.push_back(Shapes::textured_cube_vertices[normalsOffset]);
				} index += 3;

				for (int uv = 0; uv < 2; uv++) {
					int uvOffset = index + uv;
					block_vertices.push_back(Shapes::textured_cube_vertices[uvOffset]);
				}
			}
		}
	}
}

void Chunk::generateIndexArray(const Block& block, std::vector<uint32_t>& block_indices, uint64_t vertexOffset) const {
	block_indices.clear();
	for (int face = 0; face < 6; face++) {
		if (block.isFaceVisible(static_cast<Faces>(face))) {
			for (int i = 0; i < 6; i++) {
				int faceOffset = face * 6 + i;
				uint32_t currIndex = static_cast<uint32_t>(Shapes::cube_indices[faceOffset] + vertexOffset);
				block_indices.push_back(currIndex);
			}
		}
	}
}

void Chunk::generateChunk(const std::vector<float_VEC>& block_coordinates, std::vector<uint32_t>& chunk_indices, std::vector<float>& chunk_vertex) {
	insertBlocks(block_coordinates);
	updateVisibilityMask(block_coordinates);

	std::vector<uint32_t> block_indices{};
	std::vector<float> block_vertex{};

	uint64_t vertexOffset = 0;
	uint32_t vertexStride = 8;

	for (const auto& block_coordinate : block_coordinates) {
		const uint32_VEC& blockCoord = worldToBlock(block_coordinate.x, block_coordinate.y, block_coordinate.z);
		Block currBlock = blocks[blockCoord.y][blockCoord.z];

		if (!currBlock.isHidden()) {
			generateVertexArray(currBlock, blockCoord, block_vertex);
			generateIndexArray(currBlock, block_indices, vertexOffset);

			chunk_vertex.insert(chunk_vertex.end(), block_vertex.begin(), block_vertex.end());
			chunk_indices.insert(chunk_indices.end(), block_indices.begin(), block_indices.end());
			vertexOffset += block_vertex.size() / vertexStride;
		}
	}
}

Chunk::Chunk(const std::vector<float_VEC>& cubeCoordinates)  {
    std::vector<uint32_t> chunk_indices{}; 
    std::vector<float> chunk_vertex{};
    generateChunk(cubeCoordinates, chunk_indices, chunk_vertex);
	
	size_t num_vertices = chunk_vertex.size();
	for (int i = 0; i < num_vertices; i += 8) {
		std::cout << "\nVertices: "
			<< chunk_vertex[i] << ", "
			<< chunk_vertex[i + 1] << ", "
			<< chunk_vertex[i + 2] << ", "
			<< chunk_vertex[i + 3] << ", "
			<< chunk_vertex[i + 4] << ", "
			<< chunk_vertex[i + 5] << ", "
			<< chunk_vertex[i + 6] << ", "
			<< chunk_vertex[i + 7] << "\n";
	}

    chunkData = BufferObjects(chunk_vertex, Attributes_Details::objectAttributes, chunk_indices);
}

namespace Chunk_Methods {

	std::array<bool, 6> getAdjacentState(const Chunk::Blocks& blocks, const float_VEC& block_coordinate) {
		std::array<bool, 6> adjacent_state{};

		for (int i = 0; i < 6; i++) {
			float_VEC adjacent_coordinate = block_coordinate.getAdjacentCoordinate(static_cast<Faces>(i));
			const uint32_VEC& blockCoord = worldToBlock(adjacent_coordinate.x, adjacent_coordinate.y, adjacent_coordinate.z);
			adjacent_state[i] = blocks[blockCoord.y][blockCoord.z].getX(blockCoord.x);
		}
		return adjacent_state;
	}
}
