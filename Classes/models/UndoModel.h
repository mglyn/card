#pragma once
#include <vector>
#include "CardModel.h"

// UndoModel: holds reversible actions for undo feature
// Each record contains source and target areas and involved card ids/positions.
struct UndoRecord {
    enum Area { Playfield, Tray, Stack };
    int cardId{-1};
    Area from{Playfield};
    Area to{Playfield};
    float fromX{0}, fromY{0};
    float toX{0}, toY{0};
    CardModel removedCard; // Store the card that was removed (for playfield cards)
    CardModel previousHand; // Store the previous hand card
    bool hadPreviousHand{false};
};

class UndoModel {
public:
    std::vector<UndoRecord> records;
    void push(const UndoRecord& r) { records.push_back(r); }
    bool empty() const { return records.empty(); }
    UndoRecord pop() {
        auto r = records.back();
        records.pop_back();
        return r;
    }
};