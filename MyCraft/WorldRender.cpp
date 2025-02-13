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
		_shaderLightingPass.setUniformVec3(dir.first, dir.second);
}

void World::updateCameraChunkPos(const float_VEC& cameraPos) {
	_cameraChunkPos.x = (cameraPos.x > 0) ? static_cast<int>(cameraPos.x / 16.0f) : static_cast<int>(cameraPos.x / 16.0f) - 1;
	_cameraChunkPos.y = (cameraPos.x > 0) ? static_cast<int>(cameraPos.y / 16.0f) : static_cast<int>(cameraPos.y / 16.0f) - 1;
	_cameraChunkPos.z = (cameraPos.x > 0) ? static_cast<int>(cameraPos.z / 16.0f) : static_cast<int>(cameraPos.z / 16.0f) - 1;

	_cameraChunkPos.x *= CONSTANTS::Dimension_1DSize;
	_cameraChunkPos.y *= CONSTANTS::Dimension_1DSize;
	_cameraChunkPos.z *= CONSTANTS::Dimension_1DSize;
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
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void World::render(const Camera& camera, const Frustum& cameraFrustum, bool wireframeMode) {
	
	// ---GEOMETRY PASS---
	glBindFramebuffer(GL_FRAMEBUFFER, _deferred.gFBO);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		_shaderGeometryPass.useShaderProgram();

		_modelsUBO.bindToShader(_shaderGeometryPass.shaderProgram, "ModelMatrices");
		const glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)CONSTANTS::WINDOW_WIDTH / (float)CONSTANTS::WINDOW_HEIGHT, 0.1f, 100.0f);
		const glm::mat4 view = camera.updateCameraView();
		_vpUBO.bindToShader(_shaderGeometryPass.shaderProgram, "VPMatrices");

		static_cast<glm::mat4*>(_vpUBO.persistentPtr)[0] = projection;
		static_cast<glm::mat4*>(_vpUBO.persistentPtr)[1] = view;

		// ---FRUSTUM CULLING---
		updateCameraChunkPos(camera.getVector(CameraVectors::POS));
		for (size_t i = 0; i < _chunkMeshes.size(); i++) {
			bool outsideFrustum = (_chunkMeshes[i].pos.z <= _cameraChunkPos.z) ? true : _chunkMeshes[i].getBoundingBox().isOutsideFrustum(cameraFrustum);
			_indirect.drawCommands[i].instanceCount = (outsideFrustum) ? 0 : 1;
		}

		// ---DRAW CHUNKS MULTI-INDIRECT---
		glEnable(GL_DEPTH_TEST);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D_ARRAY, _textureAtlas.textureID);

		glBindVertexArray(_world.vaoHandle);
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, _indirect.indirectBuffer);
		glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr, _chunkMeshes.size(), 0);
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
		glBindVertexArray(0);
		glDisable(GL_DEPTH_TEST); // Disable DEPTH TESTING for lighting pass as we directly sample the depth buffer

	// ---LIGHTING PASS---
	glBindFramebuffer(GL_FRAMEBUFFER,0);
		_shaderLightingPass.useShaderProgram();
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D_ARRAY, _deferred.gTextArray);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, _deferred.gDepthText);
		
		setDirectionalLightUniform();
		_shaderLightingPass.setUniformMat4("invViewProj", glm::inverse(projection * view));
		_shaderLightingPass.setUniformVec3("cameraPos", camera.getVector(CameraVectors::POS));
		renderQuad();  

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, 0);
}
