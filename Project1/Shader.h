#ifndef SHADER_H
#define SHADER_H

#include "stdafx.h"
#include "FileUtils.h"

namespace Shader_Methods {
	void handleFailedShaderCompilation(const uint32_t& shader, const GLenum& type);
	void handleFailedShaderProgramCompilation(const uint32_t& shaderProgram);

	uint32_t compileShader(const char* source, const GLenum& type);
	void useShaderProgram(const uint32_t& shaderProgram);

	void setUniform1i(const uint32_t& shaderProgram, const char* name, int value);
	void setUniform1f(const uint32_t& shaderProgram, const char* name, float value);
	void setUniformVec3(const uint32_t& shaderProgram, const char* name, const glm::vec3& vector);
	void setUniformMat4(const uint32_t& shaderProgram, const char* name, const glm::mat4& matrix);

}

struct ShaderProgram {
	uint32_t _shaderProgram = 0;

	ShaderProgram(const char* vertexPath = nullptr, const char* fragmentPath = nullptr);
};

#endif // SHADER_H