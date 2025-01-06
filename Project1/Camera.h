#ifndef CAMERA_H 
#define CAMERA_H

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

enum class CAMERA_MOVEMENT : unsigned char {
	FORWARD = 0,
	BACKWARD = 1,
	LEFT = 2,
	RIGHT = 3,
};

namespace Default_Camera_Values {
	constexpr float YAW = -90.0f;
	constexpr float PITCH = 0.0f;
	constexpr float SPEED = 1.5f;
	constexpr float SENSITIVITY = 0.1f;
	constexpr float ZOOM = 45.0f;
}

struct FrameValues {
	float deltaTime;
	float lastFrame;
};

struct CameraVECs {
	glm::vec3 cameraPos;
	glm::vec3 cameraFront;
	glm::vec3 cameraUp;
};

struct Camera {
	CameraVECs _vectors{};
	FrameValues _frame{};

	Camera() {}
	Camera(CameraVECs in_CameraVECs) : _vectors(in_CameraVECs), _frame({0.0f, 0.0f}) {}
};

namespace Camera_Methods {
	void updateFrame(FrameValues& frame);
	glm::mat4 updateCameraView(const CameraVECs& vectors);
	void handleMovement(Camera& camera, GLFWwindow* window);
}

#endif // CAMERA_H