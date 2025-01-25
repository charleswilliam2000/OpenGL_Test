#include "Camera.h"
void Camera::updateCameraVECs()
{
    float_VEC front = { 0.0f, 0.0f, 0.0f };
    front.x = cos(glm::radians(_yaw)) * cos(glm::radians(_pitch));
    front.y = sin(glm::radians(_pitch));
    front.z = sin(glm::radians(_yaw)) * cos(glm::radians(_pitch));

    _vectors.cameraFront    = glm::normalize(front);
    _right                  = glm::normalize(glm::cross(_vectors.cameraFront, _worldUp));
    _vectors.cameraUp       = glm::normalize(glm::cross(_right, _vectors.cameraFront));
}
Camera::Camera(CameraVECs in_CameraVECs) : _vectors(in_CameraVECs), _worldUp(in_CameraVECs.cameraUp), _frame({ 0.0f, 0.0f })
{
    updateCameraVECs();
}

float_VEC Camera::getVector(const CameraVectors& vec) const {
    switch (vec) {
    case CameraVectors::POS:
        return _vectors.cameraPos;
    case CameraVectors::FRONT:
        return _vectors.cameraFront;
    case CameraVectors::UP:
        return _vectors.cameraUp;
    case CameraVectors::RIGHT:
        return _right;
    default:
        throw std::runtime_error("\nUnable to deduce camera vector");
    }
}

void Camera::updateFrame() {
    float currentFrame = static_cast<float>(glfwGetTime());
    _frame.deltaTime = currentFrame - _frame.lastFrame;
    _frame.lastFrame = currentFrame;
}

glm::mat4 Camera::updateCameraView() const {
    return glm::lookAt(_vectors.cameraPos, _vectors.cameraPos + _vectors.cameraFront, _vectors.cameraUp);
}

void Camera::handleCameraMovement(GLFWwindow* window) {
    float velocity = Default_Camera_Values::SPEED * _frame.deltaTime;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        _vectors.cameraPos += velocity * _vectors.cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        _vectors.cameraPos -= velocity * _vectors.cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        _vectors.cameraPos -= _right * velocity;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        _vectors.cameraPos += _right * velocity;

}

void Camera::handleMouseMovement(float xOffset, float yOffset, GLboolean constrainPitch) {
   
    xOffset *= Default_Camera_Values::SENSITIVITY;
    yOffset *= Default_Camera_Values::SENSITIVITY;

    _yaw += xOffset;
    _pitch += yOffset;

    if (constrainPitch) {
        if (_pitch > 89.0f) _pitch = 89.0f;
        if (_pitch < -89.0f) _pitch = -89.0f;
    }

    updateCameraVECs();
}
