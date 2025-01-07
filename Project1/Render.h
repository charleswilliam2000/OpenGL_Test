#ifndef RENDER_H
#define RENDER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>

#include "Shape.h"
#include "Texture.h"
#include "Shader.h"
#include "Camera.h"

struct Drawable {

	uint32_t VAO{};
	uint32_t texture{};
	uint32_t indices{};
	glm::vec3 coordinate{};

	Drawable(uint32_t VAO, uint32_t texture, uint32_t indices, glm::vec3 coordinate) : VAO(VAO), texture(texture), indices(indices), coordinate(coordinate) {}

};
using RenderData = std::vector<Drawable>;

class Renderer {
private:
	RenderData _data{};
	uint32_t _shaderProgram{};

	void updateUniforms(const uint32_t& shaderProgram, const glm::mat4& projectionMat, const glm::mat4& modelMat) const;
	void have_3D_Object_Rotate(const uint32_t& shaderProgram) const;
	void draw(const uint32_t& VAO, const uint32_t& indices) const;
public:
	void addShaderProgram(uint32_t shaderProgram);
	void terminateShaderProgram();
	void addRenderData(uint32_t VAO, uint32_t textureID, uint32_t indices, glm::vec3 coordinate);
	void render(const glm::mat4& cameraView) const;
};

#endif // RENDER_H