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

GLenum glCheckError_(const char* file, int line);

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

	~WorldLighting() noexcept {
		glDeleteVertexArrays(1, &pointLightBuffers.VAO);
		glDeleteBuffers(1, &pointLightBuffers.VBO);
		glDeleteBuffers(1, &pointLightBuffers.EBO);
		glDeleteProgram(pointLightShader._shaderProgram);
	}
};

struct IndirectRendering {
	struct DrawCommands {
		uint32_t count, instanceCount, firstIndex, baseVertex, baseInstance;

		DrawCommands(uint32_t count, uint32_t instanceCount, uint32_t firstIndex, uint32_t baseVertex, uint32_t baseInstance)
			: count(count), instanceCount(instanceCount), firstIndex(firstIndex), baseVertex(baseVertex), baseInstance(baseInstance) { }
	}; std::vector<DrawCommands> drawCommands;
	std::vector<glm::mat4> modelMatrices;
	uint32_t indirectBuffer = 0, modelUniformBuffer = 0;
};

class World {
	using Chunks = std::vector<std::pair<WorldChunk, ChunkMesh>>;
private:
	void generateTerrain(const siv::PerlinNoise& perlin, WorldChunk::Blocks& chunkTerrain, const float_VEC& chunkOffset) const;
	IndirectRendering _indirect;
	Chunks _chunks{};

	WorldLighting* _worldLighting{};
	std::pair<uint32_t, uint32_t> _worldVerticesIndices = { 0, 0 };

	BufferObjects _worldBuffers{};
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