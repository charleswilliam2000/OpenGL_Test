#version 460 core

out vec4 fragColors;
in vec3 myColors;

void main() {
	fragColors = vec4(myColors, 1.0);
}