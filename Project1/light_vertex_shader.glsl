#version 460 core

layout (location = 0) in vec3 aPos;
layout (std140, binding = 1) uniform VP_Matrices {
	mat4 projectionMatrix;
	mat4 viewMatrix;
};
uniform mat4 model;

void main() {
	gl_Position = projectionMatrix * viewMatrix * model * vec4(aPos, 1.0);
}