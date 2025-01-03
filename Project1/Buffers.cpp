#include "Buffers.h"

void Buffers_Methods::terminateBufferObjects(BufferObjects& bufferObjects) {
    glDeleteVertexArrays(1, &bufferObjects._VAO);
    glDeleteBuffers(1, &bufferObjects._VBO);
    glDeleteBuffers(1, &bufferObjects._EBO);
}