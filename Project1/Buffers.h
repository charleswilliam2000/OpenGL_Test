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
    constexpr std::array<VertexAttributes, 1> voxelPackedAttributes = {
        VertexAttributes{ 0, 1, GL_UNSIGNED_INT,  GL_FALSE, sizeof(Vertex), (void*)0 }
    };
    constexpr std::array<VertexAttributes, 1> voxelFloatAttributes = {
        VertexAttributes{ 0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0 }
    };
}
struct DrawableBufferObjects {
public:
    GLuint VAO = 0, VBO = 0, EBO = 0;

    DrawableBufferObjects() {}

    DrawableBufferObjects(const std::vector<Vertex>& vertex, const std::array<VertexAttributes, 1>& attributes, const std::vector<uint32_t>& indices);

    DrawableBufferObjects(DrawableBufferObjects&& other) noexcept
        : VAO(other.VAO), VBO(other.VBO), EBO(other.EBO) {
        other.VAO = other.VBO = other.EBO = 0; 
    }

    template <size_t arr1_size, size_t arr2_size>
    DrawableBufferObjects(
        const std::array<float, arr1_size>& vertexData,
        const std::array<VertexAttributes, 1>& attributes,
        const std::array<uint32_t, arr2_size>& indices) 
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);

        for (const auto& attrib : attributes) {
            glVertexAttribPointer(attrib.index, attrib.componentCount, attrib.type, attrib.normalized, attrib.stride, attrib.offset);
            glEnableVertexAttribArray(attrib.index);
        }
        glBindVertexArray(0);
    }

    DrawableBufferObjects& operator=(DrawableBufferObjects&& other) noexcept {
        if (this != &other) {
            glDeleteVertexArrays(1, &VAO);
            glDeleteBuffers(1, &VBO);
            glDeleteBuffers(1, &EBO);

            // Move new resources
            VAO = other.VAO;
            VBO = other.VBO;
            EBO = other.EBO;

            other.VAO = other.VBO = other.EBO = 0; 
        }
        return *this;
    }

    ~DrawableBufferObjects() {
        if (VAO) glDeleteVertexArrays(1, &VAO);
        if (VBO) glDeleteBuffers(1, &VBO);
        if (EBO) glDeleteBuffers(1, &EBO);
    }
};

struct DeferredBufferObjects {
public:
    GLuint gBuffer = 0, rboDepth = 0;
    GLuint gPosition = 0, gNormal = 0, gColorSpecular = 0;

    DeferredBufferObjects();
    DeferredBufferObjects(DeferredBufferObjects&& other) noexcept
        : gBuffer(other.gBuffer), rboDepth(other.rboDepth), gPosition(other.gPosition), gNormal(other.gNormal), gColorSpecular(other.gColorSpecular) {
        other.gBuffer = other.rboDepth = other.gPosition = other.gNormal = other.gColorSpecular = 0;
    }

    DeferredBufferObjects& operator=(DeferredBufferObjects&& other) noexcept {
        if (this != &other) {
            glDeleteFramebuffers(1, &gBuffer);
            glDeleteTextures(1, &gPosition);
            glDeleteTextures(1, &gNormal);
            glDeleteTextures(1, &gColorSpecular);
            glDeleteRenderbuffers(1, &rboDepth);

            // Move new resources
            gBuffer = other.gBuffer;
            gPosition = other.gPosition;
            gNormal = other.gNormal;
            gColorSpecular = other.gColorSpecular;
            rboDepth = other.rboDepth;

            other.gBuffer = other.gPosition = other.gNormal = other.gColorSpecular = other.rboDepth = 0;
        }
        return *this;
    }

    ~DeferredBufferObjects() {
        glDeleteFramebuffers(1, &gBuffer);
        glDeleteTextures(1, &gPosition);
        glDeleteTextures(1, &gNormal);
        glDeleteTextures(1, &gColorSpecular);
        glDeleteRenderbuffers(1, &rboDepth);
    }
};

#endif // BUFFERS_H