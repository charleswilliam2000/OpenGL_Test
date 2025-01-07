#version 460 core

out vec4 fragColors;

in vec3 FragPos;
in vec2 TextureCoords;
in vec3 Normal;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 objColor;

uniform sampler2D myTextures;

void main() {
	float ambientLevel = 0.1;
	vec3 ambient = ambientLevel * lightColor;

	//Diffuse lighting
	vec3 norm = normalize(Normal);
	vec3 lightDirection = normalize(lightPos - FragPos);
	float difference = max(dot(norm, lightDirection), 0.0);
	vec3 diffuse = difference * lightColor;

	vec3 result = (ambient + diffuse) * objColor;

	fragColors = texture(myTextures, TextureCoords) * vec4(result, 1.0);
}	