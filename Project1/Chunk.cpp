#include "Chunk.h"

namespace Chunk_Methods {

	std::array<bool, 6> getAdjacentState(const Chunk::Blocks& blocks, const float_VEC& block_coordinate) {
		std::array<bool, 6> adjacent_state{};

		checkValidBlock(block_coordinate);
		// 6 faces
		for (int i = 0; i < 6; i++) {
			float_VEC adjacent_coordinate = block_coordinate.getAdjacentCoordinate(static_cast<Faces>(i));
			checkValidBlock(adjacent_coordinate);
			const uint32_VEC& blockCoord = worldToBlock(adjacent_coordinate.x, adjacent_coordinate.y, adjacent_coordinate.z);
			adjacent_state[i] = blocks[blockCoord.y][blockCoord.z].getX(blockCoord.x);
		}
		return adjacent_state;
	}


	void insertBlock(Chunk::Blocks& blocks, const uint32_VEC& block_coordinate) {
		blocks[block_coordinate.y][block_coordinate.z].setX(block_coordinate.x);
	}

	
    void insertBlocks(Chunk::Blocks& blocks, const std::vector<float_VEC>& block_coordinates) {
        for (const auto& block_coordinate : block_coordinates) {
			checkValidBlock(block_coordinate);
			const uint32_VEC& blockCoord = worldToBlock(block_coordinate.x, block_coordinate.y, block_coordinate.z);
			blocks[blockCoord.y][blockCoord.z].setX(blockCoord.x);
        }
    }
	void generateChunk(Chunk::Blocks& blocks, const std::vector<float_VEC>& block_coordinates, std::vector<BufferObjects>& cubes) {
		insertBlocks(blocks, block_coordinates);
		GLuint sharedVBO;
		glGenBuffers(1, &sharedVBO);
		glBindBuffer(GL_ARRAY_BUFFER, sharedVBO);
		glBufferData(GL_ARRAY_BUFFER, Shapes::textured_cube_vertices.size() * sizeof(float), Shapes::textured_cube_vertices.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind to avoid accidental modification

		std::vector<uint32_t> indices{};
		for (const auto& block_coordinate : block_coordinates) {
			const uint32_VEC& blockCoord = worldToBlock(block_coordinate.x, block_coordinate.y, block_coordinate.z);
			const std::vector<uint32_t>& block_indices = Chunk_Methods::generateIndexArray(blocks, blockCoord, Shapes::cube_indices, indices);
			cubes.emplace_back(Attributes_Details::objectAttributes, block_indices, sharedVBO);
			
		}
	}
}
