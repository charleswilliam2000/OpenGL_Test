#include "Render.h"

void Renderer::addShaderProgram(uint32_t shaderProgram) {
	_shaderProgram = shaderProgram;
}

void Renderer::addRenderData(uint32_t VAO, uint32_t textureID, uint32_t indices) {
	_data.emplace_back(VAO, textureID, indices);
}

void Renderer::render() const {
	uint32_t lastTexture = 0; 

	Shader_Methods::useShaderProgram(_shaderProgram);
	for (const auto& drawable : _data) {
		if (drawable.texture != lastTexture) {
			Texture_Methods::activateTexture(drawable.texture, GL_TEXTURE0);
			lastTexture = drawable.texture;
		}
		have_3D_Object_Rotate(_shaderProgram);
		glBindVertexArray(drawable.VAO);
		draw(drawable.VAO, drawable.indices);
	}
}

void Renderer::have_3D_Object_Rotate(const uint32_t& shaderProgram) const {
	glm::mat4 viewMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
	glm::mat4 projectionMat = glm::perspective(glm::radians(45.0f),
		(float)Constants::WINDOW_WIDTH / (float)Constants::WINDOW_HEIGHT,
		0.1f, 100.0f);
	glm::mat4 modelMat = glm::rotate(glm::mat4(1.0f), (float)glfwGetTime(), glm::vec3(0.5f, 1.0f, 0.0f));


	updateUniforms(shaderProgram, viewMat, projectionMat, modelMat);
}

void Renderer::updateUniforms(const uint32_t& shaderProgram, const glm::mat4& viewMat, const glm::mat4& projectionMat, const glm::mat4& modelMat) const {
	Shader_Methods::setUniformMat4(shaderProgram, "model", modelMat);
	Shader_Methods::setUniformMat4(shaderProgram, "view", viewMat);
	Shader_Methods::setUniformMat4(shaderProgram, "projection", projectionMat);
}


void Renderer::draw(const uint32_t& VAO, const uint32_t& indices) const {
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices, GL_UNSIGNED_INT, 0);
}
