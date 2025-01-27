#include "Texture.h"

namespace Texture_Methods {
	Img_Configs loadTexture(const char* image_path) {
		int width{}, height{}, num_channels{};
		unsigned char* image_data = stbi_load(image_path, &width, &height, &num_channels, 0);
	
		return { image_data, width, height, num_channels };
	}
	void activateTexture(const uint32_t& textureID, GLenum textureUnit) {
		glActiveTexture(textureUnit); 
		glBindTexture(GL_TEXTURE_2D_ARRAY, textureID);
	}
}

Texture::Texture(const char* image_path)
{
	glGenTextures(1, &_textureID);
	glBindTexture(GL_TEXTURE_2D_ARRAY, _textureID);

	//Wrapping & flitering
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	Img_Configs texture = Texture_Methods::loadTexture(image_path);
	if (texture._data) {
		int tileW = 400, tileH = 400; // Pixel for each row/column of a tile
		int channels = texture._num_channels; //RGB

		int tilesX = texture._width / tileW, tilesY = texture._height / tileH; 
		int imageCount = tilesX * tilesY;
		glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, (channels == 4) ? GL_RGBA : GL_RGB, tileW, tileH, imageCount, 0, (channels == 4) ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, nullptr);
		
		std::vector<unsigned char> tile(tileW * tileH * channels);
		int tileSizeX = tileW * channels;
		int rowLen = tilesX * tileSizeX;

		for (int iy = 0; iy < tilesY; iy++) {
			for (int ix = 0; ix < tilesX; ix++) {
				unsigned char* ptr = texture._data + iy * tileH * rowLen + ix * tileSizeX;
				
				for (int row = 0; row < tileH; ++row)
					std::copy(ptr + row * rowLen, ptr + row * rowLen + tileSizeX,
						tile.begin() + row * tileSizeX);

				int i = iy * tilesX + ix;
				glTexSubImage3D(
					GL_TEXTURE_2D_ARRAY, 
					0, 0, 0, 
					i,
					tileW, tileH, 1,
					(channels == 4) ? GL_RGBA : GL_RGB,
					GL_UNSIGNED_BYTE, 
					tile.data()
				);
			}
		}
	}
	else throw std::runtime_error("Unable to load texture!\n");
	stbi_image_free(texture._data);
}

Skybox::Skybox()
{
	std::array<const char*, 6> facesCubeMap = { {
			"skybox_right.jpg",
			"skybox_left.jpg",
			"skybox_top.jpg",
			"skybox_bottom.jpg",
			"skybox_front.jpg",
			"skybox_back.jpg",
	} };

	glGenTextures(1, &_skyboxID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, _skyboxID);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	constexpr size_t numFaces = facesCubeMap.size();
	Img_Configs skybox{};
	for (size_t i = 0; i < numFaces; i++) {
		skybox = Texture_Methods::loadTexture(facesCubeMap[i]);
		if (skybox._data) {
			stbi_set_flip_vertically_on_load(false);
			glTexImage2D(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0,
				GL_RGB,
				skybox._width,
				skybox._height,
				0,
				GL_RGB,
				GL_UNSIGNED_BYTE,
				skybox._data
			);
			stbi_image_free(skybox._data);
		}
		else throw std::runtime_error("\nUnable to initialize skybox texture!");
	}

}
