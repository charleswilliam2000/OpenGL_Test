#include "RenderStructures.h"

void SSAO::generateSSAO(int windowWidth, int windowHeight) {
    glGenFramebuffers(1, &ssaoFBO);     
    glGenFramebuffers(1, &ssaoBlurFBO);

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

    static std::uniform_real_distribution<float> genRandomFloat(0.0f, 1.0f);
    static std::default_random_engine generator;

    constexpr auto lerp = [&](float a, float b, float z) -> float {
        return a + z * (b - a);
        };

    std::array<float_VEC, 32> ssaoKernels{}; // SAMPLING 64 KERNELS
    for (int i = 0; i < ssaoKernels.size(); i++) {
        glm::vec3 sample = { genRandomFloat(generator) * 2.0f - 1.0f, genRandomFloat(generator) * 2.0f - 1.0f, genRandomFloat(generator) };
        sample = glm::normalize(sample);
        sample *= genRandomFloat(generator);

        float scale = static_cast<float>(i) / static_cast<float>(ssaoKernels.size());
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
        glm::vec3 noise = { genRandomFloat(generator) * 2.0 - 1.0, genRandomFloat(generator) * 2.0 - 1.0, 0.0f };
        ssaoNoises[i] = noise;
    }

    glGenTextures(1, &ssaoNoise);
    glBindTexture(GL_TEXTURE_2D, ssaoNoise);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 4, 4, 0, GL_RGBA, GL_FLOAT, ssaoNoises.data());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void SSAO::bindToShader(GLuint shaderProgram) {
    _ssaoKernelsUBO.bindToShader(shaderProgram, "ubo_Kernels");
}
const std::vector<glm::mat4>& ModelMatrices::getModelMatrices() const {
    return _modelMatrices;
}
void ModelMatrices::addMatrix(const glm::mat4& modelMatrix) {
    _modelMatrices.push_back(modelMatrix);
}

void ModelMatrices::createUBO(STORAGE_TYPE modelUBOType, GLuint modelUBOBindingPoint, STORAGE_TYPE modelNormalMapUBOType, GLuint modelNormalMapUBOBindingPoint) {
    if (_modelMatrices.empty()) throw std::runtime_error("\nModel Matrices are empty");
    else {
        _modelUBO.generateBuffers(modelUBOType, _modelMatrices.size() * sizeof(glm::mat4), modelUBOBindingPoint, _modelMatrices.data());
        _modelNormalMapUBO.generateBuffers(modelNormalMapUBOType, _modelMatrices.size() * sizeof(glm::mat4), modelNormalMapUBOBindingPoint, nullptr);
    }
}

void ModelMatrices::bindToShader(const glm::mat4& viewMatrix, const GLuint& shaderProgram) const {
    _modelUBO.bindToShader(shaderProgram, "ubo_ModelMatrices");
    _modelNormalMapUBO.bindToShader(shaderProgram, "ubo_NormalMatrices");

    switch (_modelNormalMapUBO.storageType) {
    case STORAGE_TYPE::GL_BUFFER_STORAGE_INCOHERENT:
        for (size_t i = 0; i < _modelMatrices.size(); i++) {
            static_cast<glm::mat4*>(_modelNormalMapUBO.persistentPtr)[i] = glm::transpose(glm::inverse(glm::mat3(viewMatrix * _modelMatrices[i])));
        }
        glMemoryBarrier(GL_UNIFORM_BARRIER_BIT);
        break;
    default:
        throw std::runtime_error("\nStorage type not supported");
    }
}

void ViewProjectionMatrices::bindToShader(GLuint shaderProgram) const {
    _viewProjectionUBO.bindToShader(shaderProgram, "ubo_ViewProjectionMatrices");
}

void ViewProjectionMatrices::createUBO(STORAGE_TYPE type, GLuint bindingPoint) {
    _viewProjectionUBO.generateBuffers(type, 4 * sizeof(glm::mat4), bindingPoint, nullptr);
}

void ViewProjectionMatrices::updateViewProjection(const glm::mat4& viewMatrix, const GLuint& shaderProgram) {
    auto projection = glm::perspective(glm::radians(45.0f), (float)CONSTANTS::WINDOW_WIDTH / (float)CONSTANTS::WINDOW_HEIGHT, 0.1f, 100.0f);
    auto view = viewMatrix;
    auto inverseProjection = glm::inverse(projection);
    auto inverseView = glm::inverse(view);

    bindToShader(shaderProgram);

    switch (_viewProjectionUBO.storageType) {

    case STORAGE_TYPE::GL_BUFFER_STORAGE_COHERENT:
        static_cast<glm::mat4*>(_viewProjectionUBO.persistentPtr)[0] = projection;
        static_cast<glm::mat4*>(_viewProjectionUBO.persistentPtr)[1] = view;
        static_cast<glm::mat4*>(_viewProjectionUBO.persistentPtr)[2] = inverseProjection;
        static_cast<glm::mat4*>(_viewProjectionUBO.persistentPtr)[3] = inverseView;
        break;

    case STORAGE_TYPE::GL_BUFFER_STORAGE_INCOHERENT:
        static_cast<glm::mat4*>(_viewProjectionUBO.persistentPtr)[0] = projection;
        static_cast<glm::mat4*>(_viewProjectionUBO.persistentPtr)[1] = view;
        static_cast<glm::mat4*>(_viewProjectionUBO.persistentPtr)[2] = inverseProjection;
        static_cast<glm::mat4*>(_viewProjectionUBO.persistentPtr)[3] = inverseView;
        glMemoryBarrier(GL_UNIFORM_BARRIER_BIT);
        break;
    default:
        throw std::runtime_error("\nStorage type not supported");
    }
}
