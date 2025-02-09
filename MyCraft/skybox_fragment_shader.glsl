#version 460 core

out vec4 fragColors;
in vec4 vertexColors;

void main() {
	fragColors = vertexColors;
}
