#ifndef SHADER_H
#define SHADER_H

#include "stdafx.h"
#include "FileUtils.h"

namespace Shader_Methods {
	void handleFailedShaderCompilation(const uint32_t& shader, const GLenum& type);
	void handleFailedShaderProgramCompilation(const uint32_t& shaderProgram);

	uint32_t compileShader(const char* source, const GLenum& type);
}

struct ShaderProgram {
	uint32_t _shaderProgram = 0;

	ShaderProgram(const char* vertexPath = nullptr, const char* fragmentPath = nullptr);

	void useShaderProgram() const;
	void setUniform1i(const char* name, int value) const;
	void setUniform1f(const char* name, float value) const;
	void setUniformVec3(const char* name, const glm::vec3& vector) const;
	void setUniformMat4(const char* name, const glm::mat4& matrix) const;
};

#endif // SHADER_H