#version 460 core

out vec4 fragColors;

in vec3 myColors;
in vec2 aTextureCoords;

uniform sampler2D myTextures;

void main() {
	fragColors = texture(myTextures, aTextureCoords) * vec4(myColors, 1.0);
}	