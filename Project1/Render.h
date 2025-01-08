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

	Drawable() {};
	Drawable(uint32_t VAO, uint32_t texture, uint32_t indices, glm::vec3 coordinate) : VAO(VAO), texture(texture), indices(indices), coordinate(coordinate) {}

};

class Renderer {
	using Objects = std::vector<Drawable>;
	using ObjectShaders = std::vector<uint32_t>;
private:
	Objects _objectsData{};
	ObjectShaders _objectShaders{};

	Drawable _lightSource{};
	uint32_t _lightSourceShader{};

	void updateUniforms(const uint32_t& shaderProgram, const glm::mat4& projectionMat, const glm::mat4& modelMat) const;
	void have_3D_Object_Rotate(const uint32_t& shaderProgram) const;
	void draw(const uint32_t& VAO, const uint32_t& indices) const;
public:
	void addObjectShader(uint32_t objectShader);
	void addLightSourceShader(uint32_t lightSourceShader);
	void terminateShaderPrograms();

	void addObjectData(const Drawable& drawable);
	void addLightSourceData(const Drawable& lightSource);
	void render(const glm::mat4& cameraView) const;
};

#endif // RENDER_H