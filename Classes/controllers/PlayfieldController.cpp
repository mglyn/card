#include "PlayfieldController.h"

bool PlayfieldController::init(PlayfieldModel* model, PlayfieldView* view){
    _model = model;
    _view = view; 
    return model && view;
}

void PlayfieldController::initView(PlayfieldView* view){
    _view = view;
}