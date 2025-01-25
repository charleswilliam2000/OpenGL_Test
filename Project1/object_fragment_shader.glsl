#version 460 core

out vec4 fragColors;

struct DirLight {
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct PointLight {
	vec3 position;

	float constant;
    float linear;
    float quadratic;  

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 FragPos;
in vec2 TextureCoords;
in vec3 Normal;
in float layerIndex;

uniform DirLight directional_light;

#define NUM_POINT_LIGHTS 1
uniform PointLight point_light[NUM_POINT_LIGHTS];

uniform vec3 cameraPos;
uniform sampler2DArray myTextures;

vec3 calculateDirLight(DirLight light, vec3 normal, vec3 viewDir) 
{
	vec3 lightDir = normalize(-light.direction);

    float diff = max(dot(normal, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);

    vec3 ambient  = light.ambient;
    vec3 diffuse  = light.diffuse  * diff;
    vec3 specular = light.specular * spec;

    return (ambient + diffuse + specular);
}

vec3 calculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) 
{
	vec3 lightDir = normalize(light.position - fragPos);
	float diff = max(dot(normal, lightDir), 0.0);

	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	vec3 ambient = light.ambient;
	vec3 diffuse = light.diffuse * diff;
	vec3 specular = light.ambient * spec;

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	return (ambient + diffuse + specular);
}

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(cameraPos - FragPos);

    vec3 result = calculateDirLight(directional_light, norm, viewDir);
    for(int i = 0; i < NUM_POINT_LIGHTS; i++)
        result += calculatePointLight(point_light[i], norm, FragPos, viewDir);    
    
    fragColors = texture(myTextures, vec3(TextureCoords, layerIndex)) * vec4(result, 1.0);
}