#ifndef WORLD_H
#define WORLD_H

#include <future>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

#include "Chunk.h"
#include "Shader.h"
#include "Texture.h"

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
	void initializeWorldVPUniformBuffer(uint32_t& vpUniformBuffer, glm::mat4*& vpPersistentPtr) const;
	void updateCameraChunkPos(const float_VEC& cameraPos);

	void renderChunks(const uint32_t& vpUniformBuffer, const float_VEC& cameraPos, const Frustum& cameraFrustum);
	void renderQuad() const;
	void renderSkybox(const glm::mat4& view, const glm::mat4& projection) const;

	std::array<uint8_t, ChunkConstants::Dimension_2DSize> sampleHeightmap(const siv::PerlinNoise& perlin, uint32_t baseTerrainElevation, const float_VEC& chunkOffset);

	IndirectRendering _indirect;

	WorldChunks _worldChunks{};
	ChunkMeshes _chunkMeshes{};

	struct DeferredRendering {
		DeferredBufferObjects buffers{};
		ShaderProgram geometryShader{};
		ShaderProgram deferredShader{};
	} _deferred;

	struct Skybox {
		DrawableBufferObjects buffers{};
		ShaderProgram shader{};
	} _skybox{};

	int32_VEC _cameraChunkPos{};
	DrawableBufferObjects _worldBuffers{};
	uint32_t _numVertices = 0;
	uint32_t _numIndices = 0;
	ShaderProgram _wireframeShader{};
	Texture _textureAtlas{};
public:
	World();
	void generateChunks(int gridSize, int verticalSize);
	void render(const Camera& camera, const Frustum& cameraFrustum, bool wireframeMode);

	~World() noexcept {
		if (_indirect.indirectBufferPersistentPtr) {
			glUnmapBuffer(_indirect.indirectBuffer);
			_indirect.indirectBufferPersistentPtr = nullptr;
		}

		glDeleteBuffers(1, &_indirect.indirectBuffer);
		glDeleteBuffers(1, &_indirect.modelUniformBuffer);

		glDeleteProgram(_skybox.shader.shaderProgram);
		glDeleteProgram(_deferred.geometryShader.shaderProgram);
		glDeleteProgram(_deferred.deferredShader.shaderProgram);
		glDeleteProgram(_wireframeShader.shaderProgram);
	}
};

class ChunkGenerationThread {
private:
	std::mutex _queueMutex;
	std::condition_variable _condition;
	std::queue<std::function<void()>> tasks;
	std::vector<std::thread> _workers;
	std::atomic<bool> _stop;
public:
	ChunkGenerationThread(size_t numThreads);

	std::future<void> enqueueTask(std::function<void()> task);

	~ChunkGenerationThread() noexcept {
		{
			std::unique_lock<std::mutex> lock(_queueMutex);
			_stop = true;
		}
		_condition.notify_all();
		for (auto& worker : _workers) {
			worker.join();
		}
	}

};

#endif // WORLD_H