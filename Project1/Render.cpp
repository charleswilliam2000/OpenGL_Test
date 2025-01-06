#include "Render.h"

void Renderer::addShaderProgram(uint32_t shaderProgram) {
	_shaderProgram = shaderProgram;
}

void Renderer::terminateShaderProgram() {
	glDeleteProgram(_shaderProgram);
}

void Renderer::addRenderData(uint32_t VAO, uint32_t textureID, uint32_t indices) {
	_data.emplace_back(VAO, textureID, indices);
}

void Renderer::render() const {
	uint32_t lastTexture = 0; 

	Shader_Methods::useShaderProgram(_shaderProgram);

	glm::mat4 view = glm::lookAt(_camera.cameraPos, _camera.cameraPos + _camera.cameraFront, _camera.cameraUp);
	Shader_Methods::setUniformMat4(_shaderProgram, "view", view);

	for (const auto& drawable : _data) {
		glBindVertexArray(drawable.VAO);
		if (drawable.texture != lastTexture) {
			Texture_Methods::activateTexture(drawable.texture, GL_TEXTURE0);
			lastTexture = drawable.texture;
		}
		have_3D_Object_Rotate(_shaderProgram);
		draw(drawable.VAO, drawable.indices);
	}
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
