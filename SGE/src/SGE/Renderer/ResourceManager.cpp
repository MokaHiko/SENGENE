#include "ResourceManager.h"

namespace SGE {
	std::unordered_map<std::string, Ref<Shader>> ResourceManager::m_Shaders{};
	std::unordered_map<std::string, Ref<Texture2D>> ResourceManager::m_Textures{};
	std::unordered_map<std::string, Ref<Mesh>> ResourceManager::m_Meshes{};

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
}