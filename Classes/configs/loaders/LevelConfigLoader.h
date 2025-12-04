#pragma once
#include <string>
#include <sstream>
#include "cocos2d.h"

class LevelConfigLoader {
public:
    // Only read raw JSON text from root-level Levels directory
    static inline std::string loadLevelJson(int levelId){
        auto levelPathFromId = [](int id){
            std::ostringstream oss; oss << "Levels/level" << id << ".json"; return oss.str();
        };
        auto fu = cocos2d::FileUtils::getInstance();
        std::string json = fu->getStringFromFile(levelPathFromId(levelId));
        if (json.empty()){
            cocos2d::log("LevelConfigLoader: could not load level file for id %d", levelId);
        }
        return json;
    }
};