#version 460 core

out vec4 fragColors;

in vec3 FragPos;
in vec2 TextureCoords;
in vec3 Normal;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 cameraPos;

uniform sampler2D myTextures;

void main() {
	float ambientLevel = 0.1;
	vec3 ambient = ambientLevel * lightColor;

	//Diffuse lighting
	vec3 norm = normalize(Normal);
	vec3 lightDirection = normalize(lightPos - FragPos);
	float difference = max(dot(norm, lightDirection), 0.0);
	vec3 diffuse = difference * lightColor;

	//Specular lighting
	float specularLevel = 0.5;
	vec3 viewDir = normalize(cameraPos - lightPos);
	vec3 reflectDir = reflect(-lightDirection, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = specularLevel * spec * lightColor;

	vec3 result = (ambient + diffuse + specular);
	fragColors = texture(myTextures, TextureCoords) * vec4(result, 1.0);
}	