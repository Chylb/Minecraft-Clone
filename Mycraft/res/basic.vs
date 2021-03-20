#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in float aLayer;

out vec2 TexCoord;

uniform mat4 view;
uniform mat4 projection;

//flat out int layer;
out float layer;

void main()
{
	
	gl_Position = projection * view * vec4(aPos, 1.0f);
	TexCoord = vec2(aTexCoord.x, aTexCoord.y);

	layer = aLayer;
}