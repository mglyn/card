#pragma once
#include "models/GameModel.h"
#include "models/UndoModel.h"
#include "views/GameView.h"
#include "controllers/StackController.h"
#include "controllers/PlayfieldController.h"
#include "configs/loaders/LevelConfigLoader.h"
#include "services/GameModelFromLevelGenerator.h"
#include "managers/UndoManager.h"
#include <memory>

// GameController: orchestrates game flow
class GameController {
public:
    bool startGame(int levelId);
    GameView* getView() const { return _gameView; }
    bool undo();
private:
    // click handler for stack area
    void onStackClicked();
    // helper: get playfield card index by id
    int findPlayfieldCardIndex(int cardId) const;
    bool handlePlayfieldCardClick(int cardId);
    void handleStackCardClick(int cardId);
    // helper: setup subviews and controllers
    void setupViews(GameView* view);
    // update undo button enabled state
    void updateUndoButtonState();
    std::unique_ptr<GameModel> _gameModel; // owned model
    GameView* _gameView{nullptr};
    UndoManager _undoManager;
    PlayfieldController _playfieldController;
    StackController _stackController;
    StackView* _stackView{nullptr};
};