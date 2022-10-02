#version 450 core

out vec4 FragColor;
in vec2 v_TexCoord;

uniform sampler2D u_BaseColorSampler;
void main()
{
	FragColor = texture(u_BaseColorSampler, v_TexCoord);
}

