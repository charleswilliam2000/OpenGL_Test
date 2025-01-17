#version 460 core

out vec4 fragColors;

struct Light {
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};


in vec3 FragPos;
in vec2 TextureCoords;
in vec3 Normal;

uniform Light light;
uniform vec3 lightColor;
uniform vec3 cameraPos;

uniform sampler2D myTextures;

void main() {
	vec3 ambient = light.ambient * lightColor;

	//Diffuse lighting
	vec3 norm = normalize(Normal);
	vec3 lightDirection = normalize(-light.direction);
	float difference = max(dot(norm, lightDirection), 0.0);
	vec3 diffuse = light.diffuse * difference * lightColor;

	//Specular lighting
	vec3 viewDir = normalize(cameraPos - FragPos);
	vec3 reflectDir = reflect(-lightDirection, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = light.specular * spec * lightColor;

	vec3 result = (ambient + diffuse + specular);
	fragColors = texture(myTextures, TextureCoords) * vec4(result, 1.0);
}	