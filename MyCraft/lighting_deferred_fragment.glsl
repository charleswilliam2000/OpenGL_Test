#version 460 core

out vec4 FragColors;
in vec2 TexCoords;

const vec3 GLOBAL_DIRECTIONAL = vec3(-0.2, -1.0, -0.3);
const vec3 GLOBAL_AMBIENT = vec3(1.0);
const vec3 GLOBAL_DIFFUSE = vec3(0.5);
const vec3 SKY_COLOR = vec3(0.1, 0.8, 1.0);
const float GAMMA = 0.75;

const int POSITION_LAYER = 0;
const int NORMAL_LAYER = 1;
const int COLOR_SPECULAR_LAYER = 2;

layout (std140, binding = 1) uniform ubo_VPMatrices {
    mat4 projection;
    mat4 view;

    mat4 inverseProjection;
    mat4 inverseView;
};
uniform vec3 u_ViewPos;

uniform sampler2DArray gTextureArray;
uniform sampler2D ssaoColorTexture;

vec3 getViewPos(float depth) {
    vec4 clipPos = vec4(TexCoords * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0); 
    vec4 viewPos = inverseProjection * clipPos;
    viewPos /= viewPos.w;
    return viewPos.xyz;
}

vec3 getDirectionalLight(vec3 Normal, float AmbientOcclusion) {
	vec3 lightDir = GLOBAL_DIRECTIONAL;

    float diff = max(dot(Normal, lightDir), 0.0);
    vec3 ambient    = GLOBAL_AMBIENT * AmbientOcclusion;
    vec3 diffuse    = GLOBAL_DIFFUSE * diff;

    return ambient + diffuse;
}

void main()
{
    vec3 FragViewPos = texture(gTextureArray, vec3(TexCoords, POSITION_LAYER)).xyz;
    vec4 FragWorldPos = projection * vec4(FragViewPos, 1.0);
    FragWorldPos.xyz /= FragWorldPos.w;
    FragWorldPos.xyz = FragWorldPos.xyz * 0.5 + 0.5;

    vec3 Normal = texture(gTextureArray, vec3(TexCoords, NORMAL_LAYER)).rgb;
    vec3 Color = texture(gTextureArray, vec3(TexCoords, COLOR_SPECULAR_LAYER)).rgb;
    float AmbientOcclusion = texture(ssaoColorTexture, TexCoords).r;
    
    float notSky = float(FragWorldPos.z >= 0.0); 
    vec3 lighting = vec3(1.0 * AmbientOcclusion);
    vec3 finalColors = pow(mix(SKY_COLOR, lighting, notSky).rgb, vec3(1.0/GAMMA));
    
    FragColors = vec4(finalColors, 1.0);
}