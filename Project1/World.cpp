#include "World.h"

double PerlinNoise::multi3D_Noise(double x, double y, double z, int octaves, double persistence, double lacunarity, double initialFrequency, double initialAmplitude) const {
	double total = 0.0;
	double maxValue = 0.0; // Used for normalization
	double frequency = initialFrequency;
	double amplitude = initialAmplitude;

	for (int i = 0; i < octaves; i++) {
		total += noise(x * frequency, y * frequency, z * frequency) * amplitude;

		maxValue += amplitude;

		frequency *= lacunarity;
		amplitude *= persistence;
	}

	return total / maxValue;
}



void World::generateTerrain(const siv::PerlinNoise& perlin, Chunk::Blocks& chunkTerrain, const float_VEC& chunkOffset) const
{

	double frequencyX = 0.01 + chunkOffset.x * 0.001;
	double frequencyY = 0.01 + chunkOffset.y * 0.001;
	double frequencyZ = 0.01 + chunkOffset.z * 0.001;

	const double chunkOffsetX = static_cast<double>(chunkOffset.x);
	const double chunkOffsetY = static_cast<double>(chunkOffset.y);
	const double chunkOffsetZ = static_cast<double>(chunkOffset.z);

	double totalValue = 0.0f; int numTotal = 0; double avgVal = 0.0f; 

	for (uint8_t y = 0; y < Chunk_Constants::Dimension_1DSize; y++) {
		for (uint8_t z = 0; z < Chunk_Constants::Dimension_1DSize; z++) {
			for (uint8_t x = 0; x < Chunk_Constants::Dimension_1DSize; x++) {
				if (y < 13) chunkTerrain[y][z].setX(x);
				else if (y > 14) break;
				else {

					double 
						globalX = (chunkOffsetX + static_cast<double>(x)) * frequencyX,
						globalY = (chunkOffsetY + static_cast<double>(y)) * frequencyY,
						globalZ = (chunkOffsetZ + static_cast<double>(z)) * frequencyZ;

					double perlinVal = perlin.octave3D_01(globalX, globalY, globalZ, 4, 0.5);
					totalValue += perlinVal; ++numTotal; avgVal = totalValue / static_cast<double>(numTotal);

					if (perlinVal < avgVal) chunkTerrain[y][z].setX(x);
				}
			}
		}
	}
	//std::cout << "Average value: " << avgVal << "\n";
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

				generateTerrain(perlin, _chunks[i].blocks, chunkOffset);

				for (const auto& neighbor : neighborOffsets) {
					int neighborX = static_cast<int>(x) + neighbor.second.x;
					int neighborZ = static_cast<int>(z) + neighbor.second.z;

					if (neighborX >= 0 && neighborX < static_cast<int>(gridSize) &&
						neighborZ >= 0 && neighborZ < static_cast<int>(gridSize)) {
						size_t neighborIndex = neighborZ * gridSize + neighborX;
						_chunks[i].neightborChunks.neighbors[static_cast<int>(neighbor.first)] = &_chunks[neighborIndex];
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

				auto chunkData = _chunks[i].generate(chunkOffset);
				threadResults.push_back(std::make_pair(i, std::move(chunkData)));
			}

			return threadResults;
		}));
	}

	for (auto& future : meshFuture) {
		auto threadResults = future.get(); 
		size_t numChunks = threadResults.size();
		for (const auto& [chunkIndex, chunkData] : threadResults) {

			_chunks[chunkIndex].chunkData = BufferObjects(
				chunkData.chunk_vertices, 
				Attributes_Details::objectAttributes,
				chunkData.chunk_indices
			);

		}
	}
}
