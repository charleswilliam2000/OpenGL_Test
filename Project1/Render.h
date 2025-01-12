#ifndef RENDER_H
#define RENDER_H

#include "stdafx.h"
#include <vector>

#include "Shape.h"
#include "Texture.h"
#include "Shader.h"
#include "Camera.h"

struct Drawable {
	uint32_t VAO{};
	uint32_t texture{};
	uint32_t indices{};
	float_VEC coordinate{};

	Drawable() {};
	Drawable(uint32_t VAO, uint32_t texture, uint32_t indices, float_VEC coordinate) : VAO(VAO), texture(texture), indices(indices), coordinate(coordinate) {}

};

class Renderer {
	using Objects = std::vector<Drawable>;
	using ObjectShaders = std::vector<uint32_t>;
private:
	Objects _objectsData{};
	ObjectShaders _objectShaders{};

	Drawable _lightSource{};
	uint32_t _lightSourceShader{};

	uint32_t _wireframeShader{};

	void updateUniforms(const uint32_t& shaderProgram, const glm::mat4& projectionMat, const glm::mat4& modelMat) const;
	void have_3D_Object_Rotate(const uint32_t& shaderProgram) const;
	void draw(const uint32_t& VAO, const uint32_t& indices) const;
public:
	void addObjectShader(uint32_t objectShader);
	void addLightSourceShader(uint32_t lightSourceShader);
	void addWireframeShader(uint32_t wireframeShader);
	void terminateShaderPrograms();

	void addObjectData(const Drawable& drawable);
	void addLightSourceData(const Drawable& lightSource);
	void render(const glm::vec3& cameraPos, const glm::mat4& cameraView, bool wireframeMode) const;
};

#endif // RENDER_H