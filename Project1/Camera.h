#ifndef CAMERA_H 
#define CAMERA_H

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


namespace Default_Camera_Values {
	constexpr float SPEED = 1.5f;
	constexpr float SENSITIVITY = 0.05f;
	//float ZOOM = 45.0f;
}

struct FrameValues {
	float deltaTime{};
	float lastFrame{};
};

struct CameraVECs {
	glm::vec3 cameraPos{};
	glm::vec3 cameraFront{};
	glm::vec3 cameraUp{};
};

class Camera {
private:
	float _yaw	= -90.0f;
	float _pitch = 0.0f;
	CameraVECs _vectors{};
	FrameValues _frame{};
	
	glm::vec3 _right{};
	glm::vec3 _worldUp{};

	void updateCameraVECs();

public:
	Camera() {}
	Camera(CameraVECs in_CameraVECs);

	void updateFrame();
	glm::mat4 updateCameraView() const;
	void handleCameraMovement(GLFWwindow* window);
	void handleMouseMovement(float xOffset, float yOffset, GLboolean constrainPitch = true);
};


#endif // CAMERA_H