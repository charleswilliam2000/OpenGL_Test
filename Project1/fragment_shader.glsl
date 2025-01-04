#version 460 core

out vec4 fragColors;

in vec3 myColors;
in vec2 aTextCoords;

uniform sampler2D myTextures;

void main() {
	fragColors = texture(myTextures, aTextCoords) * vec4(myColors, 1.0);
}