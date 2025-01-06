#include "Camera.h"
namespace Camera_Methods {
    void updateFrame(FrameValues& frame) {
        float currentFrame = static_cast<float>(glfwGetTime());
        frame.deltaTime = currentFrame - frame.lastFrame;
        frame.lastFrame = currentFrame;
    }

    glm::mat4 updateCameraView(const CameraVECs& vectors) {
        return glm::lookAt(vectors.cameraPos, vectors.cameraPos + vectors.cameraFront, vectors.cameraUp);
    }

    void handleMovement(Camera& camera, GLFWwindow* window) {
        auto cameraSpeed = Default_Camera_Values::SPEED * camera._frame.deltaTime;

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            camera._vectors.cameraPos += cameraSpeed * camera._vectors.cameraFront;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            camera._vectors.cameraPos -= cameraSpeed * camera._vectors.cameraFront;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            camera._vectors.cameraPos -= glm::normalize(glm::cross(camera._vectors.cameraFront, camera._vectors.cameraUp)) * cameraSpeed;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            camera._vectors.cameraPos += glm::normalize(glm::cross(camera._vectors.cameraFront, camera._vectors.cameraUp)) * cameraSpeed;
    }
}