#include "Buffers.h"

DrawableBufferObjects::DrawableBufferObjects(size_t vboSize, const void* vboData, size_t eboSize, const void* eboData, const std::array<DrawableFloatAttributes, 3>& readAttribArr) 
    : vboSize(static_cast<GLuint>(vboSize)), eboSize(static_cast<GLuint>(eboSize)) {
    glGenVertexArrays(1, &vaoHandle);
    glGenBuffers(1, &vboHandle);
    glGenBuffers(1, &eboHandle);

    glBindVertexArray(vaoHandle);

    glBindBuffer(GL_ARRAY_BUFFER, vboHandle);
    glBufferData(GL_ARRAY_BUFFER, vboSize, vboData, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboHandle);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, eboSize, eboData, GL_STATIC_DRAW);

    for (const auto& attrib : readAttribArr) {
        glEnableVertexAttribArray(attrib.index);
        glVertexAttribPointer(attrib.index, attrib.size, attrib.type, attrib.normalized, attrib.stride, reinterpret_cast<void*>(attrib.offset));
    }

    glBindVertexArray(0);
}


DrawableBufferObjects::DrawableBufferObjects(size_t vboSize, const void* vboData, size_t eboSize, const void* eboData, const std::array<DrawableIntAttributes, 1>& readAttribArr)
    : vboSize(static_cast<GLuint>(vboSize)), eboSize(static_cast<GLuint>(eboSize)) {
    glGenVertexArrays(1, &vaoHandle);
    glGenBuffers(1, &vboHandle);
    glGenBuffers(1, &eboHandle);

    glBindVertexArray(vaoHandle);

    glBindBuffer(GL_ARRAY_BUFFER, vboHandle);
    glBufferData(GL_ARRAY_BUFFER, vboSize, vboData, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboHandle);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, eboSize, eboData, GL_STATIC_DRAW);

    for (const auto& attrib : readAttribArr) {
        glEnableVertexAttribArray(attrib.index);
        glVertexAttribIPointer(attrib.index, attrib.size, attrib.type, attrib.stride, reinterpret_cast<void*>(attrib.offset));
    }

    glBindVertexArray(0);
}

void DrawableBufferObjects::generateBuffersF(size_t vboSize, const void* vboData, size_t eboSize, const void* eboData, const std::array<DrawableFloatAttributes, 3>& readAttribArr) {
    this->vboSize = static_cast<GLuint>(vboSize);
    this->eboSize = static_cast<GLuint>(eboSize);

    glGenVertexArrays(1, &vaoHandle);
    glGenBuffers(1, &vboHandle);
    glGenBuffers(1, &eboHandle);

    glBindVertexArray(vaoHandle);

    glBindBuffer(GL_ARRAY_BUFFER, vboHandle);
    glBufferData(GL_ARRAY_BUFFER, vboSize, vboData, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboHandle);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, eboSize, eboData, GL_STATIC_DRAW);

    for (const auto& attrib : readAttribArr) {
        glEnableVertexAttribArray(attrib.index);
        glVertexAttribPointer(attrib.index, attrib.size, attrib.type, attrib.normalized, attrib.stride, reinterpret_cast<void*>(attrib.offset));
    }

    glBindVertexArray(0);
}

void DrawableBufferObjects::generateBuffersI(size_t vboSize, const void* vboData, size_t eboSize, const void* eboData, const std::array<DrawableIntAttributes, 1>& readAttribArr) {
    this->vboSize = static_cast<GLuint>(vboSize);
    this->eboSize = static_cast<GLuint>(eboSize);

    glGenVertexArrays(1, &vaoHandle);
    glGenBuffers(1, &vboHandle);
    glGenBuffers(1, &eboHandle);

    glBindVertexArray(vaoHandle);

    glBindBuffer(GL_ARRAY_BUFFER, vboHandle);
    glBufferData(GL_ARRAY_BUFFER, vboSize, vboData, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboHandle);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, eboSize, eboData, GL_STATIC_DRAW);

    for (const auto& attrib : readAttribArr) {
        glEnableVertexAttribArray(attrib.index);
        glVertexAttribIPointer(attrib.index, attrib.size, attrib.type, attrib.stride, reinterpret_cast<void*>(attrib.offset));
    }

    glBindVertexArray(0);
}

UniformBufferObjects::UniformBufferObjects(size_t size, GLuint bindingPoint, const void* data) 
    : bindingPoint(bindingPoint), bufferSize(static_cast<GLuint>(size)) {
        glGenBuffers(1, &handle);
        glBindBuffer(GL_UNIFORM_BUFFER, handle);
        glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, handle);

        glBufferStorage(GL_UNIFORM_BUFFER, bufferSize, data, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
        persistentPtr = glMapBufferRange(GL_UNIFORM_BUFFER, 0, bufferSize, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);

        if (!persistentPtr) {
            throw std::runtime_error("\nUnable to initialize persistent ptr");
        }

        glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
void UniformBufferObjects::generateBuffersPersistent(size_t size, GLuint bindingPoint, const void* data) {
    this->bindingPoint = bindingPoint;
    this->bufferSize = size;

    glGenBuffers(1, &handle);
    glBindBuffer(GL_UNIFORM_BUFFER, handle);
    glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, handle);

    glBufferStorage(GL_UNIFORM_BUFFER, bufferSize, data, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
    persistentPtr = glMapBufferRange(GL_UNIFORM_BUFFER, 0, bufferSize, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);

    if (!persistentPtr) {
        throw std::runtime_error("\nUnable to initialize persistent ptr");
    }

    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

DeferredBufferObjects::DeferredBufferObjects(int windowWidth, int windowHeight) {
    glGenFramebuffers(1, &gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

    //Generate position color buffer
    glGenTextures(1, &gPosition);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, windowWidth, windowHeight, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

    //Generate normal color buffer
    glGenTextures(1, &gNormal);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, windowWidth, windowHeight, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

    //Generate color/specular buffer
    glGenTextures(1, &gColorSpecular);
    glBindTexture(GL_TEXTURE_2D, gColorSpecular);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, windowWidth, windowHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gColorSpecular, 0);

    constexpr GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, attachments);

    //Attach depth buffer (Render buffer)
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, windowWidth, windowHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        throw std::runtime_error("\nFramebuffer not complete!");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DeferredBufferObjects::generateBuffers(int windowWidth, int windowHeight) {
    glGenFramebuffers(1, &gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

    //Generate position color buffer
    glGenTextures(1, &gPosition);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, windowWidth, windowHeight, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

    //Generate normal color buffer
    glGenTextures(1, &gNormal);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, windowWidth, windowHeight, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

    //Generate color/specular buffer
    glGenTextures(1, &gColorSpecular);
    glBindTexture(GL_TEXTURE_2D, gColorSpecular);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, windowWidth, windowHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gColorSpecular, 0);

    constexpr GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, attachments);

    //Attach depth buffer (Render buffer)
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, windowWidth, windowHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        throw std::runtime_error("\nFramebuffer not complete!");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
