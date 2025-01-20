#include "World.h"

void World::generateTerrain(const siv::PerlinNoise& perlin, WorldChunk::Blocks& chunkTerrain, const float_VEC& chunkOffset) const
{
	const double frequencyX = 0.001 + chunkOffset.x * 0.00001;
	const double frequencyZ = 0.001 + chunkOffset.z * 0.00001;

	const double chunkOffsetX = static_cast<double>(chunkOffset.x);
	const double chunkOffsetZ = static_cast<double>(chunkOffset.z);

	for (uint8_t z = 0; z < Chunk_Constants::Dimension_1DSize; z++) {
		for (uint8_t x = 0; x < Chunk_Constants::Dimension_1DSize; x++) {
			double globalX = (chunkOffsetX + static_cast<double>(x)) * frequencyX;
			double globalZ = (chunkOffsetZ + static_cast<double>(z)) * frequencyZ;

			double perlinVal = std::pow(perlin.octave2D_01(globalX, globalZ, 8, 0.5), 2);
			int height = static_cast<int>(perlinVal * static_cast<double>(Chunk_Constants::Dimension_1DSize));
			_ASSERT_EXPR(height < Chunk_Constants::Dimension_1DSize, "\nHeight higher than chunk's dimension");

			for (uint8_t y = 0; y < height; y++) {
				chunkTerrain[y][z].setID(BLOCK_ID::DIRT, x);
			}
		}
	}
}

void World::generateChunk(size_t gridSize)
{
	const siv::PerlinNoise::seed_type seed = 123456u;
	const siv::PerlinNoise perlin{ seed };

	_chunks.resize(gridSize * gridSize);

	constexpr std::array<std::pair<FACES, int32_VEC>, 4> neighborOffsets = { {
		{FACES::WEST,	{-1, 0, 0}},
		{FACES::NORTH,	{0, 0, -1}},

		{FACES::EAST,	{1, 0, 0}},
		{FACES::SOUTH,	{0, 0, 1}}
		}
	};

	const size_t numThreads = std::min<size_t>(std::thread::hardware_concurrency(), gridSize * gridSize);;
	std::vector<std::future<void>> terrainFuture;

	// Divide the total chunks among threads
	size_t chunksPerThread = (gridSize * gridSize + numThreads - 1) / numThreads;

	for (size_t thread = 0; thread < numThreads; thread++) {
		terrainFuture.emplace_back(std::async(std::launch::async, [&, thread]() {
			size_t start = thread * chunksPerThread;
			size_t end = std::min(start + chunksPerThread, gridSize * gridSize);

			for (size_t i = start; i < end; i++) {
				size_t z = i / gridSize;
				size_t x = i % gridSize;

				float_VEC chunkOffset = {
					static_cast<float>(x) * 16.0f,
					0.0f,
					static_cast<float>(z) * 16.0f
				};

				generateTerrain(perlin, _chunks[i].first.blocks, chunkOffset);

				for (const auto& neighbor : neighborOffsets) {
					int neighborX = static_cast<int>(x) + neighbor.second.x;
					int neighborZ = static_cast<int>(z) + neighbor.second.z;

					if (neighborX >= 0 && neighborX < static_cast<int>(gridSize) &&
						neighborZ >= 0 && neighborZ < static_cast<int>(gridSize)) {
						size_t neighborIndex = neighborZ * gridSize + neighborX;
						_chunks[i].first.neighborChunks.neighbors[static_cast<int>(neighbor.first)] = &_chunks[neighborIndex].first;
					}
				}
			}
			}));
	}

	for (auto& future : terrainFuture) {
		future.get();
	} 

	std::vector<std::future<std::vector<std::pair<size_t, Chunk_Data>>>> meshFuture;
	for (size_t thread = 0; thread < numThreads; thread++) {
		meshFuture.emplace_back(std::async(std::launch::async, [&, thread]() {
			size_t start = thread * chunksPerThread;
			size_t end = std::min(start + chunksPerThread, gridSize * gridSize);

			std::vector<std::pair<size_t, Chunk_Data>> threadResults; 

			for (size_t i = start; i < end; i++) {
				size_t z = i / gridSize;
				size_t x = i % gridSize;
				float_VEC chunkOffset = {
					static_cast<float>(x) * 16.0f,
					0.0f,
					static_cast<float>(z) * 16.0f
				};

				threadResults.emplace_back(i, _chunks[i].second.generate(chunkOffset, _chunks[i].first));
			}

			return threadResults;
		}));
	}

	for (auto& future : meshFuture) {
		const auto& threadResults = future.get(); 
		for (const auto& [chunkIndex, chunkData] : threadResults) {

			_chunks[chunkIndex].second.chunkData = BufferObjects(
				chunkData.chunk_vertices, 
				Attributes_Details::objectAttributes,
				chunkData.chunk_indices
			);

		}
	}
}
