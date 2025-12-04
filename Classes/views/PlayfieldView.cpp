#include "PlayfieldView.h"
#include "CardView.h"
using namespace cocos2d;

PlayfieldView* PlayfieldView::create(const PlayfieldModel* model){
    auto v = new (std::nothrow) PlayfieldView();
    if (v && v->init()){
        v->setContentSize(Size(1080.f, 1000.f));
        v->setAnchorPoint(cocos2d::Vec2::ANCHOR_BOTTOM_LEFT);
        if (model){
            v->refresh(*model);
        }
        v->autorelease();
        return v;
    }
    delete v;
    return nullptr;
}

void PlayfieldView::refresh(const PlayfieldModel& model){
    this->removeAllChildren();
    // simple layout: place roots in a grid-like formation
    float startX = 300.f;
    float startY = 900.f;
    float dx = 120.f;
    float dy = 180.f;
    int col = 0;
    int row = 0;
    for (auto const& c : model.cards){
        CardModel cm = c;
        // assign a default position if not set
        if (cm.position.x == 0 && cm.position.y == 0){
            cm.position.x = startX + dx * col;
            cm.position.y = startY - dy * row;
        }
        auto cv = CardView::create(cm);
        if (cv){
            // forward clicks via CardView's callback
            cv->setClickCallback([this, cm](int /*id*/){ if (_onCardClick) _onCardClick(cm.id); });
            this->addChild(cv);
        }
        // advance grid
        ++col;
        if (col >= 6){ col = 0; ++row; }
    }
}