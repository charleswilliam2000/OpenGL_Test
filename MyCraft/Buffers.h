#ifndef BUFFERS_H
#define BUFFERS_H

#include <glad/glad.h>

#include "Shape.h"

#include <array>
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

struct DeferredBufferObjects {
public:
    GLuint gBuffer = 0, rboDepth = 0;
    GLuint gPosition = 0, gNormal = 0, gColorSpecular = 0;

    DeferredBufferObjects() {}
    DeferredBufferObjects(int windowWidth, int windowHeight);

    void generateBuffers(int windowWidth, int windowHeight);

    DeferredBufferObjects(const DeferredBufferObjects&) = delete;
    DeferredBufferObjects& operator=(const DeferredBufferObjects&) = delete;

    ~DeferredBufferObjects() {
        glDeleteFramebuffers(1, &gBuffer);
        glDeleteTextures(1, &gPosition);
        glDeleteTextures(1, &gNormal);
        glDeleteTextures(1, &gColorSpecular);
        glDeleteRenderbuffers(1, &rboDepth);
    }
};

#endif // BUFFERS_H