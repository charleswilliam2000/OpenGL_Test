#version 460 core

out vec4 FragColors;
in vec2 TexCoords;

uniform sampler2D lightingText;
uniform sampler2D gDepth;

void main() {
	float depth = texture(gDepth, TexCoords).r;

	bool isBackground = (depth == 0.0);
	if (isBackground){
		FragColors = vec4(vec3(0.1, 0.8, 1), 1.0);
	}
	else {
		FragColors = texture(lightingText, TexCoords);
	}
}