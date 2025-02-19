#ifndef INDIRECT_H
#define INDIRECT_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shape.h"

#include <vector>
#include <iostream>

struct IndirectRendering {
	struct DrawCommands {
		uint32_t count = 0, instanceCount = 0, firstIndex = 0, baseVertex = 0, baseInstance = 0;

		DrawCommands() {}
		DrawCommands(uint32_t count, uint32_t instanceCount, uint32_t firstIndex, uint32_t baseVertex, uint32_t baseInstance)
			: count(count), instanceCount(instanceCount), firstIndex(firstIndex), baseVertex(baseVertex), baseInstance(baseInstance) {
		}
	}; std::vector<DrawCommands> drawCommands;
	DrawCommands* indirectBufferPersistentPtr = nullptr;
	uint32_t indirectBuffer = 0;

	IndirectRendering() {}

	void addDrawCommand(uint32_t count, uint32_t instanceCount, uint32_t firstIndex, uint32_t baseVertex, uint32_t baseInstance);
	void generateBufferPersistent();
};

#endif INDIRECT_H