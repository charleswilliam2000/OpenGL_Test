#version 460 core

layout (location = 0) in vec3 aPos;

uniform mat4 projection;
uniform mat4 view;

out vec4 vertexColors;

void main() {
	vec4 pos = projection * view * vec4(aPos, 1.0);
	vertexColors = vec4(vec3(0.1, 0.8, 1), 1.0);
	gl_Position = pos.xyww;
}