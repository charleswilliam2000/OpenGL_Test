#version 460 core
layout (location = 0) in uint aPacked;

#define MAX_CHUNKS 100
layout(std140, binding = 0) uniform ModelMatrices {
    mat4 modelMatrices[MAX_CHUNKS];
};

layout(std140, binding = 1) uniform VPMatrices {
	mat4 projectionMatrix;
	mat4 viewMatrix;
};

out vec3 FragPos;
out vec2 TexCoords;
out vec3 Normal;

const vec3 normalsArr[6] = vec3[](
    vec3(-1.0, 0.0, 0.0),
    vec3(0.0, -1.0, 0.0),
    vec3(0.0, 0.0, -1.0),
    vec3(1.0, 0.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.0, 1.0)
);

void main() {
    mat4 model = modelMatrices[gl_DrawID]; 

	int coord_mask = 31; // Binary: 11111
    float x = float((aPacked >> 0) & coord_mask);
    float y = float((aPacked >> 5) & coord_mask);
    float z = float((aPacked >> 10) & coord_mask);

	vec4 worldPos = model * vec4(vec3(x, y, z), 1.0);
    FragPos = worldPos.xyz;

    float v = float((aPacked >> 18) & 1); 
    float u = float((aPacked >> 19) & 1); 
    TexCoords = vec2(u, v);
    
    int normals_mask = 7; // Binary: 111
    uint normals_index = uint((aPacked >> 15) & normals_mask);
    Normal = mat3(transpose(inverse(model))) * normalsArr[normals_index];

    gl_Position = projectionMatrix * viewMatrix * worldPos;
}