#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#include <glfw/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <algorithm>
#include <vector>

#include "stb_image.h"

struct Img_Configs {
	unsigned char* _data{};
	int _width{}, _height{}, _num_channels{};
};

namespace Texture_Methods {
	Img_Configs loadTexture(const char* image_path);
}

struct Texture {
	uint32_t textureID = 0;
	Texture() {}
	Texture(const char* image_path);
};

#endif