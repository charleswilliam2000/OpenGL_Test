#version 460 core

out vec4 FragColors;
in vec2 TexCoords;

struct DirLight {
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

uniform DirLight directional_light;
uniform vec3 cameraPos;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gColorSpecular;

vec3 calculateDirLight(DirLight light, vec3 normal, vec3 viewDir) 
{
	vec3 lightDir = -light.direction;

    float diff = max(dot(normal, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);

    vec3 ambient  = light.ambient;
    vec3 diffuse  = light.diffuse  * diff;
    vec3 specular = light.specular * spec;

    return (ambient + diffuse + specular);
}

void main()
{
    // retrieve data from gbuffer
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec4 colorSpec = texture(gColorSpecular, TexCoords);
    vec3 Diffuse = colorSpec.rgb;
    float Specular = colorSpec.a;
    
    // then calculate lighting as usual
    vec3 lighting  = Diffuse * 0.1; // hard-coded ambient component
    vec3 viewDir  = normalize(cameraPos - FragPos);
    lighting += calculateDirLight(directional_light, Normal, viewDir);

    FragColors = vec4(Normal, 1.0);
}