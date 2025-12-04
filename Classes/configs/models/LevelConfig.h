#pragma once
#include <vector>

struct LevelCardConfig {
    int face{0};
    int suit{0};
    float x{0};
    float y{0};
};

struct LevelUndoRecordConfig {
    int cardId{-1};
    int from{0};  // 0=Playfield, 1=Tray, 2=Stack
    int to{0};
    float fromX{0}, fromY{0};
    float toX{0}, toY{0};
    LevelCardConfig removedCard;
    LevelCardConfig previousHand;
    bool hadPreviousHand{false};
};

class LevelConfig {
public:
    std::vector<LevelCardConfig> playfield;
    std::vector<LevelCardConfig> stack;
    LevelCardConfig hand;
    bool hasHand{false};
    std::vector<LevelUndoRecordConfig> undoRecords;
};