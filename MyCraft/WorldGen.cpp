#include "World.h"

std::array<uint8_t, CONSTANTS::Dimension_2DSize> WorldUtils::sampleHeightmap(const siv::PerlinNoise& perlin, uint32_t baseTerrainElevation, const float_VEC& chunkOffset)
{
	std::array<uint8_t, CONSTANTS::Dimension_2DSize> heightmap{};

	constexpr double scale = 80.0, persistence = 0.5, lacunuarity = 1.5;
	constexpr double continentalnessOffset = 881.0, erosionOffset = 244.0, elevationOffset = -111.0;

	auto sampleHeightNoise = [&](double globalX, double globalZ) -> double {
		double frequency = 1.0, amplitude = 1.0;
		double noiseHeight = 0.0;
		
		for (size_t octave = 0; octave < 4; octave++) {
			double sampleX = (globalX / scale) * frequency;
			double sampleZ = (globalZ / scale) * frequency;

			noiseHeight += std::pow((1.0 - perlin.noise2D_01(sampleX, sampleZ)), 4.0) * amplitude;

			amplitude *= persistence;
			frequency *= lacunuarity;
		}

		return noiseHeight;
		};

	auto getMaxHeight = [&](double continentalness, double elevation, double erosion, size_t maxHeight) -> double {
		const auto normalizedContinentalness = 1.0 - std::abs(std::tanh(continentalness));
		const auto normalizedErosion = 2.0 * std::pow(std::exp(std::pow(-1.0 * erosion, 2.0)), 2.0);
		const auto normalizedElevation = -2.0 * std::pow(std::exp(std::pow(-1.0 * elevation, 2.0)), 2.0);
		
		double clampedVal = std::min(std::max((normalizedContinentalness - normalizedErosion) / (normalizedElevation - normalizedErosion), 0.0), 1.0);
		double smoothstepVal = 1.0 - (clampedVal * clampedVal * clampedVal * (clampedVal * (6.0 * clampedVal - 15.0) + 10.0));
		return maxHeight * smoothstepVal;
		};

	for (uint8_t z = 0; z < CONSTANTS::Dimension_1DSize; z++) {
		for (uint8_t x = 0; x < CONSTANTS::Dimension_1DSize; x++) {

			uint32_t i = z * CONSTANTS::Dimension_1DSize + x;

			const double globalX = chunkOffset.x + static_cast<double>(x);
			const double globalZ = chunkOffset.z + static_cast<double>(z);
			const double heightNoise = sampleHeightNoise(globalX, globalZ);

			double frequency = 1.0, amplitude = 2.0, continentalness = 0.0, erosion = 0.0, elevation = 0.0;
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

			uint8_t height = static_cast<uint8_t>(heightNoise * getMaxHeight(continentalness, elevation, erosion, CONSTANTS::MAX_BLOCK_HEIGHT)) + baseTerrainElevation;

			if (height > CONSTANTS::MAX_BLOCK_HEIGHT)
				height = CONSTANTS::MAX_BLOCK_HEIGHT;

			heightmap[i] = height;
		}
	}
	return heightmap;
}

World::World(int gridSize, int verticalSize) :
	_shaderGeometryPass("geometry_vertex.glsl", "geometry_fragment.glsl"),
	_shaderSSAOPass("ssao_vertex.glsl", "ssao_fragment.glsl"),
	_shaderLightingPass("lighting_deferred_vertex.glsl", "lighting_deferred_fragment.glsl"),
	_wireframeShader("wireframe_vertex_shader.glsl", "wireframe_fragment_shader.glsl"),
	_textureAtlas("TextureAtlas.jpg")
{
	_deferred.generateBuffers(CONSTANTS::WINDOW_WIDTH, CONSTANTS::WINDOW_HEIGHT); // Generate the pipeline buffers
	_ssao.generateSSAO(CONSTANTS::WINDOW_WIDTH, CONSTANTS::WINDOW_HEIGHT);

	// --- CONFIGURE SHADERS  --- 
	_shaderGeometryPass.use();
	_shaderGeometryPass.setInt("textureAtlas", 1);

	_shaderSSAOPass.use();
	_shaderSSAOPass.setInt("gTextureArray", 2);
	_shaderSSAOPass.setInt("gDepth", 3);
	_shaderSSAOPass.setInt("ssaoNoise", 6);

	_shaderLightingPass.use();
	_shaderLightingPass.setInt("gTextureArray", 2);
	_shaderLightingPass.setInt("gDepth", 3);
	_shaderLightingPass.setInt("ssaoColorTexture", 4);
	
	// --- PREPARE MESH / DRAWABLE'S BUFFERS / INDIRECT MULTIDRAW COMMANDS --- 
	generateChunks(gridSize, verticalSize);

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
	const uint32_t baseTerrainElevation = static_cast<uint32_t>(verticalCenter) * 20u;

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
					static_cast<float>((chunkX - horizontalCenter) * static_cast<int>(CONSTANTS::Dimension_1DSize)),
					0.0f,
					static_cast<float>((chunkZ - horizontalCenter) * static_cast<int>(CONSTANTS::Dimension_1DSize))
				};

				auto heightmap = WorldUtils::sampleHeightmap(perlin, baseTerrainElevation, chunk2DOffset);

				for (int chunkY = 0; chunkY < verticalSize; chunkY++) {

					float_VEC chunk3DOffset = {
						chunk2DOffset.x,
						static_cast<float>((chunkY - verticalCenter) * static_cast<int>(CONSTANTS::Dimension_1DSize)),
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
						return height -= (height > CONSTANTS::Dimension_1DSize) ? CONSTANTS::Dimension_1DSize : height;
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
						static_cast<float>((chunkX - horizontalCenter) * static_cast<int>(CONSTANTS::Dimension_1DSize)),
						static_cast<float>((chunkY - verticalCenter) * static_cast<int>(CONSTANTS::Dimension_1DSize)),
						static_cast<float>((chunkZ - horizontalCenter) * static_cast<int>(CONSTANTS::Dimension_1DSize))
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
	uint32_t firstIndexOffset = 0;

	std::vector<PackedVertex> renderVertices; 
	std::vector<uint32_t> renderIndices;

	for (size_t i = 0; i < numChunks; i++) {
		const auto& [chunkVertices, chunkIndices] = _chunkMeshes[i].chunkData;
		auto numVertices	= static_cast<uint32_t>(chunkVertices.size());
		auto numIndices		= static_cast<uint32_t>(chunkIndices.size());

		renderVertices.insert(renderVertices.end(), chunkVertices.begin(), chunkVertices.end());
		std::transform(chunkIndices.begin(), chunkIndices.end(), std::back_inserter(renderIndices), [&](uint32_t index) {
			return index + vetexOffset;
			});

		_models.addMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(_chunkMeshes[i].pos)));
		_indirect.addDrawCommand(
			numIndices,															//Counts
			1,																	// Instance count
			firstIndexOffset,													// firstIndex (Index offset after the last chunk)
			0,																	// Base vertex (Should be 0 because we are using 0 big VBO containing all vertices of the chunks)
			0																	// Base instance (No instancing))
		);

		firstIndexOffset += numIndices;
		vetexOffset += numVertices;	
	}

	_models.createUBO(
		STORAGE_TYPE::GL_BUFFER_DATA_STATIC_DRAW, CONSTANTS::MODELS_UBO_BINDING_POINT, 
		STORAGE_TYPE::GL_BUFFER_STORAGE_INCOHERENT, CONSTANTS::NORMAL_MATRICES_BINDING_POINT
	);
	_viewProjection.createUBO(STORAGE_TYPE::GL_BUFFER_STORAGE_INCOHERENT, CONSTANTS::VP_UBO_BINDING_POINT);

	_world.generateBuffersI(
		renderVertices.size() * sizeof(PackedVertex), renderVertices.data(), 
		renderIndices.size() * sizeof(uint32_t), renderIndices.data(),
		DRAWABLE_ATTRIBUTES::DRAWABLE_PACKED_ATTRIBUTES
	);
	_indirect.generateBufferPersistent();
}

