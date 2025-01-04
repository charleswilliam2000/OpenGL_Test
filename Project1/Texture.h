#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#include <iostream>

#include "stb_image.h"

constexpr unsigned char NUM_TEXTURES = 1;

struct Texture_Configs {
	unsigned char* _data{};
	int _width{}, _height{}, _num_channels{};
};

namespace Texture_Methods {
	Texture_Configs loadTexture(const char* image_path);
}

struct Texture {
	uint32_t _textureID;
	Texture(const char* image_path);
};
#endif