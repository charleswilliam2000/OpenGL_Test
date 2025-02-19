#version 460 core

out float FragColors;
in vec2 TexCoords;

const int NUM_KERNELS = 64;
const int POSITION_LAYER = 0;
const int NORMAL_LAYER = 1;

layout (std140, binding = 1) uniform ubo_VPMatrices {
	mat4 projection;
	mat4 view;

    mat4 inverseProjection;
    mat4 inverseView;
};

layout(std140, binding = 2) uniform ubo_Kernels {
	vec3 kernels[NUM_KERNELS];
};

uniform sampler2DArray gTextureArray;
uniform sampler2D ssaoNoise;
uniform vec2 screenSize;
uniform mat4 inverseProj;

float radius = 0.1;
float bias = 0.025;

const vec2 noiseScale = vec2(screenSize.x / 4.0, screenSize.y / 4.0);

vec3 getViewPos(float depth, vec2 texCoords) {  
    float clipZ = depth * 2.0 - 1.0;
    vec4 clipSpacePos = vec4((texCoords * 2.0 - 1.0), clipZ, 1.0);
    vec4 viewSpacePos = inverseProj * clipSpacePos;
    
    viewSpacePos /= viewSpacePos.w;
    
    return viewSpacePos.xyz;
}

void main() {
    vec3 FragViewPos = texture(gTextureArray, vec3(TexCoords, POSITION_LAYER)).xyz;

    vec3 Normal = normalize(texture(gTextureArray, vec3(TexCoords, NORMAL_LAYER)).rgb);
    vec3 randomVec = normalize(texture(ssaoNoise, TexCoords * noiseScale).xyz);

    vec3 Tangent = normalize(randomVec - Normal * dot(randomVec, Normal));
    vec3 Bitangent = cross(Normal, Tangent);
    mat3 TBN = mat3(Tangent, Bitangent, Normal);

    float occlusion = 0.0;
    for(int i = 0; i < NUM_KERNELS; i++)
    {
        vec3 samplePos = TBN * kernels[i].xyz;
        samplePos = FragViewPos + samplePos * radius; 
        
        vec4 offset = vec4(samplePos, 1.0);
        offset = projection * offset; 
        offset.xyz /= offset.w;
        offset.xyz = offset.xyz * 0.5 + 0.5; 
        
        float sampleDepth = texture(gTextureArray, vec3(offset.xy, POSITION_LAYER)).z;
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(FragViewPos.z - sampleDepth));
        
        occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;           
    }
    occlusion = 1.0 - (occlusion / NUM_KERNELS);
    
    FragColors = occlusion;
}