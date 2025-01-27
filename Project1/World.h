#ifndef WORLD_H
#define WORLD_H

#include <thread>
#include <future>

#include "Chunk.h"

#include "Shader.h"
#include "Texture.h"

#include "PerlinNoise.hpp"

GLenum glCheckError_(const char* file, int line);

class WorldSkybox {
private:
	BufferObjects skyboxBuffers{};
	ShaderProgram skyboxShader{};
public:
	WorldSkybox() {}
	WorldSkybox(BufferObjects bufferObjects, const char* shaderVertexProgramName, const char* shaderFragmentProgramName);

	WorldSkybox(const WorldSkybox&) = delete;
	WorldSkybox& operator=(const WorldSkybox&) = delete;
	WorldSkybox(WorldSkybox&&) noexcept = default;
	WorldSkybox& operator=(WorldSkybox&&) noexcept = default;

	void renderSkybox(const glm::mat4& cameraView, const glm::mat4& projection) const;

	~WorldSkybox() noexcept {
		glDeleteProgram(skyboxShader._shaderProgram);
	}
};

class WorldLighting {
	using PointLightPositions = std::vector<glm::vec3>;
private:
	PointLightPositions pointLightPositions{};
	BufferObjects pointLightBuffers{};
	ShaderProgram pointLightShader{};
public:
	WorldLighting() {}
	WorldLighting(BufferObjects bufferObjects, const char* shaderVertexProgramName, const char* shaderFragmentProgramName);

	WorldLighting(const WorldLighting&) = delete; 
	WorldLighting& operator=(const WorldLighting&) = delete;
	WorldLighting(WorldLighting&&) noexcept = default;
	WorldLighting& operator=(WorldLighting&&) noexcept = default; 

	void addPointLight(const glm::vec3& pointLightPos);
	void addPointLights(PointLightPositions&& pointLightsPos);
	void renderPointLights(const glm::mat4& cameraView, const glm::mat4& projectionMat)  const;

	~WorldLighting() noexcept {
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

using UniformsVEC3 = std::pair<const char*, glm::vec3>;
using Uniforms1F = std::pair<const char*, float>;

class World {
	using WorldChunks = std::vector<WorldChunk>;
	using ChunkMeshes = std::vector<ChunkMesh>;
private:
	void setDirectionalLightUniform() const;
	void setPointLightsUniform() const;
	void generateTerrain(const siv::PerlinNoise& perlin, WorldChunk::Blocks& chunkTerrain, const float_VEC& chunkOffset) const;
	IndirectRendering _indirect;

	WorldChunks _worldChunks{};
	ChunkMeshes _chunkMeshes{};
	
	WorldSkybox _worldSkybox{};
	WorldLighting _worldLighting{};

	std::pair<uint32_t, uint32_t> _worldVerticesIndices = { 0, 0 };

	BufferObjects _worldBuffers{};
	ShaderProgram _worldShader{};
	Texture _textureAtlas{};
public:
	World() {}
	World(WorldSkybox worldSkybox, WorldLighting worldLighting, ShaderProgram worldShader, Texture textureAtlas);
	void generateChunks(size_t numChunks);
	void render(const Camera& camera, const Frustum& cameraFrustum, bool wireframeMode);

	~World() noexcept {
		glDeleteBuffers(1, &_indirect.indirectBuffer);
		glDeleteBuffers(1, &_indirect.modelUniformBuffer);
		glDeleteProgram(_worldShader._shaderProgram);
	}
};


#endif // WORLD_H