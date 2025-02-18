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

void UniformBufferObjects::generateBuffers(STORAGE_TYPE type, size_t size, GLuint bindingPoint, const void* data) {
    this->bindingPoint = bindingPoint;
    this->bufferSize = static_cast<GLuint>(size);

    glGenBuffers(1, &handle);
    glBindBuffer(GL_UNIFORM_BUFFER, handle);
    glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, handle);

    switch (type) {

    case STORAGE_TYPE::GL_BUFFER_DATA_STATIC_DRAW:

        glBufferData(GL_UNIFORM_BUFFER, bufferSize, data, GL_STATIC_DRAW);
        break;

    case STORAGE_TYPE::GL_BUFFER_DATA_DYNAMIC_DRAW:

        glBufferData(GL_UNIFORM_BUFFER, bufferSize, data, GL_DYNAMIC_DRAW);
        break;

    case STORAGE_TYPE::GL_BUFFER_STORAGE_COHERENT:

        glBufferStorage(GL_UNIFORM_BUFFER, bufferSize, data, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
        persistentPtr = glMapBufferRange(GL_UNIFORM_BUFFER, 0, bufferSize, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);

        if (!persistentPtr) {
            throw std::runtime_error("\nUnable to initialize persistent ptr");
        }
        else break;

    case STORAGE_TYPE::GL_BUFFER_STORAGE_INCOHERENT:

        glBufferStorage(GL_UNIFORM_BUFFER, bufferSize, data, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT);
        persistentPtr = glMapBufferRange(GL_UNIFORM_BUFFER, 0, bufferSize, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT);

        if (!persistentPtr) {
            throw std::runtime_error("\nUnable to initialize persistent ptr");
        }

        break;
    default:
        throw std::runtime_error("\nUnable to determine storage type");
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
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, windowWidth, windowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, gDepthText, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        throw std::runtime_error("\nFramebuffer not complete!");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PostProcessing::SSAO::generateSSAO(int windowWidth, int windowHeight) {
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

    std::uniform_real_distribution<float> genRandomFloat(0.0f, 1.0f);
    std::default_random_engine generator;

    constexpr auto lerp = [&](float a, float b, float z) -> float {
        return a + z * (b - a);
        };

    std::array<float_VEC, 64> ssaoKernels{}; // SAMPLING 64 KERNELS
    for (int i = 0; i < ssaoKernels.size(); i++) {
        glm::vec3 sample(genRandomFloat(generator) * 2.0f - 1.0f, genRandomFloat(generator) * 2.0f - 1.0f, genRandomFloat(generator));
        sample = glm::normalize(sample);
        sample *= genRandomFloat(generator);

        float scale = static_cast<float>(i) / 64.0f;
        scale = lerp(0.1f, 1.0f, scale * scale);
        sample *= scale;

        ssaoKernels[i] = sample;
    }

    constexpr size_t ssaoKernelsUBObindingPoint = 2;

    ssaoKernelsUBO.generateBuffers(
        STORAGE_TYPE::GL_BUFFER_DATA_STATIC_DRAW, 
        ssaoKernels.size() * sizeof(float_VEC), ssaoKernelsUBObindingPoint, ssaoKernels.data()
    );

    std::array<float_VEC, 16> ssaoNoises{};
    for (int i = 0; i < 16; i++)
    {
        glm::vec3 noise(genRandomFloat(generator) * 2.0 - 1.0, genRandomFloat(generator) * 2.0 - 1.0, 0.0f);
        ssaoNoises[i] = noise;
    }

    glGenTextures(1, &ssaoNoise);
    glBindTexture(GL_TEXTURE_2D, ssaoNoise);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 4, 4, 0, GL_RGB, GL_FLOAT, ssaoNoises.data());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}
