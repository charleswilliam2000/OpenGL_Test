#version 460 core
layout (location = 0) out vec3 gNormal;
layout (location = 1) out vec3 gColorSpecular;

in vec2 TexCoords;
in vec3 Normal;
in float LayerIndex;

uniform sampler2DArray textureAtlas;

void main()
{      
    gNormal = normalize(Normal);
    gColorSpecular.rgb = texture(textureAtlas, vec3(TexCoords, LayerIndex)).rgb;
}