#include "World.h"

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
			{ "point_light[0].position",	glm::vec3(23.0f, 23.0f, 23.0f)	},
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

void World::renderChunks(const uint32_t& vpUniformBuffer, const Frustum& cameraFrustum) const
{
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
}

void World::renderPointLight(const uint32_t& vpUniformBuffer) const
{
	_pointLights.pointLightShader.useShaderProgram();
	uint32_t pointlight_vpBlockIndex = glGetUniformBlockIndex(_pointLights.pointLightShader._shaderProgram, "VP_Matrices");
	glBindBufferBase(GL_UNIFORM_BUFFER, pointlight_vpBlockIndex, vpUniformBuffer);

	glm::mat4 lightSourceModel = glm::translate(glm::mat4(1.0f), glm::vec3(19.0f));
	_pointLights.pointLightShader.setUniformMat4("model", lightSourceModel);

	glBindVertexArray(_pointLights.pointLightBuffers.VAO);
	glDrawElements(GL_TRIANGLES, Shape_Indices::Cube, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void World::renderSkybox(const glm::mat4& view, const glm::mat4& projection) const {
	glDepthFunc(GL_LEQUAL);
	_skybox.skyboxShader.useShaderProgram();
	_skybox.skyboxShader.setUniformMat4("view", glm::mat4(glm::mat3(view)));
	_skybox.skyboxShader.setUniformMat4("projection", projection);
	glBindVertexArray(_skybox.skyboxBuffers.VAO);
	glDrawElements(GL_TRIANGLES, Shape_Indices::Cube, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS);
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

void World::render(const Camera& camera, const Frustum& cameraFrustum, bool wireframeMode) {

	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)Constants::WINDOW_WIDTH / (float)Constants::WINDOW_HEIGHT, 0.1f, 100.0f);
	glm::mat4 view = camera.updateCameraView();

	static GLuint vpUniformBuffer;
	static glm::mat4* vpPersistentPtr = nullptr;

	if (!vpPersistentPtr) 
		initializeWorldVPUniformBuffer(vpUniformBuffer, vpPersistentPtr);

	vpPersistentPtr[0] = projection;
	vpPersistentPtr[1] = view;

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

	renderChunks(vpUniformBuffer, cameraFrustum);
	renderPointLight(vpUniformBuffer);
	renderSkybox(view, projection);
}
