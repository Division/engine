//
// Created by Sidorenko Nikita on 2018-12-18.
//

#ifndef CPPWRAPPER_LEVEL_H
#define CPPWRAPPER_LEVEL_H

#include "EngineMain.h"
#include "EngTypes.h"
#include <string>

class Level {
public:
  Level(ScenePtr scene);
  void load(std::string fileName);

private:
  GameObjectPtr _loadHierarchy(HierarchyDataPtr hierarchy, const GameObjectPtr parent);
  TexturePtr _architectureAtlas;
  TexturePtr _architectureBumpAtlas;

  GameObjectPtr _levelRoot;
  ScenePtr _scene;
  ModelBundlePtr _architecture;
  ModelBundlePtr _level;
};


#endif //CPPWRAPPER_LEVEL_H
