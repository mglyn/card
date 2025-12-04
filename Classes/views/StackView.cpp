#include "StackView.h"
#include "CardView.h"

StackView* StackView::create(const StackModel* model){
    auto v = new (std::nothrow) StackView();
    if (v && v->init()){
        // set stack logical size
        v->setContentSize(cocos2d::Size(1080.f, 580.f));
        v->setAnchorPoint(cocos2d::Vec2::ANCHOR_BOTTOM_LEFT);

        v->refresh(*model);

        v->autorelease();
        return v;
    }
    delete v; 
    return nullptr;
}

void StackView::refresh(const StackModel& model){
    this->removeAllChildren();
    float x = _startX; float y = _startY;
    for (auto const& c : model.cards){
        CardModel cm = c;
        cm.position.x = x; cm.position.y = y;
        auto cv = CardView::create(cm);
        if (cv){
            // CardView handles its own touch; just forward id via callback
            cv->setClickCallback([this, cm](int /*id*/){ if (_onClick) _onClick(cm.id); });
            cv->setPosition(cocos2d::Vec2(x, y));
            this->addChild(cv);
        }
        x += _deltaX;
    }
    if (model.hasHand){
        auto hand = CardView::create(model.hand);
        if (hand){
            hand->setPosition(cocos2d::Vec2(_handX, y));
            this->addChild(hand, 1);
        }
    }
    // update content size to cover rendered stack area
    float width = std::max(0.f, x - _startX);
    this->setContentSize(cocos2d::Size(width + _paddingX, _cardHeight + _paddingY));
}