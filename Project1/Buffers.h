#ifndef BUFFERS_H
#define BUFFERS_H

#include <glad/glad.h>
#include <array> 
#include <iostream>

#include "Constants.h"


struct VertexAttributes {
    GLuint index{};
    GLint componentCount{};
    GLenum type{};
    GLboolean normalized{};
    GLsizei stride{};
    const void* offset{};
};

namespace Attributes_Details {
    extern std::array<VertexAttributes, 3> objectAttributes;
    extern std::array<VertexAttributes, 1> lightSourceAttributes;
}


struct BufferObjects {
public:
    GLuint VAO, VBO, EBO = 0;
    template <size_t arr1_size, size_t arr2_size, size_t arr3_size>
    BufferObjects(
        const std::array<float, arr1_size>& vertexData,
        const std::array<VertexAttributes, arr2_size>& attributes,
        const std::array<uint32_t, arr3_size>& indices = {}) {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_STATIC_DRAW);

        if (!indices.empty()) {
            glGenBuffers(1, &EBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
        }

        for (const auto& attr : attributes) {
            glVertexAttribPointer(attr.index, attr.componentCount, attr.type, attr.normalized, attr.stride, attr.offset);
            glEnableVertexAttribArray(attr.index);
        }

        glBindVertexArray(0);
    }

    ~BufferObjects() {
        glDeleteBuffers(1, &VBO);
        if (EBO) glDeleteBuffers(1, &EBO);
        glDeleteVertexArrays(1, &VAO);
    }

    void bind() const { glBindVertexArray(VAO); }
    void unbind() const { glBindVertexArray(0); }
};


#endif // BUFFERS_H