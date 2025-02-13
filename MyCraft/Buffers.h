#ifndef BUFFERS_H
#define BUFFERS_H

#include <glad/glad.h>

#include "Shape.h"

#include <array>
#include <vector>
#include <iostream>

struct DrawableIntAttributes {
    GLuint index;
    GLuint size;
    GLenum type;
    GLsizei stride;
    GLsizei offset;
};

struct DrawableFloatAttributes {
    GLuint index;
    GLuint size;
    GLenum type;
    GLboolean normalized;
    GLsizei stride;
    GLsizei offset;  // Changed from const void* to std::size_t

    constexpr DrawableFloatAttributes(GLuint index, GLuint size, GLenum type, GLboolean normalized, GLsizei stride, GLsizei offset)
        : index(index), size(size), type(type), normalized(normalized), stride(stride), offset(offset) {
    }
};
namespace DRAWABLE_ATTRIBUTES {
    constexpr std::array<DrawableIntAttributes, 1> DRAWABLE_PACKED_ATTRIBUTES = { {
            DrawableIntAttributes{0, 1, GL_UNSIGNED_INT, sizeof(PackedVertex), 0}
    } };

    constexpr std::array<DrawableFloatAttributes, 3> DRAWABLE_FLOAT_ATTRIBUTES = { {
        DrawableFloatAttributes(0, 3, GL_FLOAT, GL_FALSE, sizeof(FloatVertex), 0),
        DrawableFloatAttributes(1, 3, GL_FLOAT, GL_FALSE, sizeof(FloatVertex), 3 * sizeof(float)),
        DrawableFloatAttributes(2, 2, GL_FLOAT, GL_FALSE, sizeof(FloatVertex), 6 * sizeof(float))
    } };
}
struct DrawableBufferObjects {
public:
    GLuint vaoHandle = 0, vboHandle = 0, eboHandle = 0;
    GLuint vboSize = 0, eboSize = 0;

    DrawableBufferObjects() {}
    DrawableBufferObjects(size_t vboSize, const void* vboData, size_t eboSize, const void* eboData, const std::array<DrawableFloatAttributes, 3>& readAttribArr);
    DrawableBufferObjects(size_t vboSize, const void* vboData, size_t eboSize, const void* eboData, const std::array<DrawableIntAttributes, 1>& readAttribArr);

    void generateBuffersF(size_t vboSize, const void* vboData, size_t eboSize, const void* eboData, const std::array<DrawableFloatAttributes, 3>& readAttribArr);
    void generateBuffersI(size_t vboSize, const void* vboData, size_t eboSize, const void* eboData, const std::array<DrawableIntAttributes, 1>& readAttribArr);

    void bindToDraw() const {
        glBindVertexArray(vaoHandle);
    }

    DrawableBufferObjects(const DrawableBufferObjects&) = delete;
    DrawableBufferObjects& operator=(const DrawableBufferObjects&) = delete;

    ~DrawableBufferObjects() {
        glDeleteVertexArrays(1, &vaoHandle);
        glDeleteBuffers(1, &vboHandle);
        glDeleteBuffers(1, &eboHandle);
    }

};

struct UniformBufferObjects {
public:
    void* persistentPtr = nullptr;
    GLuint handle = 0, bindingPoint = 0, bufferSize = 0;

    UniformBufferObjects() {}
    UniformBufferObjects(size_t size, GLuint bindingPoint, const void* data = nullptr);

    void generateBuffersPersistent(size_t size, GLuint bindingPoint, const void* data = nullptr);

    UniformBufferObjects(const UniformBufferObjects&) = delete;
    UniformBufferObjects& operator=(const UniformBufferObjects&) = delete;

    void bindToShader(GLuint shaderProgram, const char* blockName) {
        GLuint blockIndex = glGetUniformBlockIndex(shaderProgram, blockName);
        if (blockIndex != GL_INVALID_INDEX) {
            glUniformBlockBinding(shaderProgram, blockIndex, bindingPoint);
        }
    }

    ~UniformBufferObjects() {
        glDeleteBuffers(1, &handle);
    }
};

struct GeometryBufferObjects {
public:
    GLuint gFBO = 0, gDepthText = 0, gTextArray = 0; // gBuffers

    GeometryBufferObjects() {}
    void generateBuffers(int windowWidth, int windowHeight);

    GeometryBufferObjects(const GeometryBufferObjects&) = delete;
    GeometryBufferObjects& operator=(const GeometryBufferObjects&) = delete;

    ~GeometryBufferObjects() {
        glDeleteFramebuffers(1, &gFBO);
        glDeleteTextures(1, &gTextArray);
        glDeleteTextures(1, &gDepthText);
    }
};

namespace PostProcessing {
    struct SSAO {
        GLuint ssaoFBO = 0, ssaoColorText = 0, ssaoBlurFBO = 0, ssaoBlurText = 0;
        GLuint ssaoNoise = 0;

        SSAO() {}
        void generateSSAO(int windowWidth, int windowHeight, const void* noiseData);

        SSAO(const SSAO&) = delete;
        SSAO& operator=(const SSAO&) = delete;

        ~SSAO() {
            glDeleteFramebuffers(1, &ssaoFBO);
            glDeleteFramebuffers(1, &ssaoBlurFBO);
            glDeleteTextures(1, &ssaoColorText);
            glDeleteTextures(1, &ssaoBlurText);
            glDeleteTextures(1, &ssaoNoise);
        }
    };
}

#endif // BUFFERS_H