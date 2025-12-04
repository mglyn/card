#pragma once
#include "models/GameModel.h"
#include "configs/models/LevelConfig.h"
#include <string>

// GameModelFromLevelGenerator: converts static config to runtime model
class GameModelFromLevelGenerator {
public:
    static void seedDemo(GameModel& gm);
    static GameModel generateGameModel(const LevelConfig& cfg);
};