#version 460 core

out vec4 fragColors;

uniform vec3 wireframeColors;

void main() {
	fragColors = vec4(wireframeColors, 1.0f);
}