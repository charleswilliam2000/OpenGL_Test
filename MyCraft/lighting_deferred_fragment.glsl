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
uniform sampler2D ssao;

vec3 getPos(float depth, vec2 texCoords) {
    vec4 clipPos = vec4(texCoords * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0); // Clip to [-1, 1] normalized range
    vec4 worldPos = invViewProj * clipPos;
    worldPos /= worldPos.w;
    return worldPos.rgb;
}

float linearizeDepth(float depth, float zNear, float zFar) {
    float z = depth * 2.0 - 1.0;
    return (2.0 * zNear * zFar) / (zFar + zNear - depth * ( zFar  - zNear));
}

vec3 calculateDirLight(DirLight light, vec3 FragPos, vec3 normal, vec3 viewDir) {
	vec3 lightDir = -light.direction;

    float diff = max(dot(normal, lightDir), 0.0);
    float AmbientOcclusion = texture(ssao, TexCoords).r;

    vec3 halfwayDir = normalize(lightDir + viewDir);

    vec3 ambient  = light.ambient * AmbientOcclusion;
    vec3 diffuse  = light.diffuse * diff;

    return ambient + diffuse;
}

void main()
{
    float depth = texture(gDepth, TexCoords).r;
    float linearDepth = linearizeDepth(depth, 0.1, 100) / 100.0;
    float notSky = float(depth != 1.0); // If Depth = 1.0 (SKY), this will return 0
    float gamma = 0.75;

    vec3 FragPos = getPos(linearDepth, TexCoords);
    vec3 Normal = texture(gTexArray, vec3(TexCoords, NORMAL_LAYER)).rgb;
    vec3 Diffuse = texture(gTexArray, vec3(TexCoords, COLOR_SPECULAR_LAYER)).rgb;

    vec3 viewDir  = normalize(cameraPos - FragPos);
    vec3 lighting = Diffuse * calculateDirLight(directional_light, FragPos, Normal, viewDir);
    vec3 skyColor = vec3(0.1, 0.8, 1.0);

    vec3 fragColors = pow(mix(skyColor, lighting, notSky).rgb, vec3(1.0/gamma));
    
    FragColors = vec4(fragColors, 1.0);
}