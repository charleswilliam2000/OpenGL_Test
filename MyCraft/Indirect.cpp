#include "Indirect.h"

void IndirectRendering::addDrawCommand(uint32_t count, uint32_t instanceCount, uint32_t firstIndex, uint32_t baseVertex, uint32_t baseInstance) {
	drawCommands.emplace_back(count, instanceCount, firstIndex, baseVertex, baseInstance);
}

void IndirectRendering::generateBufferPersistent() {
	if (drawCommands.empty()) {
		throw std::runtime_error("\nDraw commands are empty!");
	}
	else {
		glGenBuffers(1, &indirectBuffer);
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBuffer);
		glBufferStorage(GL_DRAW_INDIRECT_BUFFER, drawCommands.size() * sizeof(DrawCommands), drawCommands.data(), GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
		indirectBufferPersistentPtr = static_cast<DrawCommands*>(
			glMapBufferRange(GL_DRAW_INDIRECT_BUFFER, 0, drawCommands.size() * sizeof(DrawCommands),
				GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT)
			);

		if (!indirectBufferPersistentPtr)
			throw std::runtime_error("\nUnable to initialize indirectBufferPersistentPtr");

		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
	}
}
