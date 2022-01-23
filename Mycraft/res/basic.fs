#version 430 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 color;

uniform sampler2D ourTexture;

void main()
{
	FragColor = texture(ourTexture, TexCoord);
	FragColor.x *= color.x;
	FragColor.y *= color.y;
	FragColor.z *= color.z;
	
	if(FragColor.a < 0.5)
		discard;
}