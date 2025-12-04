#pragma once
#include "cocos2d.h"
#include <functional>
#include "models/CardModel.h"

namespace cocos2d {
    namespace ui {
        class Button;
    }
}

// GameView: root node for game UI, owns subviews
class GameView : public cocos2d::Node {
public:
    static GameView* create();
    void playMoveAnimation(const CardModel& card, float startX, float startY, float targetX, float targetY, const std::function<void()>& onComplete);
    // set undo button callback
    void setUndoCallback(const std::function<void()>& callback) { _onUndo = callback; }
    // enable/disable undo button
    void setUndoEnabled(bool enabled);
private:
    std::function<void()> _onUndo;
    cocos2d::ui::Button* _undoButton{nullptr};
};