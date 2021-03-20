#version 430 core
out vec4 FragColor;

in vec2 TexCoord;
//flat in int layer;
in float layer;

layout (binding=0) uniform sampler2DArray textureArray;

void main()
{
	FragColor = texture(textureArray, vec3(TexCoord.x,TexCoord.y,floor(layer+0.5f)) );
}