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

const std::string ARCHITECTURE_FILE = "resources/level/architecture.mdl";
const std::string NO_SHADOW = "no_shadow";

Level::Level(ScenePtr scene) : _scene(scene) {
  _levelRoot = CreateGameObject<GameObject>();
  _architecture = loader::loadModel(ARCHITECTURE_FILE);
  _architectureAtlas = loader::loadTexture("resources/level/Atlas_Base_01.jpg");
//  _architectureAtlas = loader::loadTexture("resources/white.jpg");
  _architectureBumpAtlas = loader::loadTexture("resources/level/Atlas_Base_01_Normal.jpg", false);
}

GameObjectPtr Level::_loadHierarchy(HierarchyDataPtr hierarchy, const GameObjectPtr parentObj) {

  auto parent = parentObj ? parentObj : _levelRoot;

  for (auto &child : hierarchy->children) {
    GameObjectPtr object;
    HierarchyDataPtr referenceNode = _architecture->getHierarchyByName(child->originalNodeID);

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
        light->coneAngle(120);
      }

      light->radius(30);
      light->attenuation(0.01, 0.05);

      object = light;
    } else if (referenceNode && referenceNode->hasGeometry) {
      MeshObjectPtr meshObject = CreateGameObject<MeshObject>();
      meshObject->mesh(_architecture->getMesh(referenceNode->geometry));
      if (!meshObject->mesh()->hasTBN()) {
        meshObject->mesh()->calculateTBN();
        meshObject->mesh()->createBuffer();
      }
      object = meshObject;

      ENGLog("original: %s (%s) ", child->originalNodeID.c_str(), child->name.c_str() );

      auto material = std::make_shared<MaterialTextureBump>();
      material->texture(_architectureAtlas);
      material->normalMap(_architectureBumpAtlas);
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

