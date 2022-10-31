#include "ResourceManager.h"

namespace SGE {
	std::unordered_map<std::string, Ref<Shader>> ResourceManager::m_Shaders{};
	std::unordered_map<std::string, Ref<Texture2D>> ResourceManager::m_Textures{};
    std::unordered_map<std::string, Ref<Material>> ResourceManager::m_Materials{};
    std::unordered_map<std::string, Ref<Model>> ResourceManager::m_Models{};

	Ref<Shader> ResourceManager::CreateShader(const std::string& vertexPath, const std::string& fragmentPath)
	{
		// TODO: FIND BETTER NAMING CONVENTION FOR SHADERS. 
		std::string shaderName = fragmentPath.substr(0, fragmentPath.find(".frag"));

		if(m_Shaders.find(shaderName) == m_Shaders.end())
			m_Shaders[shaderName] = CreateRef<Shader>(vertexPath, fragmentPath);

		return m_Shaders[shaderName];
	}
	
	Ref<Shader> ResourceManager::GetShader(const std::string& shaderName)
	{
		if(m_Shaders.find(shaderName) != m_Shaders.end())
			return m_Shaders[shaderName];
		
		std::cout << "ERROR::RESOURCE: Shader \"" << shaderName << "\" does not exist! \n";
		return nullptr;
	}
	
	Ref<Texture2D> ResourceManager::CreateTexture(const std::string& texturePath)
	{
		if(m_Textures.find(texturePath) == m_Textures.end())
			m_Textures[texturePath] = CreateRef<Texture2D>(texturePath.c_str());

		return m_Textures[texturePath];
	}
	
	Ref<Texture2D> ResourceManager::GetTexture(const std::string& textureName)
	{
		if(m_Textures.find(textureName) != m_Textures.end())
			return m_Textures[textureName];
		
		std::cout << "ERROR::RESOURCE: Texture \"" <<  textureName << "\" does not exist! \n";
		return nullptr;
	}
	
	Ref<Material> ResourceManager::CreateMaterial(const std::string& name, const glm::vec3& ambientColor, const glm::vec3 diffuseColor, 
												const Ref<Texture2D>& diffuseTexture, const Ref<Texture2D>& specularTexture)
	{
		if(m_Materials.find(name) == m_Materials.end())
		{
			m_Materials[name] = CreateRef<Material>();
			m_Materials[name]->Name = name;
			m_Materials[name]->AmbientColor = ambientColor;
			m_Materials[name]->DiffuseColor = diffuseColor;
			m_Materials[name]->DiffuseTexture = diffuseTexture;
			m_Materials[name]->SpecularTexture = specularTexture;
		}

		return m_Materials[name];
	}
	
	Ref<Material> ResourceManager::GetMaterial(const std::string& name)
	{
		if(m_Materials.find(name) != m_Materials.end())
			return m_Materials[name];

		std::cout << "ERROR::RESOURCE: Material \"" << name << "\" does not exist! \n";
		return nullptr;
	}

	Ref<Model> ResourceManager::CreateModel(const std::string& modelPath, bool flipUVS)
	{
		if(m_Models.find(modelPath) == m_Models.end())
			m_Models[modelPath] = CreateRef<Model>(modelPath.c_str(), flipUVS);
			
		return m_Models[modelPath];
	}	

	Ref<Model> ResourceManager::GetModel(const std::string& name)
	{
		if(m_Models.find(name) != m_Models.end())
			return m_Models[name];

		std::cout << "ERROR::RESOURCE: Model \"" << name << "\" does not exist! \n";
		return nullptr;
	}
}