#ifndef WORLD_H
#define WORLD_H

#include <thread>
#include <future>

#include "Chunk.h"

#include "Shader.h"
#include "Texture.h"

#include "PerlinNoise.hpp"

GLenum glCheckError_(const char* file, int line);

struct IndirectRendering {
	struct DrawCommands {
		uint32_t count, instanceCount, firstIndex, baseVertex, baseInstance;

		DrawCommands(uint32_t count, uint32_t instanceCount, uint32_t firstIndex, uint32_t baseVertex, uint32_t baseInstance)
			: count(count), instanceCount(instanceCount), firstIndex(firstIndex), baseVertex(baseVertex), baseInstance(baseInstance) { }
	}; std::vector<DrawCommands> drawCommands;
	std::vector<glm::mat4> modelMatrices;
	IndirectRendering::DrawCommands* indirectBufferPersistentPtr = nullptr;
	uint32_t indirectBuffer = 0, modelUniformBuffer = 0;
};

using UniformsVEC3 = std::pair<const char*, glm::vec3>;
using Uniforms1F = std::pair<const char*, float>;

class World {
	using WorldChunks = std::vector<WorldChunk>;
	using ChunkMeshes = std::vector<ChunkMesh>;
	using PointLightPositions = std::vector<glm::vec3>;
private:
	void setDirectionalLightUniform() const;
	void setPointLightsUniform() const;
	void generateTerrain(const siv::PerlinNoise& perlin, WorldChunk::Blocks& chunkTerrain, const float_VEC& chunkOffset) const;
	IndirectRendering _indirect;

	WorldChunks _worldChunks{};
	ChunkMeshes _chunkMeshes{};
	
	std::pair<uint32_t, uint32_t> _worldVerticesIndices = { 0, 0 };
	struct PointLights {
		PointLightPositions positions{};
		BufferObjects pointLightBuffers{};
		ShaderProgram pointLightShader{};
	} _pointLights;

	struct Skybox {
		BufferObjects skyboxBuffers{};
		ShaderProgram skyboxShader{};
	} _skybox;

	BufferObjects _worldBuffers{};
	ShaderProgram _worldShader{};

	ShaderProgram _wireframeShader{};

	Texture _textureAtlas{};
public:
	World(ShaderProgram worldShader, Texture textureAtlas);
	void generateChunks(size_t numChunks);
	void render(const Camera& camera, const Frustum& cameraFrustum, bool wireframeMode);

	~World() noexcept {
		if (_indirect.indirectBufferPersistentPtr) {
			glUnmapBuffer(_indirect.indirectBuffer);
			_indirect.indirectBufferPersistentPtr = nullptr;
		}

		glDeleteBuffers(1, &_indirect.indirectBuffer);
		glDeleteBuffers(1, &_indirect.modelUniformBuffer);

		glDeleteProgram(_pointLights.pointLightShader._shaderProgram);
		glDeleteProgram(_skybox.skyboxShader._shaderProgram);
		glDeleteProgram(_worldShader._shaderProgram);
		glDeleteProgram(_wireframeShader._shaderProgram);
	}
};


#endif // WORLD_H