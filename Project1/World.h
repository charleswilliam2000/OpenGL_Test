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

class ChunkGenerationThread {
private:
	std::mutex _queueMutex;
	std::condition_variable _condition;
	std::queue<std::function<void()>> tasks;
	std::vector<std::thread> _workers;
	std::atomic<bool> _stop;
public:
	ChunkGenerationThread(size_t numThreads) : _stop(false) {
		for (size_t i = 0; i < numThreads; i++) {
			_workers.emplace_back([this]() {
				while (true) {
					std::function<void()> task = nullptr;

					{
						std::unique_lock<std::mutex> lock(_queueMutex);
						_condition.wait(lock, [this]() {
							return _stop || !tasks.empty();
							});

						if (_stop && tasks.empty())
							return;

						task = std::move(tasks.front());
						tasks.pop();
						
					}
					task();
				}
				});
		}
	}

	std::future<void> enqueueTask(std::function<void()> task) {
		auto promise = std::make_shared<std::promise<void>>();
		auto future = promise->get_future();
		{
			std::unique_lock<std::mutex> lock(_queueMutex);
			tasks.emplace([task = std::move(task), promise]() {
				try {
					task();
					promise->set_value();
				}
				catch (...) {
					promise->set_exception(std::current_exception());
				}
				});
		}
		_condition.notify_one();
		return future;
	}

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