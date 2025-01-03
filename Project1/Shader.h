#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <iostream>

#include "FileUtils.h"

namespace Shader_Methods {
	void handleFailedShaderCompilation(const uint32_t& shader, const GLenum& type);
	void handleFailedShaderProgramCompilation(const uint32_t& shaderProgram);

	uint32_t compileShader(const char* source, const GLenum& type);
	void useShaderProgram(const uint32_t& shaderProgram);
}

struct ShaderProgram {
	uint32_t _shaderProgram = 0;

	ShaderProgram(const char* vertexPath = nullptr, const char* fragmentPath = nullptr);
	~ShaderProgram() noexcept {
		std::cout << "\nShaderProgram deleted!";
	}
};

#endif // SHADER_H