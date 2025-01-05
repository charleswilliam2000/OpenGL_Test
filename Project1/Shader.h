#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include "FileUtils.h"

namespace Shader_Methods {
	void handleFailedShaderCompilation(const uint32_t& shader, const GLenum& type);
	void handleFailedShaderProgramCompilation(const uint32_t& shaderProgram);

	uint32_t compileShader(const char* source, const GLenum& type);
	void useShaderProgram(const uint32_t& shaderProgram);
	void setUniformMat4(const uint32_t& shaderProgram, const char* name, const glm::mat4& matrix);
}

struct ShaderProgram {
	uint32_t _shaderProgram = 0;

	ShaderProgram(const char* vertexPath = nullptr, const char* fragmentPath = nullptr);
	~ShaderProgram() noexcept {
		std::cout << "\nShaderProgram deleted!";
	}
};

#endif // SHADER_H