#ifndef TEXTURE_H
#define TEXTURE_H

#include "stdafx.h"
#include "stb_image.h"

#include <algorithm>

struct Img_Configs {
	unsigned char* _data{};
	int _width{}, _height{}, _num_channels{};
};

struct Skybox {
	uint32_t _skyboxID = 0;

	Skybox();
};

namespace Texture_Methods {
	Img_Configs loadTexture(const char* image_path);
	void activateTexture(const uint32_t& textureID, GLenum textureUnit = GL_TEXTURE0);
}

struct Texture {
	uint32_t _textureID = 0;
	Texture() {}
	Texture(const char* image_path);
};

#endif