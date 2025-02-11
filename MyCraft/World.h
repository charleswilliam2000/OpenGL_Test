#ifndef WORLD_H
#define WORLD_H

#include <future>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

#include "Chunk.h"
#include "Indirect.h"
#include "Shader.h"
#include "Texture.h"

using UniformsVEC3 = std::pair<const char*, glm::vec3>;
using Uniforms1F = std::pair<const char*, float>;

class World {
	using WorldChunks = std::vector<WorldChunk>;
	using ChunkMeshes = std::vector<ChunkMesh>;
	using PointLightPositions = std::vector<glm::vec3>;
private:
	void setDirectionalLightUniform() const;
	void updateCameraChunkPos(const float_VEC& cameraPos);

	void renderQuad() const;
	void renderSkybox(const glm::mat4& view, const glm::mat4& projection) const;

	std::array<uint8_t, CONSTANTS::Dimension_2DSize> sampleHeightmap(const siv::PerlinNoise& perlin, uint32_t baseTerrainElevation, const float_VEC& chunkOffset);
	
	IndirectRendering _indirect;

	UniformBufferObjects _vpUBO;
	UniformBufferObjects _modelsUBO;

	WorldChunks _worldChunks;
	ChunkMeshes _chunkMeshes;

	DrawableBufferObjects _world;
	DrawableBufferObjects _skybox;
	DeferredBufferObjects _deferred;

	int32_VEC _cameraChunkPos;

	uint32_t _numVertices = 0;
	uint32_t _numIndices = 0;

	ShaderProgram _shaderGeometryPass;
	ShaderProgram _shaderLightingPass;
	ShaderProgram _skyboxShader;
	ShaderProgram _wireframeShader;

	Texture _textureAtlas{};
public:
	World(int gridSize, int verticalSize);
	void generateChunks(int gridSize, int verticalSize);
	void render(const Camera& camera, const Frustum& cameraFrustum, bool wireframeMode);
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