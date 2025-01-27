#ifndef BUFFERS_H
#define BUFFERS_H

#include "stdafx.h"
#include "Shape.h"

struct VertexAttributes {
    GLuint index{};
    GLint componentCount{};
    GLenum type{};
    GLboolean normalized{};
    GLsizei stride{};
    const void* offset{};
};

namespace Attributes_Details {
    extern std::array<VertexAttributes, 1> voxelPackedAttributes;
    extern std::array<VertexAttributes, 1> voxelFloatAttributes;
}

struct BufferObjects {
public:
    GLuint VAO = 0, VBO = 0, EBO = 0;

    BufferObjects() {}

    BufferObjects(const std::vector<Vertex>& vertex, const std::array<VertexAttributes, 1>& attributes, const std::vector<uint32_t>& indices);

    BufferObjects(BufferObjects&& other) noexcept
        : VAO(other.VAO), VBO(other.VBO), EBO(other.EBO) {
        other.VAO = other.VBO = other.EBO = 0; 
    }


    template <size_t arr1_size, size_t arr2_size>
    BufferObjects(
        const std::array<float, arr1_size>& vertexData,
        const std::array<VertexAttributes, 1>& attributes,
        const std::array<uint32_t, arr2_size>& indices) {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(Vertex), vertexData.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);


        glVertexAttribPointer(attributes[0].index, attributes[0].componentCount, attributes[0].type, attributes[0].normalized, attributes[0].stride, attributes[0].offset);
        glEnableVertexAttribArray(attributes[0].index);
        
        glBindVertexArray(0);
    }

    BufferObjects& operator=(BufferObjects&& other) noexcept {
        if (this != &other) {
            // Cleanup existing resources
            glDeleteVertexArrays(1, &VAO);
            glDeleteBuffers(1, &VBO);
            glDeleteBuffers(1, &EBO);

            // Move new resources
            VAO = other.VAO;
            VBO = other.VBO;
            EBO = other.EBO;

            other.VAO = other.VBO = other.EBO = 0; // Nullify moved resources
        }
        return *this;
    }

    ~BufferObjects() {
        if (VAO) glDeleteVertexArrays(1, &VAO);
        if (VBO) glDeleteBuffers(1, &VBO);
        if (EBO) glDeleteBuffers(1, &EBO);
    }
};


#endif // BUFFERS_H