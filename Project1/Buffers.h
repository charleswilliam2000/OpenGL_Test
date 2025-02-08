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

    ~BufferObjects() {
        if (VAO) glDeleteVertexArrays(1, &VAO);
        if (VBO) glDeleteBuffers(1, &VBO);
        if (EBO) glDeleteBuffers(1, &EBO);
    }
};

struct DeferredBufferObjects {
public:
    GLuint gBuffer = 0, rboDepth = 0;
    GLuint gPosition = 0, gNormal = 0, gColorSpecular = 0;

    DeferredBufferObjects() {
        glGenFramebuffers(1, &gBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

        //Generate position color buffer
        glGenTextures(1, &gPosition);
        glBindBuffer(GL_TEXTURE_2D, gPosition);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Constants::WINDOW_WIDTH, Constants::WINDOW_HEIGHT, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

        //Generate normal color buffer
        glGenTextures(1, &gNormal);
        glBindBuffer(GL_TEXTURE_2D, gNormal);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Constants::WINDOW_WIDTH, Constants::WINDOW_HEIGHT, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

        //Generate color/specular buffer
        glGenTextures(1, &gColorSpecular);
        glBindBuffer(GL_TEXTURE_2D, gColorSpecular);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Constants::WINDOW_WIDTH, Constants::WINDOW_HEIGHT, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gColorSpecular, 0);

        constexpr GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
        glDrawBuffers(3, attachments);

        //Attach depth buffer (Render buffer)
        glGenRenderbuffers(1, &rboDepth);
        glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, Constants::WINDOW_WIDTH, Constants::WINDOW_HEIGHT);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "\nFramebuffer not complete!";
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
};

#endif // BUFFERS_H