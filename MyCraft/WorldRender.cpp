#include "World.h"

void World::setDirectionalLightUniform() const {
	std::array<UniformsVEC3, 4> directionalUniformsVEC3 = { 
		{
			{ "directional_light.direction",	glm::vec3(-0.2f, -1.0f, -0.3f)	},
			{ "directional_light.ambient",		glm::vec3(0.2f, 0.2f, 0.2f)		},
			{ "directional_light.diffuse",		glm::vec3(0.75f, 0.75f, 0.75f)	},
			{ "directional_light.specular",		glm::vec3(1.0f, 1.0f, 1.0f)		}
		}
	};

	for (const auto& dir : directionalUniformsVEC3)
		_deferred.deferredShader.setUniformVec3(dir.first, dir.second);
}

void World::initializeWorldVPUniformBuffer(uint32_t& vpUniformBuffer, glm::mat4*& vpPersistentPtr) const {
	glGenBuffers(1, &vpUniformBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, vpUniformBuffer);
	glBufferStorage(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), nullptr, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, vpUniformBuffer);

	vpPersistentPtr =
		static_cast<glm::mat4*>(
			glMapBufferRange(
				GL_UNIFORM_BUFFER,
				0,
				2 * sizeof(glm::mat4),
				GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT
			)
			);
	if (!vpPersistentPtr) throw std::runtime_error("Unable to initialize vpPersistentPtr!");
}

void World::updateCameraChunkPos(const float_VEC& cameraPos) {
	_cameraChunkPos.x = (cameraPos.x > 0) ? static_cast<int>(cameraPos.x / 16.0f) : static_cast<int>(cameraPos.x / 16.0f) - 1;
	_cameraChunkPos.y = (cameraPos.x > 0) ? static_cast<int>(cameraPos.y / 16.0f) : static_cast<int>(cameraPos.y / 16.0f) - 1;
	_cameraChunkPos.z = (cameraPos.x > 0) ? static_cast<int>(cameraPos.z / 16.0f) : static_cast<int>(cameraPos.z / 16.0f) - 1;

	_cameraChunkPos.x *= ChunkConstants::Dimension_1DSize;
	_cameraChunkPos.y *= ChunkConstants::Dimension_1DSize;
	_cameraChunkPos.z *= ChunkConstants::Dimension_1DSize;
}

void World::renderChunks(const uint32_t& vpUniformBuffer, const float_VEC& cameraPos, const Frustum& cameraFrustum)
{
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D_ARRAY, _textureAtlas.textureID);
	const uint32_t& deferredGShader = _deferred.geometryShader.shaderProgram;

	_deferred.geometryShader.useShaderProgram();
	GLuint transformBlockIndex = glGetUniformBlockIndex(deferredGShader, "ModelMatrices");
	GLuint vpBlockIndex = glGetUniformBlockIndex(deferredGShader, "VPMatrices");
	glBindBufferBase(GL_UNIFORM_BUFFER, transformBlockIndex, _indirect.modelUniformBuffer);
	glBindBufferBase(GL_UNIFORM_BUFFER, vpBlockIndex, vpUniformBuffer);

	// ---FRUSTUM CULLING---
	updateCameraChunkPos(cameraPos);
	for (size_t i = 0; i < _chunkMeshes.size(); i++) {
		bool outsideFrustum = (_chunkMeshes[i].pos.z < _cameraChunkPos.z) ? true : _chunkMeshes[i].getBoundingBox().isOutsideFrustum(cameraFrustum);
		_indirect.drawCommands[i].instanceCount = (outsideFrustum) ? 0 : 1;
	}

	// ---DRAW CHUNKS MULTI-INDIRECT---
	glBindVertexArray(_worldBuffers.VAO);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, _indirect.indirectBuffer);
	//glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr, _chunkMeshes.size(), 0);
	glBindVertexArray(0);
	//glActiveTexture(0);
}

void World::renderQuad() const {
	static GLuint quadVAO = 0;
	static GLuint quadVBO, quadEBO;
	if (quadVAO == 0)
	{
		constexpr float quadVertices[] = {
			// positions        // texture Coords
			-1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
			-1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
			 1.0f,  1.0f, 0.0f,  1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
		};

		constexpr uint32_t quadIndices[] = {
			0, 2, 3,
			0, 1, 2
		};

		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glGenBuffers(1, &quadEBO);

		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), &quadIndices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glBindVertexArray(0);
	}
	glBindVertexArray(quadVAO);
	glDrawElements(GL_TRIANGLES, Shape_Indices::Rectangle, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void World::renderSkybox(const glm::mat4& view, const glm::mat4& projection) const {
	glDepthFunc(GL_LEQUAL);
		_skybox.shader.useShaderProgram();
		_skybox.shader.setUniformMat4("view", glm::mat4(glm::mat3(view)));
		_skybox.shader.setUniformMat4("projection", projection);
		glBindVertexArray(_skybox.buffers.VAO);
		glDrawElements(GL_TRIANGLES, Shape_Indices::Cube, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	glDepthFunc(GL_LESS);
}

World::World()
	:
	_deferred(
		DeferredRendering{
			DeferredBufferObjects{},
			ShaderProgram{"g_vertex_shader.glsl", "g_fragment_shader.glsl" },
			ShaderProgram{ "deferred_vertex_shader.glsl", "deferred_fragment_shader.glsl" },
		}),
		_skybox(
			Skybox{
				DrawableBufferObjects{ Shapes::base_cube_vertices, Attributes_Details::voxelFloatAttributes, Shapes::skybox_indices },
				ShaderProgram{ "skybox_vertex_shader.glsl", "skybox_fragment_shader.glsl" }
			}),
	_wireframeShader("wireframe_vertex_shader.glsl", "wireframe_fragment_shader.glsl"),
	_textureAtlas("TextureAtlas.jpg")
{
	_deferred.deferredShader.useShaderProgram();
	_deferred.deferredShader.setUniform1i("gPosition", 0);
	_deferred.deferredShader.setUniform1i("gNormal", 1);
	_deferred.deferredShader.setUniform1i("gColorSpecular", 2);
}

void World::render(const Camera& camera, const Frustum& cameraFrustum, bool wireframeMode) {
	//---GEOMETRY PASS---
	glBindFramebuffer(GL_FRAMEBUFFER, _deferred.buffers.gBuffer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// ---UPDATE VIEW & PROJECTION MATRICES USING PESISTENT MAPPING---
		const glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)Constants::WINDOW_WIDTH / (float)Constants::WINDOW_HEIGHT, 0.1f, 100.0f);
		const glm::mat4 view = camera.updateCameraView();
		static GLuint vpUniformBuffer;
		static glm::mat4* vpPersistentPtr = nullptr;
		if (!vpPersistentPtr)
			initializeWorldVPUniformBuffer(vpUniformBuffer, vpPersistentPtr);
		vpPersistentPtr[0] = projection;
		vpPersistentPtr[1] = view;

		// ---UPLOAD MVP MATRICES TO GEOMETRY SHADER---
		_deferred.geometryShader.useShaderProgram();
		GLuint transformBlockIndex = glGetUniformBlockIndex(_deferred.geometryShader.shaderProgram, "ModelMatrices");
		GLuint vpBlockIndex = glGetUniformBlockIndex(_deferred.geometryShader.shaderProgram, "VPMatrices");
		glBindBufferBase(GL_UNIFORM_BUFFER, transformBlockIndex, _indirect.modelUniformBuffer);
		glBindBufferBase(GL_UNIFORM_BUFFER, vpBlockIndex, vpUniformBuffer);

		// ---FRUSTUM CULLING---
		updateCameraChunkPos(camera.getVector(CameraVectors::POS));
		for (size_t i = 0; i < _chunkMeshes.size(); i++) {
			bool outsideFrustum = (_chunkMeshes[i].pos.z < _cameraChunkPos.z) ? true : _chunkMeshes[i].getBoundingBox().isOutsideFrustum(cameraFrustum);
			_indirect.drawCommands[i].instanceCount = (outsideFrustum) ? 0 : 1;
		}

		// ---DRAW CHUNKS MULTI-INDIRECT---
		glBindVertexArray(_worldBuffers.VAO);
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, _indirect.indirectBuffer);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr, _chunkMeshes.size(), 0);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glBindVertexArray(0);

	//---LIGHTING PASS---
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		_deferred.deferredShader.useShaderProgram();
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, _deferred.buffers.gPosition);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, _deferred.buffers.gNormal);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, _deferred.buffers.gColorSpecular);
			setDirectionalLightUniform();
			_deferred.deferredShader.setUniformVec3("cameraPos", camera.getVector(CameraVectors::POS));
		renderQuad();

	glBindFramebuffer(GL_READ_FRAMEBUFFER, _deferred.buffers.gBuffer);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glBlitFramebuffer(
			0, 0, Constants::WINDOW_WIDTH, Constants::WINDOW_HEIGHT, 
			0, 0, Constants::WINDOW_WIDTH, Constants::WINDOW_HEIGHT, 
			GL_DEPTH_BUFFER_BIT, GL_NEAREST
		);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	renderSkybox(view, projection);

}
