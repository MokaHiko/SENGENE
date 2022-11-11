#version 450 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec2 a_TexCoord;

layout (location = 3) in ivec4 a_BoneIDs;
layout (location = 4) in vec4 a_Weights;

layout (location = 5) in mat4 a_ModelMatrix;

uniform mat4 projection;
uniform mat4 view;

out vec3 Normal;
out vec3 FragPos;
out vec2 v_TexCoord;

// Bones
const int MAX_BONES = 100;

flat out ivec4 v_BoneIDs;
out vec4 v_Weights;

uniform mat4 u_Bones[MAX_BONES];
void main()
{
	/*
	To Note:
		- All light values are calculate in world space i.e FragPos, Normal, etc.
		- Normals should not be translated so we use the top left mat3 of a_ModelMatrix
		- Normals should not be none-uniformly scaled, so we take the inverse of the tranpose of a_ModelMatrix
	*/
	mat4 boneTransform = u_Bones[a_BoneIDs[0]] * a_Weights[0];
	boneTransform += u_Bones[a_BoneIDs[1]] * a_Weights[1];
	boneTransform += u_Bones[a_BoneIDs[2]] * a_Weights[2];
	boneTransform += u_Bones[a_BoneIDs[3]] * a_Weights[3];

	FragPos = vec3(a_ModelMatrix * vec4(a_Position, 1.0));
	Normal = inverse(transpose(mat3(a_ModelMatrix))) * a_Normal;
	v_TexCoord = a_TexCoord;

	v_BoneIDs = a_BoneIDs;
	v_Weights = a_Weights;
	
	vec4 position = boneTransform * vec4(a_Position, 1.0f);
	gl_Position = projection * view * a_ModelMatrix * position;
}