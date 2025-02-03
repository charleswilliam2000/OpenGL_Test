#include "World.h"

GLenum glCheckError_(const char* file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
		case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;

}

void World::setDirectionalLightUniform() const {
	std::array<UniformsVEC3, 4> directionalUniformsVEC3 = { 
		{
			{ "directional_light.direction",	glm::vec3(-0.2f, -1.0f, -0.3f)	},
			{ "directional_light.ambient",		glm::vec3(0.2f, 0.2f, 0.2f)		},
			{ "directional_light.diffuse",		glm::vec3(0.75f, 0.75f, 0.75f)		},
			{ "directional_light.specular",		glm::vec3(1.0f, 1.0f, 1.0f)		}
		}
	};

	for (const auto& dir : directionalUniformsVEC3)
		_worldShader.setUniformVec3(dir.first, dir.second);
}

void World::setPointLightsUniform() const {
	std::array<UniformsVEC3, 4> pointLightUniformsVEC3 = { 
		{
			{ "point_light[0].position",	glm::vec3(23.0f, 23.0f, 23.0f)},
			{ "point_light[0].ambient",		glm::vec3(0.05f, 0.05f, 0.05f) },
			{ "point_light[0].diffuse",		glm::vec3(0.8f, 0.8f, 0.8f)	   },
			{ "point_light[0].specular",	glm::vec3(1.0f, 1.0f, 1.0f)	   }
		} 
	};
	
	for (const auto& point : pointLightUniformsVEC3)
		_worldShader.setUniformVec3(point.first, point.second);

	std::array<Uniforms1F, 3> pointLightUniform1F = { 
		{
			{ "point_light[0].constant",	0.5f },
			{ "point_light[0].linear",		0.09f },
			{ "point_light[0].quadratic",	0.032f },
		} 
	};

	for (const auto& point : pointLightUniform1F)
		_worldShader.setUniform1f(point.first, point.second);
}

std::array<uint32_t, ChunkConstants::Dimension_2DSize> World::sampleHeightmap(const float_VEC& chunkOffset, const size_t& maxHeight)
{
	std::array<uint32_t, ChunkConstants::Dimension_2DSize> heightmap{};
	static const siv::PerlinNoise::seed_type seed = 123456u;
	static const siv::PerlinNoise perlin{ seed };

	constexpr double scale = 30.0, persistence = 0.5, lacunuarity = 1.5;
	constexpr size_t octaves = 4ull;

	auto getMaxHeight = [&](double continentalness, size_t maxHeight) -> double {
		double normalized = (continentalness + 1.0) * 0.5;

		double factor = normalized * normalized * (3.0 - 2.0 * normalized); 

		return (0.25 + (0.75 * factor)) * static_cast<double>(maxHeight);
		};

	for (uint32_t z = 0; z < ChunkConstants::Dimension_1DSize; z++) {
		for (uint32_t x = 0; x < ChunkConstants::Dimension_1DSize; x++) {

			uint32_t i = z * ChunkConstants::Dimension_1DSize + x;

			const double globalX = chunkOffset.x + static_cast<double>(x);
			const double globalZ = chunkOffset.z + static_cast<double>(z);

			double frequency = 1.0, amplitude = 1.0;

			double noiseHeight = 0.0;
			double continentalness = 0.0;


			for (size_t octave = 0; octave < octaves; octave++) {
				double sampleX = (globalX / scale) * frequency;
				double sampleZ = (globalZ / scale) * frequency;

				continentalness += perlin.noise2D((globalX / scale) * 0.1, (globalZ / scale) * 0.1);
				noiseHeight	+= std::pow(1.0 - perlin.noise2D_01(sampleX, sampleZ), 4) * amplitude;

				amplitude *= persistence;
				frequency *= lacunuarity;
			}

			uint32_t height = static_cast<uint32_t>(noiseHeight * getMaxHeight(continentalness, maxHeight));

			if (height > maxHeight) 
				height = maxHeight;

			heightmap[i] = height;
		}
	}
	return heightmap;
}

World::World(ShaderProgram worldShader, Texture textureAtlas)
	: 
	_pointLights(
		{ 
			{}, 
			{ Shapes::base_cube_vertices, Attributes_Details::voxelFloatAttributes, Shapes::cube_indices }, 
			{ "light_vertex_shader.glsl", "light_fragment_shader.glsl" } 
		}
	), 
	_skybox(
		{ 
			{ Shapes::base_cube_vertices, Attributes_Details::voxelFloatAttributes, Shapes::skybox_indices },
			{ "skybox_vertex_shader.glsl", "skybox_fragment_shader.glsl" } 
		}
	),
	_worldShader(worldShader), _wireframeShader("wireframe_vertex_shader.glsl", "wireframe_fragment_shader.glsl"), _textureAtlas(textureAtlas) 
{
	_worldShader.setUniform1i("myTextures", _textureAtlas._textureID);
}

void World::generateChunks(int gridSize, int verticalSize)
{
	const size_t numChunks = (gridSize * gridSize * verticalSize);
	_worldChunks.resize(numChunks);
	_chunkMeshes.resize(numChunks);

	const int horizontalCenter = gridSize / 2; // 3 = 1; 5 = 2; 7 = 3;
	const int verticalCenter = verticalSize / 2;
	const int maxTerrainHeight = ChunkConstants::Dimension_1DSize * verticalSize;

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

				float_VEC chunkOffset = {
					static_cast<float>((chunkX - horizontalCenter) * static_cast<int>(ChunkConstants::Dimension_1DSize)),
					0.0f,
					static_cast<float>((chunkZ - horizontalCenter) * static_cast<int>(ChunkConstants::Dimension_1DSize))
				};

				auto heightmap = sampleHeightmap(chunkOffset, maxTerrainHeight);

				for (int chunkY = 0; chunkY < verticalSize; chunkY++) {
					size_t chunkIndex = chunkY * (gridSize * gridSize) + i;
					_worldChunks[chunkIndex].generate(heightmap);
					for (const auto& neighbor : neighborOffsets) {

						auto neighborX = chunkX + neighbor.second.x;
						auto neighborY = chunkY + neighbor.second.y;
						auto neighborZ = chunkZ + neighbor.second.z;

						if (neighborX >= 0 && neighborX < gridSize &&
							neighborY >= 0 && neighborY < gridSize &&
							neighborZ >= 0 && neighborZ < gridSize)

						{
							size_t neighborIndex = neighborY * (gridSize * gridSize) + neighborZ * gridSize + neighborX;
							_worldChunks[chunkIndex].neighborChunks.neighbors[static_cast<int>(neighbor.first)] = &_worldChunks[neighborIndex];
						}
					}
					std::transform(heightmap.begin(), heightmap.end(), heightmap.begin(), [&](uint32_t heightValue) {
						return heightValue -= (heightValue > ChunkConstants::Dimension_1DSize) ? ChunkConstants::Dimension_1DSize : heightValue;
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

				meshFutures[i] = chunkThreadPool.enqueueTask([&, i, chunkX, chunkY, chunkZ]() -> void { // Capture by value

					float_VEC chunkOffset = {
						static_cast<float>((chunkX - horizontalCenter)	* static_cast<int>(ChunkConstants::Dimension_1DSize)),
						static_cast<float>((chunkY - verticalCenter)	* static_cast<int>(ChunkConstants::Dimension_1DSize)),
						static_cast<float>((chunkZ - horizontalCenter)	* static_cast<int>(ChunkConstants::Dimension_1DSize))
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

	_worldBuffers = BufferObjects(
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

void World::render(const Camera& camera, const Frustum& cameraFrustum, bool wireframeMode) {
	struct VP {
		glm::mat4
			view = glm::mat4(1.0f),
			projection = glm::mat4(1.0f);
	} vp_World;

	vp_World.projection = glm::perspective(glm::radians(45.0f), (float)Constants::WINDOW_WIDTH / (float)Constants::WINDOW_HEIGHT, 0.1f, 100.0f);
	vp_World.view = camera.updateCameraView();

	static GLuint vpUniformBuffer;
	static glm::mat4* vpPersistentPtr = nullptr;
	static bool isuboVP_initialized = false;

	if (!isuboVP_initialized) {
		glGenBuffers(1, &vpUniformBuffer);
		glBindBuffer(GL_UNIFORM_BUFFER, vpUniformBuffer);
		glBufferStorage(GL_UNIFORM_BUFFER, sizeof(VP), nullptr, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
		glBindBufferBase(GL_UNIFORM_BUFFER, 1, vpUniformBuffer);

		vpPersistentPtr = 
			static_cast<glm::mat4*>(
				glMapBufferRange(
						GL_UNIFORM_BUFFER, 
						0, 
						sizeof(VP),
						GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT
				)
			);
		if (!vpPersistentPtr) throw std::runtime_error("Unable to initialize vpPersistentPtr!");

		isuboVP_initialized = true;
	}

	vpPersistentPtr[0] = vp_World.projection;
	vpPersistentPtr[1] = vp_World.view;

	if (wireframeMode) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		_wireframeShader.useShaderProgram();
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		_worldShader.useShaderProgram();
		_worldShader.setUniformVec3("cameraPos", camera.getVector(CameraVectors::POS));

		setDirectionalLightUniform();
		setPointLightsUniform();
	}
	Texture_Methods::activateTexture(_textureAtlas._textureID, GL_TEXTURE0);

	GLuint transformBlockIndex = glGetUniformBlockIndex(_worldShader._shaderProgram, "ModelMatrices");
	GLuint vpBlockIndex = glGetUniformBlockIndex(_worldShader._shaderProgram, "VP_Matrices");
	glBindBufferBase(GL_UNIFORM_BUFFER, transformBlockIndex, _indirect.modelUniformBuffer);
	glBindBufferBase(GL_UNIFORM_BUFFER, vpBlockIndex, vpUniformBuffer);

	const size_t numChunks = _chunkMeshes.size();
	for (size_t i = 0; i < numChunks; i++) {
		AABB chunkAABB = _chunkMeshes[i].getBoundingBox();
		_indirect.indirectBufferPersistentPtr[i].instanceCount = (chunkAABB.isOutsideFrustum(cameraFrustum)) ? 0 : 1;
	}

	glBindVertexArray(_worldBuffers.VAO);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, _indirect.indirectBuffer);
	glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr, _chunkMeshes.size(), 0);
	glBindVertexArray(0);

	_pointLights.pointLightShader.useShaderProgram();
	vpBlockIndex = glGetUniformBlockIndex(_pointLights.pointLightShader._shaderProgram, "VP_Matrices");
	glBindBufferBase(GL_UNIFORM_BUFFER, vpBlockIndex, vpUniformBuffer);
	
	glm::mat4 lightSourceModel = glm::translate(glm::mat4(1.0f), glm::vec3(19.0f));
	_pointLights.pointLightShader.setUniformMat4("model", lightSourceModel);

	glBindVertexArray(_pointLights.pointLightBuffers.VAO);
	glDrawElements(GL_TRIANGLES, Shape_Indices::Cube, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glDepthFunc(GL_LEQUAL);
	_skybox.skyboxShader.useShaderProgram();
	_skybox.skyboxShader.setUniformMat4("view", glm::mat4(glm::mat3(vp_World.view)));
	_skybox.skyboxShader.setUniformMat4("projection", vp_World.projection);
	glBindVertexArray(_skybox.skyboxBuffers.VAO);
	glDrawElements(GL_TRIANGLES, Shape_Indices::Cube, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS);
}

ChunkGenerationThread::ChunkGenerationThread(size_t numThreads) : _stop(false)
{
	for (size_t i = 0; i < numThreads; i++) {
		_workers.emplace_back([this]() {
			while (true) {
				std::function<void()> task = nullptr;

				{
					std::unique_lock<std::mutex> lock(_queueMutex);
					_condition.wait(lock, [this]() {
						return _stop || !tasks.empty();
						});

					if (_stop && tasks.empty())
						return;

					task = std::move(tasks.front());
					tasks.pop();

				}
				task();
			}
			});
	}
	
}

std::future<void> ChunkGenerationThread::enqueueTask(std::function<void()> task) {
	auto promise = std::make_shared<std::promise<void>>();
	auto future = promise->get_future();
	{
		std::unique_lock<std::mutex> lock(_queueMutex);
		tasks.emplace([task = std::move(task), promise]() {
			try {
				task();
				promise->set_value();
			}
			catch (...) {
				promise->set_exception(std::current_exception());
			}
			});
	}
	_condition.notify_one();
	return future;
}