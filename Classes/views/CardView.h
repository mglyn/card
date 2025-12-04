#pragma once
#include "cocos2d.h"
#include "models/CardModel.h"

// CardView: renders a single card with face and suit
class CardView : public cocos2d::Node {
public:
    static CardView* create(const CardModel& model);
    // set callback to be invoked when this card is clicked
    void setClickCallback(const std::function<void(int)>& cb){ _onClick = cb; }
private:
    std::function<void(int)> _onClick;
};