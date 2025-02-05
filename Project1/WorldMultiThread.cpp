#include "World.h"

ChunkGenerationThread::ChunkGenerationThread(size_t numThreads) : _stop(false)
{
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

std::future<void> ChunkGenerationThread::enqueueTask(std::function<void()> task) {
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