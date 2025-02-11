#ifndef CAMERA_H 
#define CAMERA_H

#include "stdafx.h"

namespace Default_Camera_Values {
	constexpr float SPEED = 4.5f;
	constexpr float SENSITIVITY = 0.05f;
	//float ZOOM = 45.0f;
}

struct FrameValues {
	float deltaTime{};
	float lastFrame{};
};

enum class CameraVectors : uint8_t {
	POS = 0,
	FRONT = 1,
	UP = 2,
	RIGHT = 3
};

struct CameraVECs {
	float_VEC cameraPos{};
	float_VEC cameraFront{};
	float_VEC cameraUp{};
};

class Camera {
private:
	float _yaw	= -90.0f;
	float _pitch = 0.0f;
	CameraVECs _vectors{};
	FrameValues _frame{};
	
	float_VEC _right{};
	float_VEC _worldUp{};

	void updateCameraVECs();

public:
	Camera() {}
	Camera(CameraVECs in_CameraVECs);

	float_VEC getVector(const CameraVectors& vec) const;

	void updateFrame();
	glm::mat4 updateCameraView() const;
	void handleCameraMovement(GLFWwindow* window);
	void handleMouseMovement(float xOffset, float yOffset, GLboolean constrainPitch = true);


	const float aspect = static_cast<float>(CONSTANTS::WINDOW_WIDTH) / static_cast<float>(CONSTANTS::WINDOW_HEIGHT);
	const float fovY = glm::radians(45.0f);
	const float zNear = 0.1f;
	const float zFar = 100.0f;
};


#endif // CAMERA_H