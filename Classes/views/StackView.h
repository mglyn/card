#pragma once
#include "cocos2d.h"
#include "models/StackModel.h"
#include "models/GameModel.h"

// StackView: renders tray as horizontal face-down cards
class StackView : public cocos2d::Node {
public:
    static StackView* create(const StackModel* model);
    // callback when a stack card is clicked (by id)
    void setClickCallback(const std::function<void(int)>& cb){ _onClick = cb; }
    // rebuild children from model state
    void refresh(const StackModel& model);
    // configure stack layout relative position and spacing
    void setLayout(float startX, float startY, float deltaX){ _startX = startX; _startY = startY; _deltaX = deltaX; }
    // get the position for a card index in the stack
    cocos2d::Vec2 getCardPositionByIndex(size_t index) const { return cocos2d::Vec2(_startX + _deltaX * static_cast<float>(index), _startY); }
    // get the current hand position (rendered in stack view)
    cocos2d::Vec2 getHandPosition() const { return cocos2d::Vec2(_handX, _startY); }
    // configure padding and card height used for content size
    void setPadding(float paddingX, float paddingY){ _paddingX = paddingX; _paddingY = paddingY; }
    void setCardHeight(float h){ _cardHeight = h; }
private:
    std::function<void(int)> _onClick;
    float _startX{200.f};
    float _startY{300.f};
    float _deltaX{30.f};
    float _handX{800.f};
    float _paddingX{10.f};
    float _paddingY{10.f};
    float _cardHeight{140.f};
};