#include "Indirect.h"

IndirectRendering::IndirectRendering(const std::vector<PackedVertex>& vertex, const std::vector<uint32_t>& indices) {
	size_t firstIndexOffset = 0;
	drawCommands.emplace_back(
		static_cast<uint32_t>(indices.size()),												//Counts
		1,																					// Instance count
		static_cast<uint32_t>(firstIndexOffset),											// firstIndex (Index offset after the last chunk)
		0,																					// Base vertex 
		0																					// Base instance (No instancing)
	);
	firstIndexOffset += indices.size();

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

void IndirectRendering::generateBufferPersistent(const std::vector<DrawCommands>& in_drawCommands) {
	if (in_drawCommands.empty()) {
		throw std::runtime_error("\nDraw commands are empty!");
	}
	else {
		this->drawCommands = in_drawCommands;

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
