#include "SceneSerializer.h"

#include <fstream>

#include "Renderer/ResourceManager.h"
#include "Scene/Components.h"
namespace YAML {
template <> struct convert<glm::vec2> {
  static Node encode(const glm::vec2 &rhs) {
    Node node;
    node.push_back(rhs.x);
    node.push_back(rhs.y);
    return node;
  }

  static bool decode(const Node &node, glm::vec2 &rhs) {
    if (!node.IsSequence() || node.size() != 2)
      return false;

    rhs.x = node[0].as<float>();
    rhs.y = node[1].as<float>();
    return true;
  }
};

template <> struct convert<glm::vec3> {
  static Node encode(const glm::vec3 &rhs) {
    Node node;
    node.push_back(rhs.x);
    node.push_back(rhs.y);
    node.push_back(rhs.z);
    return node;
  }

  static bool decode(const Node &node, glm::vec3 &rhs) {
    if (!node.IsSequence() || node.size() != 3)
      return false;

    rhs.x = node[0].as<float>();
    rhs.y = node[1].as<float>();
    rhs.z = node[2].as<float>();
    return true;
  }
};
} // namespace YAML
namespace SGE {
YAML::Emitter &operator<<(YAML::Emitter &out, glm::vec2 &v) {
  out << YAML::Flow;
  out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
  return out;
}

YAML::Emitter &operator<<(YAML::Emitter &out, glm::vec3 &v) {
  out << YAML::Flow;
  out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
  return out;
}

YAML::Emitter &operator<<(YAML::Emitter &out, glm::vec4 &v) {
  out << YAML::Flow;
  out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
  return out;
}

SceneSerializer::SceneSerializer(const Ref<Scene> &scene) : m_Scene(scene) {}

SceneSerializer::~SceneSerializer() {}

void SceneSerializer::Serialize(const std::string &filePath) {
  YAML::Emitter out;
  out << YAML::BeginMap;
  out << YAML::Key << "Scene" << YAML::Value << "Untitled Scene";
  out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;

  m_Scene->m_Registry.each([&](auto entityID) {
    Entity entity{entityID, m_Scene.get()};
    if (!entity)
      return;

    SerializeEntity(out, entity);
  });

  out << YAML::EndSeq;
  out << YAML::EndMap;

  std::ofstream fout(filePath);
  fout << out.c_str();
}

void SceneSerializer::Deserialize(const std::string &filePath) {
  std::ifstream stream(filePath);
  std::stringstream strStream;
  strStream << stream.rdbuf();

  YAML::Node data = YAML::Load(strStream.str());
  if (!data["Scene"])
    throw std::runtime_error("Failed to load scene");

  std::string sceneName = data["Scene"].as<std::string>();
  m_Scene->m_Name = sceneName;

  auto entities = data["Entities"];
  if (entities) {
    for (auto entity : entities) {
      uint32_t uuid = entity["Entity"].as<uint32_t>();

      std::string name;
      auto tagComponent = entity["TagComponent"];
      if (tagComponent) {
        name = tagComponent["Tag"].as<std::string>();
      }

      Entity deserializedEntity = m_Scene->CreateEntity(name);

      auto transformComponent = entity["TransformComponent"];
      if (transformComponent) {
        auto &tc = deserializedEntity.GetComponent<TransformComponent>();
        tc.Position = transformComponent["Position"].as<glm::vec3>();
        tc.Rotation = transformComponent["Rotation"].as<glm::vec3>();
        tc.Scale = transformComponent["Scale"].as<glm::vec3>();
      }

      auto meshRendererComponent = entity["MeshRendererComponent"];
      if (meshRendererComponent) {
        std::string path = meshRendererComponent["Path"].as<std::string>();
        bool flipUVS = meshRendererComponent["FlipUVS"].as<bool>();
        deserializedEntity.AddComponent<MeshRendererComponent>(
            Model::CreateModel(path), flipUVS);
      }

      auto skinnedMeshRendererComponent =
          entity["SkinnedMeshRendererComponent"];
      if (skinnedMeshRendererComponent) {
        std::string path =
            skinnedMeshRendererComponent["Path"].as<std::string>();
        bool flipUVS = skinnedMeshRendererComponent["FlipUVS"].as<bool>();
        deserializedEntity.AddComponent<SkinnedMeshRendererComponent>(
            AnimatedModel::CreateAnimatedModel(path, flipUVS));
      }

      auto directionalLightComponent = entity["DirectionalLightComponent"];
      if (directionalLightComponent) {
        auto &dlComponent =
            deserializedEntity.AddComponent<DirectionalLightComponent>();
        dlComponent.Ambient =
            directionalLightComponent["Ambient"].as<glm::vec3>();
        dlComponent.Diffuse =
            directionalLightComponent["Diffuse"].as<glm::vec3>();
        dlComponent.Specular =
            directionalLightComponent["Specular"].as<glm::vec3>();
      }

      auto pointLightComponent = entity["PointLightComponent"];
      if (pointLightComponent) {
        auto &plComponent =
            deserializedEntity.AddComponent<PointLightComponent>();
        plComponent.Ambient = pointLightComponent["Ambient"].as<glm::vec3>();
        plComponent.Diffuse = pointLightComponent["Diffuse"].as<glm::vec3>();
        plComponent.Specular = pointLightComponent["Specular"].as<glm::vec3>();

        plComponent.Constant = pointLightComponent["Constant"].as<float>();
        plComponent.Linear = pointLightComponent["Linear"].as<float>();
        plComponent.Quadratic = pointLightComponent["Quadratic"].as<float>();
      }

      auto cam3DComponent = entity["Camera3DComponent"];
      if (cam3DComponent) {
        auto &camComponent =
            deserializedEntity.AddComponent<Camera3DComponent>();
        camComponent.IsActive = cam3DComponent["IsActive"].as<bool>();
      }

      auto rbComponent = entity["RigidBodyComponent"];
      if (rbComponent) {
        auto &rb = deserializedEntity.AddComponent<RigidBodyComponent>();
        rb.UseGravity = rbComponent["UseGravity"].as<bool>();
      }

      auto sphereColliderComponent = entity["SphereColliderComponent"];
      if (sphereColliderComponent) {
        auto &sphereCollider =
            deserializedEntity.AddComponent<SphereColliderComponent>();
        sphereCollider.sphereCollider.Center =
            sphereColliderComponent["Center"].as<glm::vec3>();
        sphereCollider.sphereCollider.Radius =
            sphereColliderComponent["Radius"].as<float>();
      }

      auto planeColliderComponent = entity["PlaneColliderComponent"];
      if (planeColliderComponent) {
        auto &planeCollider =
            deserializedEntity.AddComponent<PlaneColliderComponent>();
        planeCollider.planeCollider.Origin =
            planeColliderComponent["Origin"].as<glm::vec3>();
        planeCollider.planeCollider.Normal =
            planeColliderComponent["Normal"].as<glm::vec3>();
        planeCollider.planeCollider.Bounds =
            planeColliderComponent["Bounds"].as<glm::vec2>();
      }
    }
  }
}

void SceneSerializer::SerializeEntity(YAML::Emitter &out, Entity entity) {
  out << YAML::BeginMap;
  out << YAML::Key << "Entity" << YAML::Value << "42312412";

  if (entity.HasComponent<TagComponent>()) {
    out << YAML::Key << "TagComponent";
    out << YAML::BeginMap;

    auto &tag = entity.GetComponent<TagComponent>();
    out << YAML::Key << "Tag" << YAML::Value << tag.Tag.c_str();

    out << YAML::EndMap;
  }

  if (entity.HasComponent<TransformComponent>()) {
    out << YAML::Key << "TransformComponent";
    out << YAML::BeginMap;

    auto &transform = entity.GetComponent<TransformComponent>();
    out << YAML::Key << "Position" << YAML::Value << transform.Position;
    out << YAML::Key << "Rotation" << YAML::Value << transform.Rotation;
    out << YAML::Key << "Scale" << YAML::Value << transform.Scale;

    out << YAML::EndMap;
  }

  if (entity.HasComponent<MeshRendererComponent>()) {
    out << YAML::Key << "MeshRendererComponent";
    out << YAML::BeginMap;

    auto &model = entity.GetComponent<MeshRendererComponent>();
    for (auto it = ResourceManager::m_Models.begin();
         it != ResourceManager::m_Models.end();) {
      if (it->second == model.Model) {
        out << YAML::Key << "Path" << YAML::Value << it->first;
      }
      it++;
    }
    out << YAML::Key << "FlipUVS" << YAML::Value << model.FlipUVS;
    out << YAML::EndMap;
  }

  if (entity.HasComponent<SkinnedMeshRendererComponent>()) {
    out << YAML::Key << "SkinnedMeshRendererComponent";
    out << YAML::BeginMap;

    auto &model = entity.GetComponent<SkinnedMeshRendererComponent>();
    for (auto it = ResourceManager::m_AnimatedModels.begin();
         it != ResourceManager::m_AnimatedModels.end();) {
      if (it->second == model.AnimatedModel) {
        out << YAML::Key << "Path" << YAML::Value << it->first;
      }
      it++;
    }
    out << YAML::Key << "FlipUVS" << YAML::Value << model.FlipUVS;
    out << YAML::EndMap;
  }

  if (entity.HasComponent<DirectionalLightComponent>()) {
    out << YAML::Key << "DirectionalLightComponent";
    out << YAML::BeginMap;

    auto &directionalLightComponent =
        entity.GetComponent<DirectionalLightComponent>();
    out << YAML::Key << "Ambient" << YAML::Value
        << directionalLightComponent.Ambient;
    out << YAML::Key << "Diffuse" << YAML::Value
        << directionalLightComponent.Diffuse;
    out << YAML::Key << "Specular" << YAML::Value
        << directionalLightComponent.Specular;

    out << YAML::EndMap;
  }

  if (entity.HasComponent<PointLightComponent>()) {
    out << YAML::Key << "PointLightComponent";
    out << YAML::BeginMap;

    auto &pointLightComponent = entity.GetComponent<PointLightComponent>();
    out << YAML::Key << "Ambient" << YAML::Value << pointLightComponent.Ambient;
    out << YAML::Key << "Diffuse" << YAML::Value << pointLightComponent.Diffuse;
    out << YAML::Key << "Specular" << YAML::Value
        << pointLightComponent.Specular;

    out << YAML::Key << "Constant" << YAML::Value
        << pointLightComponent.Constant;
    out << YAML::Key << "Linear" << YAML::Value << pointLightComponent.Linear;
    out << YAML::Key << "Quadratic" << YAML::Value
        << pointLightComponent.Quadratic;

    out << YAML::EndMap;
  }

  if (entity.HasComponent<Camera3DComponent>()) {
    out << YAML::Key << "Camera3DComponent";
    out << YAML::BeginMap;

    auto &cam3DComponent = entity.GetComponent<Camera3DComponent>();
    out << YAML::Key << "IsActive" << YAML::Value << cam3DComponent.IsActive;

    out << YAML::EndMap;
  }

  if (entity.HasComponent<RigidBodyComponent>()) {
    out << YAML::Key << "RigidBodyComponent";
    out << YAML::BeginMap;

    auto &rbComponent = entity.GetComponent<RigidBodyComponent>();
    out << YAML::Key << "UseGravity" << YAML::Value << rbComponent.UseGravity;

    out << YAML::EndMap;
  }

  if (entity.HasComponent<SphereColliderComponent>()) {
    out << YAML::Key << "SphereColliderComponent";
    out << YAML::BeginMap;

    auto &sphereCollider = entity.GetComponent<SphereColliderComponent>();
    glm::vec3 center = sphereCollider.sphereCollider.Center -
                       entity.GetComponent<TransformComponent>().Position;
    out << YAML::Key << "Center" << YAML::Value << center;
    out << YAML::Key << "Radius" << YAML::Value
        << sphereCollider.sphereCollider.Radius;
    out << YAML::EndMap;
  }

  if (entity.HasComponent<PlaneColliderComponent>()) {
    out << YAML::Key << "PlaneColliderComponent";
    out << YAML::BeginMap;

    auto &planeCollider = entity.GetComponent<PlaneColliderComponent>();
    out << YAML::Key << "Origin" << YAML::Value
        << planeCollider.planeCollider.Origin;
    out << YAML::Key << "Normal" << YAML::Value
        << planeCollider.planeCollider.Normal;
    out << YAML::Key << "Bounds" << YAML::Value
        << planeCollider.planeCollider.Bounds;

    out << YAML::EndMap;
  }

  out << YAML::EndMap;
}
} // namespace SGE