#version 460 core

out vec4 fragColors;

in vec2 aTextureCoords;

uniform sampler2D myTextures;

void main() {
	fragColors = texture(myTextures, aTextureCoords);
}	