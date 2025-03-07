#version 460 core
layout (location = 0) in uint aPacked;

out vec2 TexCoords;
out vec3 Normal;
out float LayerIndex;

const int MAX_CHUNKS = 100;
layout(std140, binding = 0) uniform ubo_ModelMatrices {
    mat4 modelMatrices[MAX_CHUNKS];
};

layout(std140, binding = 1) uniform ubo_VPMatrices {
	mat4 projection;
	mat4 view;

    mat4 inverseProjection;
    mat4 inverseView;
};

layout (std140, binding = 2) uniform ubo_NormalMatrices {
    mat4 normalMatrices[MAX_CHUNKS];
};

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

	vec4 viewPos = view * model * vec4(vec3(x, y, z), 1.0);

    int normals_mask = 7; // Binary: 111
    uint normals_index = uint((aPacked >> 15) & normals_mask);
    Normal = mat3(normalMatrices[gl_DrawID]) * normalsArr[normals_index];

    float v = float((aPacked >> 18) & 1); 
    float u = float((aPacked >> 19) & 1); 
    TexCoords = vec2(u, v);

    int texture_mask = 15; // 1111
    LayerIndex = float((aPacked >> 20) & texture_mask);

    gl_Position = projection * viewPos;
}