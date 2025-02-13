#version 460 core

out vec4 FragColors;
in vec2 TexCoords;

struct DirLight {
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

const int NORMAL_LAYER = 0;
const int COLOR_SPECULAR_LAYER = 1;

uniform DirLight directional_light;
uniform vec3 cameraPos;
uniform mat4 invViewProj;

uniform sampler2DArray gTexArray;
uniform sampler2D gDepth;

vec3 getPos(float depth, vec2 texCoords) {
    vec4 clipPos = vec4(texCoords * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0); // Clip to [-1, 1] normalized range
    vec4 worldPos = invViewProj * clipPos;
    worldPos /= worldPos.w;
    return worldPos.xyz;
}

vec3 calculateDirLight(DirLight light, vec3 FragPos, vec3 normal, vec3 viewDir) 
{
	vec3 lightDir = -light.direction;

    float diff = max(dot(normal, lightDir), 0.0);

    vec3 halfwayDir = normalize(lightDir + viewDir);

    vec3 ambient  = light.ambient;
    vec3 diffuse  = light.diffuse  * diff;

    return (ambient + diffuse);
}

void main()
{
    float depth = texture(gDepth, TexCoords).r;
    float notSky = float(abs(depth - 1.0) > 0.0001); // If Depth = 1.0, this will return 0
    float gamma = 0.7;

    vec3 FragPos = getPos(depth, TexCoords);
    vec3 Normal = texture(gTexArray, vec3(TexCoords, NORMAL_LAYER)).rgb;
    vec3 Diffuse = texture(gTexArray, vec3(TexCoords, COLOR_SPECULAR_LAYER)).rgb;
    
    vec3 viewDir  = normalize(cameraPos - FragPos);
    vec3 lighting = Diffuse * calculateDirLight(directional_light, FragPos, Normal, viewDir);
    vec3 skyColor = vec3(0.1, 0.8, 1.0);

    vec3 fragColors = mix(skyColor, lighting, notSky);
    
    FragColors = vec4(pow(fragColors.rgb, vec3(1.0/gamma)), 1.0);
}