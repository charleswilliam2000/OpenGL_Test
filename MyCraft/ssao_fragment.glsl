#version 460 core

out float FragColors;
in vec2 TexCoords;


layout (std140, binding = 1) uniform VPMatrices {
	mat4 projectionMatrix;
	mat4 viewMatrix;
};
#define NUM_KERNELS 64
layout(std140, binding = 2) uniform Kernels {
	vec3 kernels[NUM_KERNELS];
};

const int NORMAL_LAYER = 0;

uniform sampler2DArray gTextArray;
uniform sampler2D gDepth;
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

vec3 getWorldPos(vec4 viewSpacePos) {    
    vec4 worldSpacePos = inverse(viewMatrix) * viewSpacePos;
    return worldSpacePos.xyz;
}

float linearizeDepth(float depth, float zNear, float zFar) {
    float z = depth * 2.0 - 1.0;
    return (2.0 * zNear * zFar) / (zFar + zNear - depth * ( zFar  - zNear));
}

void main() {
    float depth = texture(gDepth, TexCoords).r; 

    vec3 fragViewPos = getViewPos(depth, TexCoords);

    vec3 normal = normalize(mat3(viewMatrix) * texture(gTextArray, vec3(TexCoords, NORMAL_LAYER)).rgb);
    vec3 randomVec = normalize(texture(ssaoNoise, TexCoords * noiseScale).xyz);

    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);

    float occlusion = 0.0;
    for(int i = 0; i < NUM_KERNELS; i++)
    {
        vec3 samplePos = TBN * kernels[i].xyz;
        samplePos = fragViewPos + samplePos * radius; 
        
        vec4 offset = vec4(samplePos, 1.0);
        offset = projectionMatrix * offset; 
        offset.xyz /= offset.w;
        offset.xyz = offset.xyz * 0.5 + 0.5; 
        
        float sampleDepth = texture(gDepth, offset.xy).r;
        float linearSampleDepth = linearizeDepth(sampleDepth, 0.1, 100.0);
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragViewPos.z - linearSampleDepth));
        
        occlusion += (linearSampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;           
    }
    occlusion = 1.0 - (occlusion / NUM_KERNELS);
    
    FragColors = occlusion;
}