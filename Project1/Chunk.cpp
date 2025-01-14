#include "Chunk.h"

bool Chunk::checkValidBlock(const uint8_VEC& block_coordinate) {
	if (block_coordinate.x < 0 || block_coordinate.y < 0 || block_coordinate.z < 0) {
		std::cerr << "\nOne coordinate is smaller than 0! Coordinates: "
			<< block_coordinate.x << " - " << block_coordinate.y << " - " << block_coordinate.z;
		throw std::runtime_error("\nOne coordinate is smaller than 0!");
	}

	if (block_coordinate.x >= 16 || block_coordinate.y >= 16 || block_coordinate.z >= 16) {
		std::cerr << "\nOne coordinate is larger than 32! Coordinates: "
			<< block_coordinate.x << " - " << block_coordinate.y << " - " << block_coordinate.z;
		throw std::runtime_error("\nOne coordinate is larger than 32!");
	}
	return true;
}

void Chunk::insertBlocks(const std::vector<uint8_VEC>& block_coordinates, std::vector<uint8_VEC>& visibleBlocks) {
	for (const auto& blockCoord : block_coordinates) {
		checkValidBlock(blockCoord);
		Block& currentBlock = blocks[blockCoord.y][blockCoord.z];
		currentBlock.setX(blockCoord.x);
		updateVisibilityMask(currentBlock, blockCoord);
	}

	for (const auto& blockCoord : block_coordinates) {
		Block& currentBlock = blocks[blockCoord.y][blockCoord.z];
		if (!currentBlock.attribute[blockCoord.x].isHidden()) {
			visibleBlocks.emplace_back(blockCoord.x, blockCoord.y, blockCoord.z);
		}
	}
}

void Chunk::updateVisibilityMask(Block& currBlock, const uint8_VEC& blockCoord) {
	for (int face = 0; face < 6; face++) {
		Faces currFace = static_cast<Faces>(face);
		uint8_VEC adjacentCoord = blockCoord.getAdjacentCoordinate(currFace);
		if (adjacentCoord == blockCoord) currBlock.attribute[adjacentCoord.x].setFaceVisible(currFace);
		else {
			Block& adjacentBlock = blocks[adjacentCoord.y][adjacentCoord.z];
			if (adjacentBlock.getX(adjacentCoord.x)) {
				currBlock.attribute[blockCoord.x].setFaceHidden(currFace);
				(face % 2 == 0)
					? adjacentBlock.attribute[adjacentCoord.x].setFaceHidden(static_cast<Faces>(face + 1))
					: adjacentBlock.attribute[adjacentCoord.x].setFaceHidden(static_cast<Faces>(face - 1));
			}
			else currBlock.attribute[blockCoord.x].setFaceVisible(currFace);
		}
	}
}
void Chunk::generateChunk(const std::vector<uint8_VEC>& block_coordinates, std::vector<uint32_t>& chunk_indices, std::vector<float>& chunk_vertex) {
	std::vector<uint8_VEC> visibleBlocks{}; visibleBlocks.reserve(Constants::MAX_VISIBLE_BLOCKS);
	insertBlocks(block_coordinates, visibleBlocks);
		
	uint64_t vertexOffset = 0;
	size_t numThreads = std::thread::hardware_concurrency();
	size_t blocksPerThread = visibleBlocks.size() / numThreads;

	std::vector<std::future<std::pair<std::vector<float>, std::vector<uint32_t>>>> futures;
	
	for(size_t t = 0; t < numThreads; t++) {
		size_t start = t * blocksPerThread;
		size_t end = (t == numThreads - 1) ? visibleBlocks.size() : start + blocksPerThread;

		uint64_t threadVertexOffset = vertexOffset;
		vertexOffset += (end - start) * 24;
		futures.emplace_back(std::async(std::launch::async, [&, start, end, threadVertexOffset]() {
			std::vector<float> local_vertex; local_vertex.reserve((end - start) * 32 * sizeof(float));
			std::vector<uint32_t> local_indices; local_indices.reserve((end - start) * 6 * sizeof(uint32_t));

			uint64_t localOffset = threadVertexOffset;

			for (size_t i = start; i < end; i++) {
				const uint8_VEC& blockCoord = visibleBlocks[i];
				const Block& currBlock = blocks[blockCoord.y][blockCoord.z];

				for (int face = 0; face < 6; face++) {
					std::array<uint32_t, 6> face_indices{};
					std::array<float, 32> face_vertex{};

					generateVertexArray(currBlock, blockCoord, face_vertex, face);
					if (currBlock.attribute[blockCoord.x].isFaceVisible(static_cast<Faces>(face))) {
						generateIndexArray(currBlock, face_indices, face, localOffset);
					}
					local_vertex.insert(local_vertex.end(), face_vertex.begin(), face_vertex.end());
					local_indices.insert(local_indices.end(), face_indices.begin(), face_indices.end());
				}

				localOffset += 24; // Increment for the next block
			}

			return std::make_pair(local_vertex, local_indices);
		}));
	}

	for (auto& future : futures) {
		auto it = future.get();
		chunk_vertex.insert(chunk_vertex.end(), it.first.begin(), it.first.end());
		chunk_indices.insert(chunk_indices.end(), it.second.begin(), it.second.end());
	}
}


Chunk::Chunk(const std::vector<uint8_VEC>& cubeCoordinates)  {
	for (int i = 0; i < 10; i++) {
		auto start = std::chrono::high_resolution_clock::now();

		std::vector<uint32_t> chunk_indices{}; chunk_indices.reserve(Constants::MAX_VISIBLE_BLOCKS * 6 * 6);
		std::vector<float> chunk_vertex{}; chunk_vertex.reserve(Constants::MAX_VISIBLE_BLOCKS * 6 * 32);
		generateChunk(cubeCoordinates, chunk_indices, chunk_vertex);
		chunkData = BufferObjects(chunk_vertex, Attributes_Details::objectAttributes, chunk_indices);

		auto end = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
		std::cout << "chunk generation time: " << duration << " milliseconds.\n";
	}
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
