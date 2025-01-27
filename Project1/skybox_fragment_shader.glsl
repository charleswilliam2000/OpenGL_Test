#version 460 core

in vec3 texCoords;

out vec4 fragColors;

uniform samplerCube skybox;

void main() {
	fragColors = texture(skybox, texCoords);
}
