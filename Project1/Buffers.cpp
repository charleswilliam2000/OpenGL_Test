#include "Buffers.h"


void BufferObjects::generate_vertices_and_elements_buffers() {
    glGenBuffers(1, &_VBO);
    glGenBuffers(1, &_EBO);
}

void BufferObjects::bind_buffers_to_arrays(GLsizeiptr vertices_size, const void* vertices_data, GLsizeiptr vertices_index_size, const void* vertices_index_data) {
    glBindVertexArray(_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, _VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices_size, vertices_data, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertices_index_size, vertices_index_data, GL_STATIC_DRAW);
}

void BufferObjects::enable_vertex_attributes() {
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

void Buffers_Methods::terminateBufferObjects(BufferObjects& bufferObjects) {
    glDeleteVertexArrays(1, &bufferObjects._VAO);
    glDeleteBuffers(1, &bufferObjects._VBO);
    glDeleteBuffers(1, &bufferObjects._EBO);
}