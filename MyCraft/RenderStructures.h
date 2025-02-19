#ifndef RENDER_STRUCTURES_H
#define RENDER_STRUCTURES_H

#include <glad/glad.h>
#include <glfw/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

#include "Constants.h"
#include "Camera.h"
#include "Buffers.h"

class SSAO {
private:
	UniformBufferObjects _ssaoKernelsUBO;
public:
	GLuint ssaoFBO = 0, ssaoColorText = 0;
	GLuint ssaoBlurFBO = 0, ssaoBlurText = 0;
	GLuint ssaoNoise = 0;

	SSAO() {}

	void generateSSAO(int windowWidth, int windowHeight);
	void bindToShader(GLuint shaderProgram);

	SSAO(const SSAO&) = delete;
	SSAO& operator=(const SSAO&) = delete;

	~SSAO() noexcept {
		glDeleteFramebuffers(1, &ssaoFBO);
		glDeleteFramebuffers(1, &ssaoBlurFBO);
		glDeleteTextures(1, &ssaoColorText);
		glDeleteTextures(1, &ssaoBlurText);
		glDeleteTextures(1, &ssaoNoise);
	}
};

class ModelMatrices {
private:
	std::vector<glm::mat4> _modelMatrices;
	UniformBufferObjects _modelUBO;
public:
	void addMatrix(const glm::mat4& modelMatrix);
	void createUBO(STORAGE_TYPE type, GLuint bindingPoint);
	void bindToShader(GLuint shaderProgram);
};

class ViewProjectionMatrices {
private:
	glm::mat4 _projection{};
	glm::mat4 _view{};

	glm::mat4 _inverseProjection{};
	glm::mat4 _inverseView{};

	UniformBufferObjects _viewProjectionUBO;
public:
	ViewProjectionMatrices() {}
	void createUBO(STORAGE_TYPE type, GLuint bindingPoint);
	void bindToShader(GLuint shaderProgram);
	void updateViewProjection(const Camera& camera, const GLuint& shaderProgram);
};

#endif // RENDER_STRUCTURES_H