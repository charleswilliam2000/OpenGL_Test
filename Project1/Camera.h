#ifndef CAMERA_H 
#define CAMERA_H

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
	constexpr float SPEED = 2.5f;
	constexpr float SENSITIVITY = 0.1f;
	constexpr float ZOOM = 45.0f;
}

struct Camera {

};


#endif // CAMERA_H