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
#include "objects/Projector.h"
#include "../objects/StainGlass.h"
#include <string>

const unsigned int LAYER_DEFAULT = 1 << 0;
const unsigned int LAYER_WINDOW = 1 << 5;
const unsigned int LAYER_ROOM1 = 1 << 10;
const unsigned int LAYER_ROOM2 = 1 << 11;
const unsigned int LAYER_ALL_ROOMS = LAYER_ROOM1 | LAYER_ROOM2;
const unsigned int LAYER_FLOOR = 1 << 9;

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

Level::Level(ScenePtr &scene, SpriteSheetPtr &decals) : _scene(scene), _decals(decals) {
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

GameObjectPtr Level::_createLight(HierarchyDataPtr &child) {
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

  return light;
}

GameObjectPtr Level::_createProjector(HierarchyDataPtr &child) {
  auto projector = CreateGameObject<Projector>();
  projector->transform()->setMatrix(child->transform);
  vec3 scale = projector->transform()->scale();
  projector->transform()->scale(vec3(1));

  projector->type(ProjectorType::Projector);
  projector->zFar(fabs(scale.z));
  projector->aspect(scale.x / scale.y);

  projector->zNear(0.005);
  projector->attenuation(0.0, 0.00);
  projector->orthographicSize(scale.y);
//  projector->setDebugEnabled(true);
  projector->isOrthographic(true);
  projector->castShadows(true);
  std::string decalName = child->name;
  decalName.erase(0, decalName.find('_') + 1);
  auto bounds = _decals->getSpriteData(decalName).bounds;
  projector->spriteBounds(bounds);

  return nullptr;
}

MeshObjectPtr Level::_createMeshObjectByName(const std::string &name) {
  if (name.find("Plane_mesh") != std::string::npos) {
    auto result = CreateGameObject<StainGlass>();
    result->setSpriteSheet(_decals);
    result->layer();
    return result;
  } else {
    return CreateGameObject<MeshObject>();
  }
}

MeshObjectPtr Level::_createMeshObject(ModelBundlePtr &bundle, HierarchyDataPtr &referenceNode) {
  MeshObjectPtr meshObject = _createMeshObjectByName(referenceNode->name);
  meshObject->mesh(bundle->getMesh(referenceNode->geometry));
  if (!meshObject->mesh()->hasTBN()) {
    meshObject->mesh()->calculateTBN();
    meshObject->mesh()->createBuffer();
  }

  auto material = std::make_shared<MaterialTextureBump>();
  material->texture(_textures.at(*referenceNode->material->diffuse));
  auto normalMapName = *referenceNode->material->diffuse;
  normalMapName.replace(normalMapName.length() - 4, 0, "_Normal");
  material->normalMap(_textures.at(normalMapName));
  meshObject->material(material);

  return meshObject;
}

void Level::_assignLayer(GameObjectPtr &parent) {
  unsigned int currentLayer = LAYER_ROOM2;

  auto callback = [&](TransformPtr transform) {
    const auto &object = transform->gameObject();
    object->layer(currentLayer);

    if (auto light = std::dynamic_pointer_cast<LightObject>(object)) {
      light->cameraVisibilityMask(currentLayer | LAYER_DEFAULT);
    } else
    if (auto projector = std::dynamic_pointer_cast<Projector>(object)) {
      projector->cameraVisibilityMask(currentLayer | LAYER_DEFAULT);
    }

    if (object->name().find("Floor") != std::string::npos) {
      object->layer(object->layer() | LAYER_FLOOR);
    }
  };

  if (parent->name() == "room1") {
    currentLayer = LAYER_ROOM1;
    parent->transform()->forEachChild(true, callback);
  } else if (parent->name() == "room2") {
    currentLayer = LAYER_ROOM2;
    parent->transform()->forEachChild(true, callback);
  }
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
      object = _createLight(child);
    } else if (referenceNode && referenceNode->hasGeometry) {
      object = _createMeshObject(bundle, referenceNode);
    } else if (child->name.find("Projector") != std::string::npos) {
      object = _createProjector(child);
    } else {
      object = CreateGameObject<GameObject>();
    }

    if (object) {
      object->transform()->parent(parent->transform());
      object->transform()->setMatrix(child->transform);
      object->name(hierarchy->name);

      _loadHierarchy(child, object);
    }
  }

  _assignLayer(parent);
  return parent;
}

void Level::load(std::string fileName) {
  _level = loader::loadModel(fileName);
  _loadHierarchy(_level->hierarchy(), nullptr);
}


