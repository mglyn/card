#include "StackController.h"
using namespace cocos2d;

bool StackController::init(StackModel* model, StackView* view){
    _model = model; 
    _view = view; 
    return model && view;
}

void StackController::initView(StackView* view){
    _view = view;
}

CardModel StackController::popTailCard(bool& ok){
    ok = false;
    if (!_model) return CardModel{};
    if (_model->cards.empty()) return CardModel{};
    CardModel next = _model->cards.back();
    _model->cards.pop_back();
    ok = true;
    return next;
}

CardModel StackController::popById(int cardId, bool& ok){
    ok = false;
    if (!_model) return CardModel{};
    for (auto it = _model->cards.begin(); it != _model->cards.end(); ++it){
        if (it->id == cardId){
            CardModel out = *it;
            _model->cards.erase(it);
            ok = true;
            return out;
        }
    }
    return CardModel{};
}

void StackController::refreshView(){
    if (_view && _model){
        _view->refresh(*_model);
    }
}