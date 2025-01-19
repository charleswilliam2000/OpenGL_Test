#include "Render.h"

void Renderer::addObjectShader(uint32_t objectShader) 
{
	_objectShaders.push_back(objectShader);
}

void Renderer::addLightSourceShader(uint32_t lightSourceShader)
{
	_lightSourceShader = lightSourceShader;
}

void Renderer::addWireframeShader(uint32_t wireframeShader)
{
	_wireframeShader = wireframeShader;
}

void Renderer::terminateShaderPrograms() {
	for (auto& shaderProgram : _objectShaders)
		glDeleteProgram(shaderProgram);
	glDeleteProgram(_lightSourceShader);
}

void Renderer::addObjectData(uint32_t VAO, uint32_t texture, uint32_t indices, float_VEC pos) {
	_objectsData.emplace_back(VAO, texture, indices, pos);
}

void Renderer::addLightSourceData(uint32_t VAO, uint32_t texture, uint32_t indices, float_VEC pos)
{
	_lightSource = { VAO, texture, indices, pos };
}

void Renderer::render(const glm::vec3& cameraPos, const glm::mat4& cameraView, bool wireframeMode) const {
	uint32_t lastTexture		= 0;
	glm::mat4 objectModel		= glm::mat4(1.0f);
	glm::mat4 projection		= glm::perspective(glm::radians(45.0f), (float)Constants::WINDOW_WIDTH / (float)Constants::WINDOW_HEIGHT, 0.1f, 100.0f);
	glm::vec3 light_coord		= glm::vec3(_lightSource.coordinate.x, _lightSource.coordinate.y, _lightSource.coordinate.z);
	glm::mat4 lightSourceModel	= glm::translate(glm::mat4(1.0f), light_coord);

	if (wireframeMode) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		Shader_Methods::useShaderProgram(_wireframeShader);
		Shader_Methods::setUniformVec3(_wireframeShader, "wireframeColors", glm::vec3(1.0f, 1.0f, 1.0f));
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		Shader_Methods::useShaderProgram(_objectShaders[0]);
		Shader_Methods::setUniformVec3(_objectShaders[0], "cameraPos", cameraPos);
		Shader_Methods::setUniformVec3(_objectShaders[0], "directional_light.direction", glm::vec3(- 0.2f, -1.0f, -0.3f));
		Shader_Methods::setUniformVec3(_objectShaders[0], "directional_light.ambient",	 glm::vec3(0.2f, 0.2f, 0.2f));
		Shader_Methods::setUniformVec3(_objectShaders[0], "directional_light.diffuse",	 glm::vec3(0.5f, 0.5f, 0.5f));
		Shader_Methods::setUniformVec3(_objectShaders[0], "directional_light.specular",  glm::vec3(1.0f, 1.0f, 1.0f));

		Shader_Methods::setUniformVec3(_objectShaders[0], "point_light[0].position", light_coord);

		Shader_Methods::setUniformVec3(_objectShaders[0], "point_light[0].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
		Shader_Methods::setUniformVec3(_objectShaders[0], "point_light[0].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
		Shader_Methods::setUniformVec3(_objectShaders[0], "point_light[0].specular", glm::vec3(1.0f, 1.0f, 1.0f));

		Shader_Methods::setUniform1f(_objectShaders[0], "point_light[0].constant", 0.5f);
		Shader_Methods::setUniform1f(_objectShaders[0], "point_light[0].linear", 0.09f);
		Shader_Methods::setUniform1f(_objectShaders[0], "point_light[0].quadratic", 0.032f);

	}

	Shader_Methods::setUniformMat4(_objectShaders[0], "projection", projection);
	Shader_Methods::setUniformMat4(_objectShaders[0], "view", cameraView);

	for (const auto& object : _objectsData) {
		if (object.texture != lastTexture) {
			Texture_Methods::activateTexture(object.texture, GL_TEXTURE0);
			lastTexture = object.texture;
		}
		Shader_Methods::setUniformMat4(_objectShaders[0], "model", glm::translate(objectModel, glm::vec3(object.coordinate.x, object.coordinate.y, object.coordinate.z)));

		draw(object.VAO, object.indices);
	}

	Shader_Methods::useShaderProgram(_lightSourceShader);
	Shader_Methods::setUniformMat4(_lightSourceShader, "projection", projection);
	Shader_Methods::setUniformMat4(_lightSourceShader, "view", cameraView);
	Shader_Methods::setUniformMat4(_lightSourceShader, "model", lightSourceModel);

	draw(_lightSource.VAO, _lightSource.indices);
}

void Renderer::have_3D_Object_Rotate(const uint32_t& shaderProgram) const {
	glm::mat4 projectionMat = glm::perspective(glm::radians(45.0f),
		(float)Constants::WINDOW_WIDTH / (float)Constants::WINDOW_HEIGHT,
		0.1f, 100.0f);
	glm::mat4 modelMat = glm::rotate(glm::mat4(1.0f), (float)glfwGetTime(), glm::vec3(0.5f, 1.0f, 0.0f));


	updateUniforms(shaderProgram, projectionMat, modelMat);
}

void Renderer::updateUniforms(const uint32_t& shaderProgram, const glm::mat4& projectionMat, const glm::mat4& modelMat) const {
	Shader_Methods::setUniformMat4(shaderProgram, "model", modelMat);
	Shader_Methods::setUniformMat4(shaderProgram, "projection", projectionMat);
}


void Renderer::draw(const uint32_t& VAO, const uint32_t& indices) const {
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices, GL_UNSIGNED_INT, 0);
}
