#version 450 core

layout(location=0)in vec3 a_Position;
layout(location=1)in vec3 a_Normal;
layout(location=2)in vec2 a_TexCoord;

layout(location=5)in mat4 a_ModelMatrix;

uniform mat4 projection;
uniform mat4 view;

out vec3 Normal;
out vec3 FragPos;
out vec2 v_TexCoord;

uniform float u_Time;
void main()
{
	/*
	To Note:
	- All light values are calculate in world space i.e FragPos, Normal, etc.
	- Normals should not be translated so we use the top left mat3 of a_ModelMatrix
	- Normals should not be none-uniformly scaled, so we take the inverse of the tranpose of a_ModelMatrix
	*/
	
	//	Check if Model Has Bones
	vec4 position=vec4(a_Position,1.);
	
	FragPos=vec3(a_ModelMatrix*position);
	Normal=inverse(transpose(mat3(a_ModelMatrix)))*a_Normal;
	v_TexCoord=a_TexCoord;
	
	float windSpeed=2.f;
	vec2 variance=vec2(.01f,.01f)*sin((FragPos.x+FragPos.z)+(u_Time*windSpeed))*FragPos.y;
	position.x+=variance.x;
	position.z+=variance.y;
	
	gl_Position=projection*view*a_ModelMatrix*position;
}