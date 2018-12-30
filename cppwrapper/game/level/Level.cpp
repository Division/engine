//
// Created by Sidorenko Nikita on 2018-12-18.
//

#include "Level.h"
#include "loader/HierarchyLoader.h"
#include "loader/ModelLoader.h"
#include "loader/TextureLoader.h"
#include "scene/GameObject.h"
#include "EngMath.h"
#include "system/Logging.h"
#include "objects/LightObject.h"

const std::string ATLAS_ARCHITECTURE = "resources/level/Atlas_Base_01.jpg";
const std::string ATLAS_ARCHITECTURE_NORMALMAP = "resources/level/Atlas_Base_01_Normal.jpg";
const std::string ATLAS_FURNITURE = "resources/level/Atlas_Furniture_01.jpg";
const std::string ATLAS_FURNITURE_NORMALMAP = "resources/level/Atlas_Furniture_01_Normal.jpg";
const std::string ATLAS_PROPS1 = "resources/level/Atlas_Props_01.jpg";
const std::string ATLAS_PROPS1_NORMALMAP = "resources/level/Atlas_Props_01_Normal.jpg";
const std::string ATLAS_PROPS2 = "resources/level/Atlas_Props_02.jpg";
const std::string ATLAS_PROPS2_NORMALMAP = "resources/level/Atlas_Props_02_Normal.jpg";
const std::string ARCHITECTURE_FILE = "resources/level/architecture.mdl";
const std::string PROPS_FILE = "resources/level/props.mdl";
const std::string NO_SHADOW = "no_shadow";

Level::Level(ScenePtr scene) : _scene(scene) {
  _levelRoot = CreateGameObject<GameObject>();
  _architecture = loader::loadModel(ARCHITECTURE_FILE);
  _props = loader::loadModel(PROPS_FILE);
  _textures[ATLAS_ARCHITECTURE] = loader::loadTexture(ATLAS_ARCHITECTURE);
  _textures[ATLAS_ARCHITECTURE_NORMALMAP] = loader::loadTexture(ATLAS_ARCHITECTURE_NORMALMAP, false);
  _textures[ATLAS_FURNITURE] = loader::loadTexture(ATLAS_FURNITURE);
  _textures[ATLAS_FURNITURE_NORMALMAP] = loader::loadTexture(ATLAS_FURNITURE_NORMALMAP, false);
  _textures[ATLAS_PROPS1] = loader::loadTexture(ATLAS_PROPS1);
  _textures[ATLAS_PROPS1_NORMALMAP] = loader::loadTexture(ATLAS_PROPS1_NORMALMAP, false);
  _textures[ATLAS_PROPS2] = loader::loadTexture(ATLAS_PROPS2);
  _textures[ATLAS_PROPS2_NORMALMAP] = loader::loadTexture(ATLAS_PROPS2_NORMALMAP, false);
}

GameObjectPtr Level::_loadHierarchy(HierarchyDataPtr hierarchy, const GameObjectPtr parentObj) {

  auto parent = parentObj ? parentObj : _levelRoot;

  for (auto &child : hierarchy->children) {
    GameObjectPtr object;
    HierarchyDataPtr arch = _architecture->getHierarchyByName(child->originalNodeID);
    HierarchyDataPtr prop = _props->getHierarchyByName(child->originalNodeID);
    HierarchyDataPtr referenceNode;
    ModelBundlePtr bundle;

    if (arch) {
      referenceNode = arch;
      bundle = _architecture;
    } else if (prop) {
      referenceNode = prop;
      bundle = _props;
    }

    if (child->isLight) {
      auto lightData = _level->getLight(child->light);
      auto light = CreateGameObject<LightObject>();
      light->type(lightData->type == "spot" ? LightObjectType::Spot : LightObjectType::Point);

      if (child->name.find(NO_SHADOW) == std::string::npos) {
        light->castShadows(true);
      }

      if (light->type() == LightObjectType::Spot) {
        light->coneAngle(lightData->coneAngle);
      } else {
        light->coneAngle(160);
      }

      light->radius(30);
      light->attenuation(0.01, 0.01);

      object = light;
    } else if (referenceNode && referenceNode->hasGeometry) {
      MeshObjectPtr meshObject = CreateGameObject<MeshObject>();
      meshObject->mesh(bundle->getMesh(referenceNode->geometry));
      if (!meshObject->mesh()->hasTBN()) {
        meshObject->mesh()->calculateTBN();
        meshObject->mesh()->createBuffer();
      }
      object = meshObject;

      auto material = std::make_shared<MaterialTextureBump>();
      material->texture(_textures.at(*referenceNode->material->diffuse));
      auto normalMapName = *referenceNode->material->diffuse;
      normalMapName.replace(normalMapName.length() - 4, 0, "_Normal");
      material->normalMap(_textures.at(normalMapName));
      meshObject->material(material);
    } else {
      object = CreateGameObject<GameObject>();
    }

    object->transform()->parent(parent->transform());
    object->transform()->setMatrix(child->transform);
    object->name(hierarchy->name);

    _loadHierarchy(child, object);
  }

  return parent;
}

void Level::load(std::string fileName) {
  _level = loader::loadModel(fileName);
  _loadHierarchy(_level->hierarchy(), nullptr);
}

