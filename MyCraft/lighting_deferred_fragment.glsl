#version 460 core

out vec4 FragColors;
in vec2 TexCoords;

const vec3 GLOBAL_DIRECTIONAL = vec3(0.2, 1.0, 0.3);
const vec3 GLOBAL_AMBIENT = vec3(1.0);
const vec3 GLOBAL_DIFFUSE = vec3(0.5);
const vec3 SKY_COLOR = vec3(0.1, 0.8, 1.0);
const float GAMMA = 0.75;

const int NORMAL_LAYER = 0;
const int COLOR_SPECULAR_LAYER = 1;

layout (std140, binding = 1) uniform ubo_VPMatrices {
    mat4 projection;
    mat4 view;

    mat4 inverseProjection;
    mat4 inverseView;
};

uniform sampler2DArray gTextureArray;
uniform sampler2D gDepth;
uniform sampler2D ssaoColorTexture;

vec3 getViewPos(float depth) {
    float z = depth * 2.0 - 1.0;
    vec4 clipPos = vec4(TexCoords * 2.0 - 1.0, z, 1.0); 
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

float linearizeDepth(float depth, float zNear, float zFar) {
    float z_normalized = depth * 2.0 -  1.0;
    return 2.0 * zNear * zFar / (zFar + zNear - z_normalized * (zFar - zNear));
}

void main()
{
    float depth             = texture(gDepth, TexCoords).r;
    float AmbientOcclusion  = texture(ssaoColorTexture, TexCoords).r;

    vec3 FragViewPos    = getViewPos(depth);
    vec3 Normal         = texture(gTextureArray, vec3(TexCoords, NORMAL_LAYER)).rgb;
    vec3 Color          = texture(gTextureArray, vec3(TexCoords, COLOR_SPECULAR_LAYER)).rgb;
    
    float not_sky = float(abs(depth - 1.0) > 0.0); 
    vec3 lighting = Color * getDirectionalLight(Normal, AmbientOcclusion);
    //vec3 lighting = vec3(1.0) * getDirectionalLight(Normal, AmbientOcclusion);
    vec3 final_colors = pow(mix(SKY_COLOR, lighting, not_sky).rgb, vec3(1.0/GAMMA));
    
    FragColors = vec4(final_colors, 1.0);
}