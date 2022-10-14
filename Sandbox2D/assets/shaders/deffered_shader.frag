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

struct DirectionalLight
{
	vec3 Direction;

	vec3 Ambient;
	vec3 Diffuse;
	vec3 Specular;
};

struct PointLight
{
	vec3 Position;

	float Constant;
	float Linear;
	float Quadratic;

	vec3 Ambient;
	vec3 Diffuse;
	vec3 Specular;
};

uniform DirectionalLight u_DirLight;
uniform Material u_Material;
uniform vec3 u_MainCameraPos;

in vec3 Normal;
in vec3 FragPos;
in vec2 v_TexCoord;

const int MAX_POINT_LIGHTS = 10;
uniform int u_NPointLights;
uniform PointLight u_PointLights[MAX_POINT_LIGHTS];

vec3 CalculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir);
vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
void main()
{
	vec3 norm = normalize(Normal);
	vec3 viewDir =  normalize(u_MainCameraPos - FragPos);

	// Directional Light
	vec3 result = {0.0, 0.0, 0.0};
	result = CalculateDirectionalLight(u_DirLight, norm, viewDir);

	//Point Lights
	for(int i = 0; i < u_NPointLights; i++)
		result += CalculatePointLight(u_PointLights[i], norm, FragPos, viewDir);
		
	FragColor = vec4(result, 1.0f);
}

vec3 CalculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(-light.Direction);

	float diff = max(dot(normal,lightDir),0.0);

	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir,reflectDir), 0.0f), 32);

	vec3 ambient = light.Ambient * u_Material.Ambient;
	vec3 diffuse = light.Diffuse * diff * u_Material.Diffuse;
	vec3 specular = light.Specular * spec * u_Material.Specular;

	if(u_Material.HasDiffuseTexture)
	{
		ambient *= texture(u_Material.texture_diffuse1, v_TexCoord).rgb;
		diffuse *= texture(u_Material.texture_diffuse1, v_TexCoord).rgb;
	}

	if(u_Material.HasSpecularTexture)
		specular = light.Specular * spec * u_Material.Specular * texture(u_Material.texture_specular1, v_TexCoord).rgb;

	return (ambient + diffuse + specular);
}
vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.Position - fragPos);

	float diff = max(dot(normal, lightDir), 0);

	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0f), 32);

	float distance = length(light.Position - fragPos);
	float attenuation = 1.0 / (light.Constant + light.Linear * distance + 
  			     light.Quadratic * (distance * distance)); 

	vec3 ambient = light.Ambient * u_Material.Ambient;
	vec3 diffuse = light.Diffuse * diff * u_Material.Diffuse;
	vec3 specular = light.Specular * spec * u_Material.Specular;

	if(u_Material.HasDiffuseTexture)
	{
		ambient *= texture(u_Material.texture_diffuse1, v_TexCoord).rgb;
		diffuse *= texture(u_Material.texture_diffuse1, v_TexCoord).rgb;
	}

	if(u_Material.HasSpecularTexture)
		specular = light.Specular * spec * u_Material.Specular * texture(u_Material.texture_specular1, v_TexCoord).rgb;

	return (ambient + diffuse + specular) * attenuation;
}