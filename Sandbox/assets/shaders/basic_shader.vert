#version 450 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec2 a_TexCoord;
layout (location = 3) in mat4 a_ModelMatrix;

uniform mat4 projection;
uniform mat4 view;

out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoord;

void main()
{
	/*
	To Note:
		- All light values are calculate in world space i.e FragPos, Normal, etc.
		- Normals should not be translated so we use the top left mat3 of a_ModelMatrix
		- Normals should not be none-uniformly scaled, so we take the inverse of the tranpose of a_ModelMatrix
	*/
	FragPos = vec3(a_ModelMatrix * vec4(a_Position, 1.0));
	Normal = inverse(transpose(mat3(a_ModelMatrix))) * a_Normal;
	TexCoord = a_TexCoord;
	gl_Position = projection * view *  a_ModelMatrix * vec4(a_Position, 1.0f);
}