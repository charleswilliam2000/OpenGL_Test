#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in int aPackedBits;

out vec3 FragPos;
out vec3 Normal;
out vec2 TextureCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {	
	FragPos = vec3(model * vec4(aPos, 1.0));

	int aPacked = aPackedBits;

    // Unpack normals (3 bits + 1 sign bit)
    float nz = float((aPacked >> 0) & 1); 
    float ny = float((aPacked >> 1) & 1); 
    float nx = float((aPacked >> 2) & 1); 

    bool isNegative = ((aPacked >> 3) & 1) == 1; 

    float v = float((aPacked >> 4) & 1); 
    float u = float((aPacked >> 5) & 1); 

    TextureCoords = vec2(u, v);

    // Apply sign to the normal components
    float sign = (isNegative) ? -1.0 : 1.0;
    Normal = vec3(nx, ny, nz) * sign; 

  gl_Position = projection * view * model * vec4(aPos, 1.0);
}