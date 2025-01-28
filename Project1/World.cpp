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
			{ "directional_light.diffuse",		glm::vec3(0.5f, 0.5f, 0.5f)		},
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

void World::generateTerrain(const siv::PerlinNoise& perlin, WorldChunk::Blocks& chunkTerrain, const float_VEC& chunkOffset) const
{
	const double frequencyX = 0.001 + chunkOffset.x * 0.00001;
	const double frequencyZ = 0.001 + chunkOffset.z * 0.00001;

	const double chunkOffsetX = static_cast<double>(chunkOffset.x) * 2;
	const double chunkOffsetZ = static_cast<double>(chunkOffset.z) * 2;

	for (uint8_t z = 0; z < Chunk_Constants::Dimension_1DSize; z++) {
		for (uint8_t x = 0; x < Chunk_Constants::Dimension_1DSize; x++) {
			double globalX = (chunkOffsetX + static_cast<double>(x)) * frequencyX;
			double globalZ = (chunkOffsetZ + static_cast<double>(z)) * frequencyZ;

			double perlinVal = std::pow(perlin.octave2D_01(globalX, globalZ, 8, 0.5), 2);
			int height = static_cast<int>(perlinVal * static_cast<double>(Chunk_Constants::Dimension_1DSize));
			_ASSERT_EXPR(height < Chunk_Constants::Dimension_1DSize, "\nHeight higher than chunk's dimension");

			for (uint8_t y = 0; y < height; y++) {
				if (y < height - 1)
					chunkTerrain[y][z].setID(BLOCK_ID::DIRT, x);
				else if (y == height - 1)
					chunkTerrain[y][z].setID(BLOCK_ID::GRASS, x);
			}
		}
	}
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

void World::generateChunks(size_t gridSize)
{
	const siv::PerlinNoise::seed_type seed = 123456u;
	const siv::PerlinNoise perlin{ seed };

	_worldChunks.resize(gridSize * gridSize);
	_chunkMeshes.resize(gridSize * gridSize);

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

				generateTerrain(perlin, _worldChunks[i].blocks, chunkOffset);

				for (const auto& neighbor : neighborOffsets) {
					int neighborX = static_cast<int>(x) + neighbor.second.x;
					int neighborZ = static_cast<int>(z) + neighbor.second.z;

					if (neighborX >= 0 && neighborX < static_cast<int>(gridSize) &&
						neighborZ >= 0 && neighborZ < static_cast<int>(gridSize)) {
						size_t neighborIndex = neighborZ * gridSize + neighborX;
						_worldChunks[i].neighborChunks.neighbors[static_cast<int>(neighbor.first)] = &_worldChunks[neighborIndex];
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

				threadResults.emplace_back(i, _chunkMeshes[i].generate(chunkOffset, _worldChunks[i]));
			}

			return threadResults;
			}));
	}

	uint32_t vetexOffset = 0;
	std::vector<Vertex> worldVertex; std::vector<uint32_t> worldIndex;

	for (auto& future : meshFuture) {
		const auto& threadResults = future.get();
		for (const auto& [chunkIndex, chunkData] : threadResults) {

			auto numVertices	= static_cast<uint32_t>(chunkData.chunk_vertices.size());
			auto numIndices		= static_cast<uint32_t>(chunkData.chunk_indices.size());

			_worldVerticesIndices.first += numVertices; _worldVerticesIndices.second += numIndices;
			_chunkMeshes[chunkIndex].numVerticesIndices = std::make_pair(numVertices, numIndices);

			worldVertex.insert(worldVertex.end(), chunkData.chunk_vertices.begin(), chunkData.chunk_vertices.end());
			std::transform(chunkData.chunk_indices.begin(), chunkData.chunk_indices.end(), std::back_inserter(worldIndex), [&](uint32_t index) {
				return index + vetexOffset;
				});

			_indirect.modelMatrices.push_back(
				glm::translate(
					glm::mat4(1.0f),
					glm::vec3(_chunkMeshes[chunkIndex].pos)
				)
			);
			vetexOffset += numVertices;
		}
	}

	glGenBuffers(1, &_indirect.modelUniformBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, _indirect.modelUniformBuffer);
	glBufferData(GL_UNIFORM_BUFFER, _indirect.modelMatrices.size() * sizeof(glm::mat4), _indirect.modelMatrices.data(), GL_STATIC_DRAW);

	_worldBuffers = BufferObjects(
		worldVertex,
		Attributes_Details::voxelPackedAttributes,
		worldIndex
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

		vpPersistentPtr = static_cast<glm::mat4*>(
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
