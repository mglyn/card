#pragma once
#include "models/PlayfieldModel.h"
#include "views/PlayfieldView.h"

// PlayfieldController: coordinates forest stack interactions
class PlayfieldController {
public:
    bool init(PlayfieldModel* model, PlayfieldView* view);
    void initView(PlayfieldView* view);
    PlayfieldView* getView() const { return _view; }
private:
    PlayfieldModel* _model{nullptr};
    PlayfieldView* _view{nullptr};
};