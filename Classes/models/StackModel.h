#pragma once
#include <vector>
#include "CardModel.h"

// StackModel: horizontal spread of face-down cards (tray)
class StackModel {
public:
    std::vector<CardModel> cards; // all faceUp=false initially
    CardModel hand;
    bool hasHand{ false };
};