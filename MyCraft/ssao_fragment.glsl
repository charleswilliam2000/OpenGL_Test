#version 460 core

out float FragColors;
in vec2 TexCoords;

const int NUM_KERNELS = 64;
const int NORMAL_LAYER = 0;

layout (std140, binding = 1) uniform ubo_VPMatrices {
	mat4 projection;
	mat4 view;

    mat4 inverseProjection;
    mat4 inverseView;
};

layout(std140, binding = 3) uniform ubo_Kernels {
	vec3 kernels[NUM_KERNELS];
};

uniform sampler2DArray gTextureArray;
uniform sampler2D gDepth;
uniform sampler2D ssaoNoise;
uniform vec2 screenSize;

const float RADIUS = 1.0;
const float BIAS = 0.1;

const vec2 NOISE_SCALE = vec2(screenSize.x / 4.0, screenSize.y / 4.0);

vec3 getViewPos(float depth) {  
    float clipZ = depth * 2.0 - 1.0;
    vec4 clipSpacePos = vec4((TexCoords * 2.0 - 1.0), clipZ, 1.0);
    vec4 viewSpacePos = inverseProjection * clipSpacePos;
    
    viewSpacePos /= viewSpacePos.w;
    
    return viewSpacePos.xyz;
}

void main() {
    vec3 FragViewPos = getViewPos(texture(gDepth, TexCoords).r);
    vec3 FragWorldPos = vec3(inverseView * vec4(FragViewPos, 1.0)).xyz;

    vec3 Normal = normalize(texture(gTextureArray, vec3(TexCoords, NORMAL_LAYER)).rgb);
    vec3 randomVec = normalize(texture(ssaoNoise, TexCoords * NOISE_SCALE).xyz);

    vec3 Tangent = normalize(randomVec - Normal * dot(randomVec, Normal));
    vec3 Bitangent = cross(Normal, Tangent);
    mat3 TBN = mat3(Tangent, Bitangent, Normal);

    float occlusion = 0.0;
    for(int i = 0; i < NUM_KERNELS; i++)
    {
        vec3 samplePos = TBN * kernels[i].xyz;
        //samplePos = FragViewPos + samplePos * RADIUS;
        samplePos = FragWorldPos + samplePos * RADIUS; 
        samplePos = vec3(view * vec4(samplePos, 1.0)).xyz;

        vec4 offset = vec4(samplePos, 1.0);
        offset = projection * offset; 
        offset.xyz /= offset.w;
        offset.xyz = offset.xyz * 0.5 + 0.5; 
        
        float sampleDepth = getViewPos(texture(gDepth, offset.xy).r).z;
        float rangeCheck = smoothstep(0.0, 1.0, RADIUS / abs(FragViewPos.z - sampleDepth));
        
        occlusion += (sampleDepth >= samplePos.z + BIAS ? 1.0 : 0.0) * rangeCheck;           
    }
    occlusion = 1.0 - (occlusion / NUM_KERNELS);
    
    FragColors = occlusion;
}