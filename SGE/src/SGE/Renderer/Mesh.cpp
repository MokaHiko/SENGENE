#include "Mesh.h"

#include "Renderer/ResourceManager.h"
namespace SGE {
	Ref<Material> Material::CreateMaterial(const std::string& name, const glm::vec3& ambientColor, const glm::vec3 diffuseColor, 
										 const Ref<Texture2D>& diffuseTexture, const Ref<Texture2D>& specularTexture)
	{
		return ResourceManager::CreateMaterial(name, ambientColor, diffuseColor, diffuseTexture, specularTexture);
	}
}