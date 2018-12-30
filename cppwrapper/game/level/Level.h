//
// Created by Sidorenko Nikita on 2018-12-18.
//

#ifndef CPPWRAPPER_LEVEL_H
#define CPPWRAPPER_LEVEL_H

#include "EngineMain.h"
#include "EngTypes.h"
#include <string>
#include <unordered_map>

class Level {
public:
  Level(ScenePtr scene);
  void load(std::string fileName);

private:
  GameObjectPtr _loadHierarchy(HierarchyDataPtr hierarchy, const GameObjectPtr parent);

  GameObjectPtr _levelRoot;
  ScenePtr _scene;
  ModelBundlePtr _architecture;
  ModelBundlePtr _props;
  ModelBundlePtr _level;

  std::unordered_map<std::string, TexturePtr> _textures;
};


#endif //CPPWRAPPER_LEVEL_H
