#ifndef BUFFERS_H
#define BUFFERS_H

#include "stdafx.h"
#include <array> 
#include <vector>

struct VertexAttributes {
    GLuint index{};
    GLint componentCount{};
    GLenum type{};
    GLboolean normalized{};
    GLsizei stride{};
    const void* offset{};
};

namespace Attributes_Details {
    constexpr unsigned char num_objectAttributes = 3;
    extern std::array<VertexAttributes, num_objectAttributes> objectAttributes;
    extern std::array<VertexAttributes, 1> lightSourceAttributes;
}



struct BufferObjects {
public:
    GLuint VAO = 0, VBO = 0, EBO = 0;
    uint32_t object_indices = 0;

    BufferObjects() {}

    template <size_t arr1_size>
    BufferObjects(
        const std::array<VertexAttributes, arr1_size>& attributes,
        const std::vector<uint32_t>& indices,
        GLuint sharedVBO = 0) : object_indices(static_cast<int>(indices.size())) {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, sharedVBO);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);
        

        for (const auto& attr : attributes) {
            glVertexAttribPointer(attr.index, attr.componentCount, attr.type, attr.normalized, attr.stride, attr.offset);
            glEnableVertexAttribArray(attr.index);
        }
        glBindVertexArray(0);
    }

    template <size_t arr1_size, size_t arr2_size, size_t arr3_size>
    BufferObjects(
        const std::array<float, arr1_size>& vertexData,
        const std::array<VertexAttributes, arr2_size>& attributes,
        const std::array<uint32_t, arr3_size>& indices) {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);


        for (const auto& attr : attributes) {
            glVertexAttribPointer(attr.index, attr.componentCount, attr.type, attr.normalized, attr.stride, attr.offset);
            glEnableVertexAttribArray(attr.index);
        }
        glBindVertexArray(0);
    }

    ~BufferObjects() {
        if (VAO) glDeleteVertexArrays(1, &VAO);
        if (VBO) glDeleteBuffers(1, &VBO);
        if (EBO) glDeleteBuffers(1, &EBO);
    }
};


#endif // BUFFERS_H