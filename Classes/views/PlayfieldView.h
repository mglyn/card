#pragma once
#include "cocos2d.h"
#include "models/PlayfieldModel.h"

// PlayfieldView: renders forest; only roots face-up, children face-down
class PlayfieldView : public cocos2d::Node {
public:
    static PlayfieldView* create(const PlayfieldModel* model);
    // callback when a playfield card is clicked (by id)
    void setClickCallback(const std::function<void(int)>& cb){ _onCardClick = cb; }
    // rebuild children from model state
    void refresh(const PlayfieldModel& model);
private:
    std::function<void(int)> _onCardClick;
};