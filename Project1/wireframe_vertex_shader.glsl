#version 460 core

layout (location = 0) in uint aPacked;

#define MAX_CHUNKS 100
layout(std140, binding = 0) uniform ModelMatrices {
    mat4 modelMatrices[MAX_CHUNKS];
};

layout(std140, binding = 1) uniform VP_Matrices {
    mat4 projectionMatrix;
    mat4 viewMatrix;
};

void main() {
	mat4 model = modelMatrices[gl_DrawID]; 
    int coord_mask = 31; // Binary: 11111
    float x = float((aPacked >> 0) & coord_mask);
    float y = float((aPacked >> 5) & coord_mask);
    float z = float((aPacked >> 10) & coord_mask);

    vec3 aPos = vec3(x, y, z);
	gl_Position = projectionMatrix * viewMatrix * model * vec4(aPos, 1.0);
}