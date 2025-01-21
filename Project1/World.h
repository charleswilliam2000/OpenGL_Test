#ifndef WORLD_H
#define WORLD_H

#include <iostream>
#include <thread>
#include <future>

#include "Chunk.h"

#include "Camera.h"
#include "Shader.h"
#include "Texture.h"

#include "PerlinNoise.hpp"

class WorldLighting {
	using UniformsVEC3 = std::pair<const char*, glm::vec3>;
	using Uniforms1F = std::pair<const char*, float>;
	using PointLightPositions = std::vector<glm::vec3>;
private:
	PointLightPositions pointLightPositions{};
	BufferObjects pointLightBuffers{};
	ShaderProgram pointLightShader{};
public:
	WorldLighting() {}
	WorldLighting(BufferObjects bufferObjects, const char* shaderVertexProgramName, const char* shaderFragmentProgramName);

	void addPointLight(const glm::vec3& pointLightPos);
	void addPointLights(PointLightPositions&& pointLightsPos);

	void setDirectionalLightUniform(const ShaderProgram& worldShaderProgram) const noexcept;
	void setPointLightsUniform(const ShaderProgram& worldShaderProgram) const;

	void renderPointLights(const glm::mat4& cameraView, const glm::mat4& projectionMat)  const noexcept;

	~WorldLighting() {
		glDeleteVertexArrays(1, &pointLightBuffers.VAO);
		glDeleteBuffers(1, &pointLightBuffers.VBO);
		glDeleteBuffers(1, &pointLightBuffers.EBO);
		glDeleteProgram(pointLightShader._shaderProgram);
	}
};

class World {
	using Chunks = std::vector<std::pair<WorldChunk, ChunkMesh>>;
private:
	void generateTerrain(const siv::PerlinNoise& perlin, WorldChunk::Blocks& chunkTerrain, const float_VEC& chunkOffset) const;
	Chunks _chunks{};
	WorldLighting* _worldLighting{};
	ShaderProgram _worldShader{};
	Texture _textureAtlas{};
public:
	World() {}
	World(WorldLighting* worldLighting, ShaderProgram worldShader, Texture textureAtlas);
	void generateChunks(size_t numChunks);
	void render(const Camera& camera, bool wireframeMode) const;

	~World() noexcept {
		for (const auto& chunk : _chunks) {
			glDeleteVertexArrays(1, &chunk.second.chunkData.VAO);
			glDeleteBuffers(1, &chunk.second.chunkData.VBO);
			glDeleteBuffers(1, &chunk.second.chunkData.EBO);
		}
		glDeleteProgram(_worldShader._shaderProgram);
	}
};


#endif // WORLD_H