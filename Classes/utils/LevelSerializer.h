#pragma once
#include <string>
#include "configs/models/LevelConfig.h"

// LevelSerializer: serialize and deserialize LevelConfig to/from JSON
class LevelSerializer {
public:
    // Deserialize JSON string to LevelConfig
    static LevelConfig deserialize(const std::string& json);
    
    // Serialize LevelConfig to JSON string
    static std::string serialize(const LevelConfig& config);
};
