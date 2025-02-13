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

void GeometryBufferObjects::generateBuffers(int windowWidth, int windowHeight) {
    glGenFramebuffers(1, &gFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, gFBO);

    //Generate color buffers (2D TEXTURE ARRAY)
    glGenTextures(1, &gTextArray);
    glBindTexture(GL_TEXTURE_2D_ARRAY, gTextArray);

    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGB16F, windowWidth, windowHeight, 2, 0, GL_RGB, GL_FLOAT, nullptr);

    //Wrapping & flitering
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, gTextArray, 0, 0); // NORMAL
    glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, gTextArray, 0, 1); // COLOR_SPEC
   
    constexpr GLuint attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, attachments);

    //Attach depth buffer 
    glGenTextures(1, &gDepthText);  
    glBindTexture(GL_TEXTURE_2D, gDepthText);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, windowWidth, windowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, gDepthText, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        throw std::runtime_error("\nFramebuffer not complete!");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PostProcessing::SSAO::generateSSAO(int windowWidth, int windowHeight, const void* noiseData) {
    glGenFramebuffers(1, &ssaoFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);

    glGenTextures(1, &ssaoColorText);
    glBindTexture(GL_TEXTURE_2D, ssaoColorText);
   
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, windowWidth, windowHeight, 0, GL_RED, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorText, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        throw std::runtime_error("\nFramebuffer not complete!");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glGenFramebuffers(1, &ssaoBlurFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);

    glGenTextures(1, &ssaoBlurText);
    glBindTexture(GL_TEXTURE_2D, ssaoBlurText);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, windowWidth, windowHeight, 0, GL_RED, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoBlurText, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        throw std::runtime_error("\nFramebuffer not complete!");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glGenTextures(1, &ssaoNoise);
    glBindTexture(GL_TEXTURE_2D, ssaoNoise);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 4, 4, 0, GL_RGB, GL_FLOAT, noiseData);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}
