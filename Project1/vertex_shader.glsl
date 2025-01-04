#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColors;
layout (location = 2) in vec2 aTextures;

out vec3 myColors;
out vec2 aTextCoords;

void main() {
	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
	myColors = aColors;
	aTextCoords = aTextures;
}