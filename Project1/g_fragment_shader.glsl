#version 460 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gColorSpecular;

in vec3 FragPos;
in vec2 TexCoords;
in vec3 Normal;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

void main()
{    
    gPosition = FragPos;
    gNormal = normalize(Normal);
    gColorSpecular.rgb = texture(texture_diffuse1, TexCoords).rgb;
    gColorSpecular.a = texture(texture_specular1, TexCoords).r;
}