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

void Renderer::addObjectData(const Drawable& drawable) {
	_objectsData.push_back(drawable);
}

void Renderer::addLightSourceData(const Drawable& lightSource)
{
	_lightSource = lightSource;
}

void Renderer::render(const glm::vec3& cameraPos, const glm::mat4& cameraView, bool wireframeMode) const {
	uint32_t lastTexture		= 0; 
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
		Shader_Methods::setUniformVec3(_objectShaders[0], "lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
		Shader_Methods::setUniformVec3(_objectShaders[0], "lightPos", light_coord);
		Shader_Methods::setUniformVec3(_objectShaders[0], "cameraPos", cameraPos);
	}

	for (const auto& object : _objectsData) {
		if (object.texture != lastTexture) {
			Texture_Methods::activateTexture(object.texture, GL_TEXTURE0);
			lastTexture = object.texture;
		}
		glm::mat4 objectModel = glm::mat4(1.0f);
		
		Shader_Methods::setUniformMat4(_objectShaders[0], "projection", projection);
		Shader_Methods::setUniformMat4(_objectShaders[0], "view", cameraView);
		Shader_Methods::setUniformMat4(_objectShaders[0], "model", objectModel);

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
