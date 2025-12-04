#pragma once
#include <string>
#include <vector>
#include "cocos2d.h"

// CardModel: runtime data for a single card
// Holds face, suit, id, position, and open/covered state.
enum class CardSuitType {
    CST_NONE = -1,
    CST_CLUBS,
    CST_DIAMONDS,
    CST_HEARTS,
    CST_SPADES,
    CST_NUM_CARD_SUIT_TYPES
};

enum class CardFaceType {
    CFT_NONE = -1,
    CFT_ACE,
    CFT_TWO,
    CFT_THREE,
    CFT_FOUR,
    CFT_FIVE,
    CFT_SIX,
    CFT_SEVEN,
    CFT_EIGHT,
    CFT_NINE,
    CFT_TEN,
    CFT_JACK,
    CFT_QUEEN,
    CFT_KING,
    CFT_NUM_CARD_FACE_TYPES
};

struct Vec2 { float x{0}, y{0}; };

class CardModel {
public:
    int id{ -1 };
    CardFaceType face{ CardFaceType::CFT_NONE };
    CardSuitType suit{ CardSuitType::CST_NONE };
    cocos2d::Vec2 position{};
    bool faceUp{ true }; // opened if true, covered if false

    int faceValue() const { return static_cast<int>(face); }
};