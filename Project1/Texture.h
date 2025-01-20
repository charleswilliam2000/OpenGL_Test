#ifndef TEXTURE_H
#define TEXTURE_H

#include <vector>
#include <algorithm>

#include "stdafx.h"
#include "stb_image.h"

struct Texture_Configs {
	unsigned char* _data{};
	int _width{}, _height{}, _num_channels{};
};

namespace Texture_Methods {
	Texture_Configs loadTexture(const char* image_path);
	void activateTexture(const uint32_t& textureID, GLenum textureUnit = GL_TEXTURE0);
}

struct Texture {
	uint32_t _textureID;
	Texture(const char* image_path);
};
#endif