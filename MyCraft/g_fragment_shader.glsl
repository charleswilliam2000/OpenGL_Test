#version 460 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gColorSpecular;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;
in float LayerIndex;

uniform sampler2DArray textureAtlas;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

void main()
{    
    // store the fragment position vector in the first gbuffer texture
    vec4 text = texture(textureAtlas, vec3(TexCoords, LayerIndex));

    gPosition = FragPos;
    // also store the per-fragment normals into the gbuffer
    gNormal = normalize(Normal);
    // and the diffuse per-fragment color
    gColorSpecular.rgb = text.rgb * texture(texture_diffuse1, TexCoords).rgb;
    // store specular intensity in gAlbedoSpec's alpha component
    gColorSpecular.a = text.r * texture(texture_specular1, TexCoords).r;
}