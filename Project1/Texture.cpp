#include "Texture.h"

namespace Texture_Methods {
	Texture_Configs loadTexture(const char* image_path) {
		int width{}, height{}, num_channels{};
		unsigned char* image_data = stbi_load(image_path, &width, &height, &num_channels, 0);
	
		return { image_data, width, height, num_channels };
	}
}

Texture::Texture(const char* image_path)
{
	glGenTextures(NUM_TEXTURES, &_textureID);
	glBindTexture(GL_TEXTURE_2D, _textureID);

	//Wrapping & flitering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	Texture_Configs texture = Texture_Methods::loadTexture(image_path);
	if (texture._data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture._width, texture._height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture._data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else throw std::runtime_error("Unable to load texture!\n");
	stbi_image_free(texture._data);
}