#version 450 core

out vec4 FragColor;

struct Material
{
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
	float shininess;
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
in vec2 TexCoord;

#define MAX_POINT_LIGHTS 10
uniform int u_NPointLights;
uniform PointLight u_PointLights[10];

vec3 CalculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir);
vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
void main()
{
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(u_MainCameraPos - FragPos);

	vec3 result = CalculateDirectionalLight(u_DirLight, norm, viewDir);

	for(int i = 0; i < u_NPointLights; i++)
		result += CalculatePointLight(u_PointLights[i], norm, FragPos, viewDir);

	FragColor = vec4(result, 1.0f);
}

vec3 CalculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(-light.Direction);

	float diff = max(dot(normal,lightDir),0.0);
	
	vec3 reflectDir = reflect(-light.Direction, normal);
	float spec = pow(max(dot(viewDir,reflectDir), 0.0f), u_Material.shininess);

	vec3 ambient  = light.Ambient  * texture(u_Material.texture_diffuse1, TexCoord).rgb;
	vec3 diffuse  = light.Diffuse  * diff * texture(u_Material.texture_diffuse1, TexCoord).rgb;
	vec3 specular = light.Specular * spec * texture(u_Material.texture_specular1, TexCoord).rgb;

	return (ambient + diffuse + specular);
}

vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.Position - fragPos);

	float diff = max(dot(normal, lightDir), 0);

	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0f), u_Material.shininess);

	float distance = length(light.Position - fragPos);
	float attenuation = 1.0 / (light.Constant + light.Linear * distance + 
  			     light.Quadratic * (distance * distance)); 

	vec3 ambient = light.Ambient * vec3(texture(u_Material.texture_diffuse1, TexCoord));
	vec3 diffuse = light.Diffuse * diff * vec3(texture(u_Material.texture_diffuse1, TexCoord));
	vec3 specular = light.Specular * spec * vec3(texture(u_Material.texture_specular1, TexCoord));

	return (ambient + diffuse + specular) * attenuation;
}