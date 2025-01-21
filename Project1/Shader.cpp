#include "Shader.h"

void Shader_Methods::handleFailedShaderCompilation(const uint32_t& shader, const GLenum& type) {
    int success{};
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        throw std::runtime_error("\nUNABLE TO COMPILE SHADER");
    }
}

void Shader_Methods::handleFailedShaderProgramCompilation(const uint32_t& shaderProgram) {
    int success{};
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::SHADER_PROGRAM::COMPILATION_FAILED\n" << infoLog << std::endl;
        throw std::runtime_error("\nUNABLE TO CREATE SHADER PROGRAM");
    }
}

uint32_t Shader_Methods::compileShader(const char* source, const GLenum& type) {
    uint32_t shader{};

    shader = glCreateShader(type);

    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    handleFailedShaderCompilation(shader, type);
    return shader;
}

void ShaderProgram::useShaderProgram() const {
    glUseProgram(_shaderProgram);
}

void ShaderProgram::setUniform1i(const char* name, int value) const {
    uint32_t location = glGetUniformLocation(_shaderProgram, name);
    glUniform1i(location, value);
}

void ShaderProgram::setUniform1f(const char* name, float value) const {
    uint32_t location = glGetUniformLocation(_shaderProgram, name);
    glUniform1f(location, value);
}

void ShaderProgram::setUniformVec3(const char* name, const glm::vec3& vector) const {
    uint32_t location = glGetUniformLocation(_shaderProgram, name);
    glUniform3fv(location, 1, glm::value_ptr(vector));
}

void ShaderProgram::setUniformMat4(const char* name, const glm::mat4& matrix) const {
    uint32_t location = glGetUniformLocation(_shaderProgram, name);
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

ShaderProgram::ShaderProgram(const char* vertexPath, const char* fragmentPath) {
    if (!vertexPath && !fragmentPath)
        throw std::runtime_error("Neither vertexPath, or fragmentPath was specified");

    std::string vertexCode = File_Utils::readFile(vertexPath);
    std::string fragmentCode = File_Utils::readFile(fragmentPath);

    auto vertexShader     = Shader_Methods::compileShader(vertexCode.c_str(), GL_VERTEX_SHADER);
    auto fragmentShader   = Shader_Methods::compileShader(fragmentCode.c_str(), GL_FRAGMENT_SHADER);

    _shaderProgram = glCreateProgram();
    glAttachShader(_shaderProgram, vertexShader);
    glAttachShader(_shaderProgram, fragmentShader);
    glLinkProgram(_shaderProgram);

    Shader_Methods::handleFailedShaderProgramCompilation(_shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}
