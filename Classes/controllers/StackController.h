#pragma once
#include "models/StackModel.h"
#include "views/StackView.h"

// StackController: coordinates bottom tray interactions
class StackController {
public:
    bool init(StackModel* model, StackView* view);
    void initView(StackView* view);
    // Pop the tail card from stack; returns true if a card was popped
    CardModel popTailCard(bool& ok);
    // Pop by card id
    CardModel popById(int cardId, bool& ok);
    // Refresh view with current model and game state
    void refreshView();
    // Access internal model
    StackModel* getModel() const { return _model; }
private:
    StackModel* _model{nullptr};
    StackView* _view{nullptr};
};