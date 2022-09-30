#include "Entity.h"

namespace SGE {
	Entity::Entity(entt::entity entityHandle, Scene* scene)
		:m_EntityHandle(entityHandle), m_Scene(scene){}
		
	Entity::Entity(uint32_t entityID, Scene* scene)
		:m_EntityHandle(entt::entity(entityID)), m_Scene(scene) {}

}