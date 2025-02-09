#version 460 core
layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gColorSpecular;

in vec3 FragPos;
in vec2 TexCoords;
in vec3 Normal;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

void main()
{    
    gPosition = vec4(FragPos, 1.0);
    gNormal = vec4(normalize(Normal), 1.0);
    gColorSpecular.rgb = texture(texture_diffuse1, TexCoords).rgb;
    gColorSpecular.a = texture(texture_specular1, TexCoords).r;
}