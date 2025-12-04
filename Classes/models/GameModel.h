#pragma once
#include <vector>
#include "CardModel.h"
#include "PlayfieldModel.h"
#include "StackModel.h"

// GameModel: runtime state of a game session
// Stores single playfield and stack plus the current hand.
class GameModel {
public:
    PlayfieldModel playfieldModel; // table playfield
    StackModel stack;              // bottom/tray pile
    // current hand maintained at game level
};
