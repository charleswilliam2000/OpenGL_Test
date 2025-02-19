#include "RenderStructures.h"

void SSAO::generateSSAO(int windowWidth, int windowHeight) {
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

    _ssaoKernelsUBO.generateBuffers(
        STORAGE_TYPE::GL_BUFFER_DATA_STATIC_DRAW,
        ssaoKernels.size() * sizeof(float_VEC), CONSTANTS::SSAO_KERNELS_BINDING_POINT, ssaoKernels.data()
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

void SSAO::bindToShader(GLuint shaderProgram) {
    _ssaoKernelsUBO.bindToShader(shaderProgram, "ubo_Kernels");
}
void ModelMatrices::addMatrix(const glm::mat4& modelMatrix) {
    _modelMatrices.push_back(modelMatrix);
}

void ModelMatrices::createUBO(STORAGE_TYPE type, GLuint bindingPoint) {
    if (_modelMatrices.empty()) throw std::runtime_error("\nModel Matrices are empty");
    else _modelUBO.generateBuffers(type, _modelMatrices.size() * sizeof(glm::mat4), bindingPoint, _modelMatrices.data());
}

void ModelMatrices::bindToShader(GLuint shaderProgram) {
    _modelUBO.bindToShader(shaderProgram, "ubo_ModelMatrices");
}

void ViewProjectionMatrices::bindToShader(GLuint shaderProgram) {
    _viewProjectionUBO.bindToShader(shaderProgram, "ubo_ViewProjectionMatrices");
}

void ViewProjectionMatrices::createUBO(STORAGE_TYPE type, GLuint bindingPoint) {
    _viewProjectionUBO.generateBuffers(type, sizeof(ViewProjectionMatrices) - sizeof(UniformBufferObjects), bindingPoint, nullptr);
}

void ViewProjectionMatrices::updateViewProjection(const Camera& camera, const GLuint& shaderProgram) {
    _projection = glm::perspective(glm::radians(45.0f), (float)CONSTANTS::WINDOW_WIDTH / (float)CONSTANTS::WINDOW_HEIGHT, 0.1f, 100.0f);
    _view = camera.updateCameraView();
    _inverseProjection = glm::inverse(_projection);
    _inverseView = glm::inverse(_view);

    bindToShader(shaderProgram);

    switch (_viewProjectionUBO.storageType) {

    case STORAGE_TYPE::GL_BUFFER_STORAGE_COHERENT:
        static_cast<glm::mat4*>(_viewProjectionUBO.persistentPtr)[0] = _projection;
        static_cast<glm::mat4*>(_viewProjectionUBO.persistentPtr)[1] = _view;
        static_cast<glm::mat4*>(_viewProjectionUBO.persistentPtr)[2] = _inverseProjection;
        static_cast<glm::mat4*>(_viewProjectionUBO.persistentPtr)[3] = _inverseView;
        break;

    case STORAGE_TYPE::GL_BUFFER_STORAGE_INCOHERENT:
        static_cast<glm::mat4*>(_viewProjectionUBO.persistentPtr)[0] = _projection;
        static_cast<glm::mat4*>(_viewProjectionUBO.persistentPtr)[1] = _view;
        static_cast<glm::mat4*>(_viewProjectionUBO.persistentPtr)[2] = _inverseProjection;
        static_cast<glm::mat4*>(_viewProjectionUBO.persistentPtr)[3] = _inverseView;
        glMemoryBarrier(GL_UNIFORM_BARRIER_BIT);
        break;
    default:
        throw std::runtime_error("\nStorage type not supported");
    }
}
