#include "World.h"

std::array<uint8_t, ChunkConstants::Dimension_2DSize> World::sampleHeightmap(const siv::PerlinNoise& perlin, uint32_t baseTerrainElevation, const float_VEC& chunkOffset)
{
	std::array<uint8_t, ChunkConstants::Dimension_2DSize> heightmap{};

	constexpr double scale = 40.0, persistence = 0.5, lacunuarity = 1.25;
	constexpr double continentalnessOffset = 1.0, erosionOffset = 244.0, elevationOffset = -111.0;

	auto sampleHeightNoise = [&](double globalX, double globalZ) -> double {
		double frequency = 1.0, amplitude = 1.5;
		double noiseHeight = 0.0;
		
		for (size_t octave = 0; octave < 4; octave++) {
			double sampleX = (globalX / scale) * frequency;
			double sampleZ = (globalZ / scale) * frequency;

			noiseHeight += std::pow((1.0 - perlin.noise2D_01(sampleX, sampleZ)), 4) * amplitude;

			amplitude *= persistence;
			frequency *= lacunuarity;
		}

		return noiseHeight;
		};

	auto getMaxHeight = [&](double continentalness, double elevation, double erosion, size_t maxHeight) -> double {
		const auto normalizedContinentalness = 1.0 - std::tanh(continentalness);
		const auto normalizedErosion = std::pow(1.0 - (std::tanh(1.25 * erosion) * std::tanh(1.25 * erosion)), 2.0);
		const auto normalizedElevation = std::pow(1.0 - (std::tanh(1.5 * elevation) * std::tanh(1.5 * elevation)), 2.0);
		
		double clampedVal = std::min(std::max((normalizedContinentalness - 4.0 * normalizedErosion) / (-2.0 * normalizedElevation - 4.0 * normalizedErosion), 0.0), 1.0);
		double smoothstepVal = 1.0 - (clampedVal * clampedVal * clampedVal * (clampedVal * (6.0 * clampedVal - 15.0) + 10.0));
		return maxHeight * smoothstepVal;
		};

	for (uint8_t z = 0; z < ChunkConstants::Dimension_1DSize; z++) {
		for (uint8_t x = 0; x < ChunkConstants::Dimension_1DSize; x++) {

			uint32_t i = z * ChunkConstants::Dimension_1DSize + x;

			const double globalX = chunkOffset.x + static_cast<double>(x);
			const double globalZ = chunkOffset.z + static_cast<double>(z);
			const double heightNoise = sampleHeightNoise(globalX, globalZ);

			double frequency = 1.0, amplitude = 1.5, continentalness = 0.0, erosion = 0.0, elevation = 0.0;
			for (size_t octave = 0; octave < 4; octave++) {
				const auto sampleContinentalnessX = ((globalX + continentalnessOffset) / scale) * frequency;
				const auto sampleContinentalnessZ = ((globalZ + continentalnessOffset) / scale) * frequency;

				const auto sampleErosionX = ((globalX + erosionOffset) / scale) * frequency;
				const auto sampleErosionZ = ((globalZ + erosionOffset) / scale) * frequency;

				const auto sampleElevationX = ((globalX + elevationOffset) / scale) * frequency;
				const auto sampleElevationZ = ((globalZ + elevationOffset) / scale) * frequency;


				continentalness += perlin.noise2D(sampleContinentalnessX, sampleContinentalnessZ) * amplitude;
				erosion += perlin.noise2D(sampleErosionX, sampleErosionZ) * amplitude;
				elevation += perlin.noise2D(sampleElevationX, sampleElevationZ) * amplitude;

				amplitude *= persistence;
				frequency *= lacunuarity;
			}

			uint8_t height = static_cast<uint8_t>(heightNoise * getMaxHeight(continentalness, elevation, erosion, Constants::MAX_BLOCK_HEIGHT)) + baseTerrainElevation;

			if (height > Constants::MAX_BLOCK_HEIGHT)
				height = Constants::MAX_BLOCK_HEIGHT;

			heightmap[i] = height;
		}
	}
	return heightmap;
}

void World::generateChunks(int gridSize, int verticalSize)
{
	static const siv::PerlinNoise::seed_type seed = 123456u;
	static const siv::PerlinNoise perlin{ seed };

	const size_t numChunks = (gridSize * gridSize * verticalSize);
	_worldChunks.resize(numChunks);
	_chunkMeshes.resize(numChunks);

	const int horizontalCenter = gridSize / 2; // 3 = 1; 5 = 2; 7 = 3;
	const int verticalCenter = verticalSize / 2;
	const uint32_t baseTerrainElevation = verticalCenter * 20;

	constexpr std::array<std::pair<FACES, int32_VEC>, 6> neighborOffsets = { {
		{FACES::WEST,	{-1, 0, 0}},
		{FACES::BOTTOM, {0, -1, 0}},
		{FACES::NORTH,	{0, 0, -1}},

		{FACES::EAST,	{1, 0, 0}},
		{FACES::TOP,	{0, 1, 0}},
		{FACES::SOUTH,	{0, 0, 1}}
		}
	};

	const size_t numThreads = std::min<size_t>(std::thread::hardware_concurrency() - 1, numChunks);
	static ChunkGenerationThread chunkThreadPool(numThreads);

	std::vector<std::future<void>> terrainFutures(gridSize * gridSize);
	for (int chunkZ = 0; chunkZ < gridSize; chunkZ++) {
		for (int chunkX = 0; chunkX < gridSize; chunkX++) {

			size_t i = chunkZ * gridSize + chunkX;

			terrainFutures[i] = chunkThreadPool.enqueueTask([&, i, chunkX, chunkZ]() -> void {

				float_VEC chunk2DOffset = {
					static_cast<float>((chunkX - horizontalCenter) * static_cast<int>(ChunkConstants::Dimension_1DSize)),
					0.0f,
					static_cast<float>((chunkZ - horizontalCenter) * static_cast<int>(ChunkConstants::Dimension_1DSize))
				};

				auto heightmap = sampleHeightmap(perlin, baseTerrainElevation, chunk2DOffset);

				for (int chunkY = 0; chunkY < verticalSize; chunkY++) {

					float_VEC chunk3DOffset = {
						chunk2DOffset.x,
						static_cast<float>((chunkY - verticalCenter) * static_cast<int>(ChunkConstants::Dimension_1DSize)),
						chunk2DOffset.z
					};

					size_t chunkIndex = chunkY * (gridSize * gridSize) + i;
					_worldChunks[chunkIndex].generate(perlin, chunk3DOffset, heightmap);

					for (const auto& neighbor : neighborOffsets) {

						auto neighborX = chunkX + neighbor.second.x;
						auto neighborY = chunkY + neighbor.second.y;
						auto neighborZ = chunkZ + neighbor.second.z;

						if (neighborX >= 0 && neighborX < gridSize &&
							neighborY >= 0 && neighborY < verticalSize &&
							neighborZ >= 0 && neighborZ < gridSize)

						{
							size_t neighborIndex = neighborY * (gridSize * gridSize) + neighborZ * gridSize + neighborX;
							_worldChunks[chunkIndex].neighborChunks.neighbors[static_cast<int>(neighbor.first)] = &_worldChunks[neighborIndex];
						}
					}

					std::transform(heightmap.begin(), heightmap.end(), heightmap.begin(), [&](uint8_t height) {
						return height -= (height > ChunkConstants::Dimension_1DSize) ? ChunkConstants::Dimension_1DSize : height;
						});
				}

				});
		}
	}

	for (auto& future : terrainFutures) {
		future.get();
	}

	std::vector<std::future<void>> meshFutures(numChunks);
	for (int chunkY = 0; chunkY < verticalSize; chunkY++) {
		for (int chunkZ = 0; chunkZ < gridSize; chunkZ++) {
			for (int chunkX = 0; chunkX < gridSize; chunkX++) {
				size_t i = chunkY * (gridSize * gridSize) + chunkZ * gridSize + chunkX;

				meshFutures[i] = chunkThreadPool.enqueueTask([&, i, chunkX, chunkY, chunkZ]() -> void { 

					float_VEC chunkOffset = {
						static_cast<float>((chunkX - horizontalCenter) * static_cast<int>(ChunkConstants::Dimension_1DSize)),
						static_cast<float>((chunkY - verticalCenter) * static_cast<int>(ChunkConstants::Dimension_1DSize)),
						static_cast<float>((chunkZ - horizontalCenter) * static_cast<int>(ChunkConstants::Dimension_1DSize))
					};

					_chunkMeshes[i].pos = chunkOffset;
					_chunkMeshes[i].generate(_worldChunks[i]);
					});
			}
		}
	}

	for (auto& future : meshFutures) {
		future.get();
	}


	uint32_t vetexOffset = 0;
	std::vector<Vertex> renderedVertices; std::vector<uint32_t> renderedIndices;

	for (size_t i = 0; i < numChunks; i++) {
		const auto& [chunkVertices, chunkIndices] = _chunkMeshes[i].chunkData;
		auto numVertices = static_cast<uint32_t>(chunkVertices.size());
		auto numIndices = static_cast<uint32_t>(chunkIndices.size());

		_worldVerticesIndices.first += numVertices; _worldVerticesIndices.second += numIndices;
		_chunkMeshes[i].numVerticesIndices = std::make_pair(numVertices, numIndices);

		renderedVertices.insert(renderedVertices.end(), chunkVertices.begin(), chunkVertices.end());
		std::transform(chunkIndices.begin(), chunkIndices.end(), std::back_inserter(renderedIndices), [&](uint32_t index) {
			return index + vetexOffset;
			});

		_indirect.modelMatrices.push_back(
			glm::translate(
				glm::mat4(1.0f),
				glm::vec3(_chunkMeshes[i].pos)
			)
		);
		vetexOffset += numVertices;

	}

	glGenBuffers(1, &_indirect.modelUniformBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, _indirect.modelUniformBuffer);
	glBufferData(GL_UNIFORM_BUFFER, _indirect.modelMatrices.size() * sizeof(glm::mat4), _indirect.modelMatrices.data(), GL_STATIC_DRAW);

	_worldBuffers = DrawableBufferObjects(
		renderedVertices,
		Attributes_Details::voxelPackedAttributes,
		renderedIndices
	);

	uint32_t firstIndexOffset = 0;
	for (const auto& chunkMesh : _chunkMeshes) {
		_indirect.drawCommands.emplace_back(
			static_cast<uint32_t>(chunkMesh.numVerticesIndices.second), //Counts
			1,															// Instance count
			firstIndexOffset,											// firstIndex (Index offset after the last chunk)
			0,															// Base vertex (Should be 0 because we are using 0 big VBO containing all vertices of the chunks)
			0															// Base instance (No instancing)
		);
		firstIndexOffset += static_cast<uint32_t>(chunkMesh.numVerticesIndices.second);
	}

	glGenBuffers(1, &_indirect.indirectBuffer);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, _indirect.indirectBuffer);

	glBufferStorage(GL_DRAW_INDIRECT_BUFFER, _indirect.drawCommands.size() * sizeof(IndirectRendering::DrawCommands), _indirect.drawCommands.data(),
		GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);

	_indirect.indirectBufferPersistentPtr =
		static_cast<IndirectRendering::DrawCommands*>(
			glMapBufferRange(
				GL_DRAW_INDIRECT_BUFFER,
				0,
				_indirect.drawCommands.size() * sizeof(IndirectRendering::DrawCommands),
				GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT
			)
			);

	if (!_indirect.indirectBufferPersistentPtr)
		throw std::runtime_error("\nUnable to initialize indirectBufferPersistentPtr");

	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
}