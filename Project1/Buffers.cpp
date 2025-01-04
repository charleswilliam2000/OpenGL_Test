#include "Buffers.h"


void BufferObjects::generate_vertices_and_elements_buffers() {
    glGenBuffers(NUM_VBO, &_VBO);
    glGenBuffers(NUM_EBO, &_EBO);
}

void BufferObjects::bind_buffers_to_arrays(GLsizeiptr vertices_size, const void* vertices_data, GLsizeiptr vertices_index_size, const void* vertices_index_data) {
    glBindVertexArray(_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, _VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices_size, vertices_data, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertices_index_size, vertices_index_data, GL_STATIC_DRAW);
}

void BufferObjects::enable_vertex_attributes() {
    VertexAttributes attributes[] = {
        {0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0},            //Coordinates
        {1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))},  //Colors (RGB)
        {2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)) } // Textures
    };

    for (const auto& attribute : attributes) {
        glVertexAttribPointer(attribute.index, attribute.componentCount, attribute.type, attribute.normalized, attribute.stride, attribute.offset);
        glEnableVertexAttribArray(attribute.index);
    }
}

void Buffers_Methods::terminateBufferObjects(BufferObjects& bufferObjects) {
    glDeleteVertexArrays(NUM_VAO, &bufferObjects._VAO);
    glDeleteBuffers(NUM_VBO, &bufferObjects._VBO);
    glDeleteBuffers(NUM_EBO, &bufferObjects._EBO);
}