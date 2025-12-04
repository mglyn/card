#include "GameView.h"
#include "views/CardView.h"
#include "models/CardModel.h"
#include "ui/CocosGUI.h"
using namespace cocos2d;

GameView* GameView::create(){
    auto v = new (std::nothrow) GameView();
    if (v && v->init()) { 
        // background sprite
        if (auto bg = Sprite::create("background.png")){
            bg->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE);
            bg->setPosition(cocos2d::Vec2(540,1040));
            // scale to fit design resolution
            const Size s = bg->getContentSize();
            bg->setScaleX(1080.0f / s.width);
            bg->setScaleY(2080.0f / s.height);
            v->addChild(bg, -10);
        }
        
        // create undo button (positioned to the right of hand card)
        // hand position is at x=800, so button will be at x=950
        auto undoBtn = cocos2d::ui::Button::create();
        if (undoBtn) {
            undoBtn->setTitleText("Undo");
            undoBtn->setTitleFontSize(36);
            undoBtn->setTitleColor(cocos2d::Color3B::WHITE);
            undoBtn->setPosition(cocos2d::Vec2(950, 300)); // Right of hand card
            undoBtn->setContentSize(cocos2d::Size(120, 80));
            undoBtn->setZoomScale(0.1f);
            undoBtn->addClickEventListener([v](cocos2d::Ref*) {
                if (v->_onUndo) {
                    v->_onUndo();
                }
            });
            v->addChild(undoBtn, 10);
            v->_undoButton = undoBtn;
            v->setUndoEnabled(false); // Initially disabled
        }
        
        CCLOG("gameview created");
        v->autorelease(); 
        return v; 
    }
    delete v; 
    return nullptr;
}

void GameView::playMoveAnimation(const CardModel& card, float startX, float startY, float targetX, float targetY, const std::function<void()>& onComplete){
    CardView* animCard = CardView::create(card);
    if (!animCard){
        if (onComplete) onComplete();
        return;
    }
    animCard->setPosition(cocos2d::Vec2(startX, startY));
    this->addChild(animCard, 100);
    cocos2d::Vec2 target(targetX, targetY);
    auto move = MoveTo::create(0.25f, target);
    auto done = CallFunc::create([animCard, onComplete](){
        if (animCard){ animCard->removeFromParent(); }
        if (onComplete) onComplete();
    });
    animCard->runAction(Sequence::create(move, done, nullptr));
}

void GameView::setUndoEnabled(bool enabled) {
    if (_undoButton) {
        _undoButton->setEnabled(enabled);
        _undoButton->setBright(enabled);
    }
}