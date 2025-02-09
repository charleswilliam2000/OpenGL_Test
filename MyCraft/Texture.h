#ifndef TEXTURE_H
#define TEXTURE_H

#include "stdafx.h"
#include "stb_image.h"

#include <algorithm>

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