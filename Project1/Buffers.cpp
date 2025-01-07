#include "Buffers.h"


void BufferObjects::generate_vertices_and_elements_buffers() {
    glGenBuffers(Constants::NUM_VBO, VBOs.data());
    glGenBuffers(Constants::NUM_EBO, EBOs.data());
}

void BufferObjects::bind_buffers_to_arrays(int array_idx, GLsizeiptr vertices_size, const void* vertices_data, GLsizeiptr vertices_index_size, const void* vertices_index_data) {
    glBindVertexArray(VAOs[array_idx]);

    _ASSERT_EXPR(Constants::NUM_VBO == Constants::NUM_EBO, "\nNumber of VBOs not equal to EBOs!");
    const int objectBuffersAmount = Constants::NUM_VBO;
    for (int i = 0; i < objectBuffersAmount; i++) {
        glBindBuffer(GL_ARRAY_BUFFER, VBOs[i]);
        glBufferData(GL_ARRAY_BUFFER, vertices_size, vertices_data, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[i]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertices_index_size, vertices_index_data, GL_STATIC_DRAW);
    }
}

void BufferObjects::enable_vertex_attributes(int array_idx) {
    VertexAttributes objectAttributes[] = {
            {0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0 },            //Coordinates
            {1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)) }, // Textures
            {2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)) } // Normals
    };
    VertexAttributes lightSourceAttrib = { 0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0 }; 

    switch (array_idx) {
    case 1:
        for (const auto& attribute : objectAttributes) {
            glVertexAttribPointer(attribute.index, attribute.componentCount, attribute.type, attribute.normalized, attribute.stride, attribute.offset);
            glEnableVertexAttribArray(attribute.index);
        }
        break;
    case 2:
        glVertexAttribPointer(lightSourceAttrib.index, lightSourceAttrib.componentCount, lightSourceAttrib.type, lightSourceAttrib.normalized, lightSourceAttrib.stride, lightSourceAttrib.offset);
        glEnableVertexAttribArray(lightSourceAttrib.index);
    }
}

void Buffers_Methods::terminateBufferObjects(BufferObjects& bufferObjects) {
    glDeleteVertexArrays(Constants::NUM_VAO, bufferObjects.VAOs.data());
    glDeleteBuffers(Constants::NUM_VBO, bufferObjects.VBOs.data());
    glDeleteBuffers(Constants::NUM_EBO, bufferObjects.EBOs.data());
}