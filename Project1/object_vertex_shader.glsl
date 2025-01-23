#version 460 core
layout (location = 0) in uint aPacked; 

out vec3 FragPos;
out vec3 Normal;
out vec2 TextureCoords;
out float layerIndex;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

const vec3 normalsArr[6] = vec3[](
    vec3(-1.0, 0.0, 0.0),
    vec3(0.0, -1.0, 0.0),
    vec3(0.0, 0.0, -1.0),
    vec3(1.0, 0.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.0, 1.0)
);

void main() {	
    //Unpack coordinates (5 bits each)
    int coord_mask = 31; // Binary: 11111
    float x = float((aPacked >> 0) & coord_mask);
    float y = float((aPacked >> 5) & coord_mask);
    float z = float((aPacked >> 10) & coord_mask);

    int normals_mask = 7; // Binary: 111
    uint normals_index = uint((aPacked >> 15) & normals_mask);
    Normal = normalsArr[normals_index];

    float v = float((aPacked >> 18) & 1); 
    float u = float((aPacked >> 19) & 1); 

    int texture_mask = 15; // 1111
    layerIndex = float((aPacked >> 20) & texture_mask);

    vec3 aPos = vec3(x, y, z);
    FragPos = vec3(model * vec4(aPos, 1.0));
    TextureCoords = vec2(u, v);

  gl_Position = projection * view * model * vec4(aPos, 1.0);
}