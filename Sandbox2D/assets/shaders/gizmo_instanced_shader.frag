#version 450 core
out vec4 FragColor;

struct Material
{
	bool HasDiffuseTexture;
	bool HasSpecularTexture;

	sampler2D texture_diffuse1;
	sampler2D texture_specular1;

	vec3 Ambient;
	vec3 Diffuse;
	vec3 Specular;
};

in vec4 v_Weights;
flat in ivec4 v_BoneIDs;

uniform int u_FocusedBoneIndex;

void main()
{
	for(int i = 0; i < 4; i++)
	{
		if(v_BoneIDs[i] == u_FocusedBoneIndex)
		{
			if(v_Weights[i] >= 0.7)
				FragColor = vec4(1.0, 0.0, 0.0, 1.0f);
			else if(v_Weights[i] >= 0.4 && v_Weights[i] < 0.7)
				FragColor = vec4(0.0, 1.0, 0.0, 1.0f);
			else if(v_Weights[i] >= 0.1)
				FragColor = vec4(1.0, 1.0, 0.0, 1.0f);
			else FragColor = vec4(0.0, 0.0, 1.0, 1.0f);
		}
	}
}