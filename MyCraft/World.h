#ifndef WORLD_H
#define WORLD_H

#include <glad/glad.h>
#include <glfw/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <future>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <random>

#include "Constants.h"
#include "vectors.h"
#include "Indirect.h"
#include "Shader.h"
#include "Texture.h"
#include "Chunk.h"

using UniformsVEC3 = std::pair<const char*, glm::vec3>;
using Uniforms1F = std::pair<const char*, float>;

namespace WorldUtils {
	std::array<uint8_t, CONSTANTS::Dimension_2DSize> sampleHeightmap(const siv::PerlinNoise& perlin, uint32_t baseTerrainElevation, const float_VEC& chunkOffset);
}

class World {
	using WorldChunks = std::vector<WorldChunk>;
	using ChunkMeshes = std::vector<ChunkMesh>;
	using PointLightPositions = std::vector<glm::vec3>;

private:
	void setDirectionalLightUniform() const;
	void updateCameraChunkPos(const float_VEC& cameraPos);

	void renderQuad() const;
	void generateChunks(int gridSize, int verticalSize);

	PostProcessing::SSAO _ssao;
	IndirectRendering _indirect;

	UniformBufferObjects _vpUBO;
	UniformBufferObjects _modelsUBO;

	WorldChunks _worldChunks;
	ChunkMeshes _chunkMeshes;

	DrawableBufferObjects _world;
	GeometryBufferObjects _deferred;

	int32_VEC _cameraChunkPos;

	Shader _shaderGeometryPass;
	Shader _shaderSSAOPass;
	Shader _shaderLightingPass;
	Shader _wireframeShader;

	Texture _textureAtlas{};
public:
	World(int gridSize, int verticalSize);
	void render(const Camera& camera, const Frustum& cameraFrustum, bool wireframeMode);
};

class ChunkGenerationThread {
private:
	std::mutex _queueMutex;
	std::condition_variable _condition;
	std::queue<std::function<void()>> _tasks;
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