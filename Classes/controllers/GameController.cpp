#include "GameController.h"
#include "views/PlayfieldView.h"
#include "views/StackView.h"
#include "views/CardView.h"
#include "utils/LevelSerializer.h"
using namespace cocos2d;

bool GameController::startGame(int levelId) {
    std::string json = LevelConfigLoader::loadLevelJson(levelId);
    LevelConfig cfg = LevelSerializer::deserialize(json);
    GameModel gm = GameModelFromLevelGenerator::generateGameModel(cfg);
    _gameModel = std::make_unique<GameModel>(std::move(gm));

    auto view = GameView::create();
    _gameView = view;
    setupViews(view);

    // Set up undo button callback
    if (_gameView) {
        _gameView->setUndoCallback([this]() {
            this->undo();
            });
        updateUndoButtonState();
    }

    return view != nullptr;
}

int GameController::findPlayfieldCardIndex(int cardId) const {
    if (!_gameModel) return -1;
    // assuming playfield roots are the clickable cards for now
    const auto& cards = _gameModel->playfieldModel.cards;
    for (size_t i = 0; i < cards.size(); ++i) {
        if (cards[i].id == cardId) return static_cast<int>(i);
    }
    return -1;
}

bool GameController::handlePlayfieldCardClick(int cardId) {
    if (!_gameModel)
        return false;
    if (!_gameModel->stack.hasHand) {
        return false;
    }
    // Try match against current hand
    int idx = findPlayfieldCardIndex(cardId);
    if (idx >= 0) {
        auto& cards = _gameModel->playfieldModel.cards;
        const CardModel c = cards[idx];
        int diff = std::abs(c.faceValue() - _gameModel->stack.hand.faceValue());
        if (diff == 1) {
            // Record undo information before making changes
            UndoRecord undoRec;
            undoRec.cardId = _gameModel->stack.hand.id;
            undoRec.from = UndoRecord::Tray; // from hand
            undoRec.to = UndoRecord::Playfield; // to playfield
            undoRec.fromX = _stackView ? _stackView->getHandPosition().x : 0;
            undoRec.fromY = _stackView ? _stackView->getHandPosition().y : 0;
            undoRec.toX = c.position.x;
            undoRec.toY = c.position.y;
            undoRec.removedCard = c; // Store the playfield card that will be removed
            undoRec.previousHand = _gameModel->stack.hand; // Store current hand
            undoRec.hadPreviousHand = true;
            _undoManager.record(undoRec);

            // Play match animation for playfield card
            // remove clicked from playfield
            auto& cards2 = _gameModel->playfieldModel.cards;
            if (idx >= 0 && idx < static_cast<int>(cards2.size())) {
                cards2.erase(cards2.begin() + idx);
            }
            if (_playfieldController.getView()) {
                _playfieldController.getView()->refresh(_gameModel->playfieldModel);
            }

            if (_gameView) {
                cocos2d::Vec2 handPos = _stackView->getHandPosition();
                _gameView->playMoveAnimation(c, c.position.x, c.position.y, handPos.x, handPos.y, [this, c, idx]() {
                    // Update current hand 
                    _gameModel->stack.hand = c;
                    _gameModel->stack.hasHand = true;
                    _stackController.refreshView();
                    updateUndoButtonState();
                    });
            }
            else {
                // Update current hand 
                _gameModel->stack.hand = c;
                _gameModel->stack.hasHand = true;
                _stackController.refreshView();
                updateUndoButtonState();
            }
            return true;
        }
    }
    return true;
}

void GameController::handleStackCardClick(int cardId) {
    // compute start position BEFORE removing card
    StackModel* sm = _stackController.getModel();
    size_t posIndex = 0;
    for (size_t i = 0; i < sm->cards.size(); ++i) {
        if (sm->cards[i].id == cardId) {
            posIndex = i; break;
        }
    }
    cocos2d::Vec2 startPos = _stackView ? _stackView->getCardPositionByIndex(posIndex) : cocos2d::Vec2(200.f, 250.f);
    cocos2d::Vec2 handPos = _stackView ? _stackView->getHandPosition() : cocos2d::Vec2(200.f, 150.f);

    bool ok = false;
    CardModel c = _stackController.popById(cardId, ok);
    _stackController.refreshView();

    // Record undo information
    if (ok) {
        UndoRecord undoRec;
        undoRec.cardId = cardId;
        undoRec.from = UndoRecord::Stack;
        undoRec.to = UndoRecord::Tray;
        undoRec.fromX = startPos.x;
        undoRec.fromY = startPos.y;
        undoRec.toX = handPos.x;
        undoRec.toY = handPos.y;
        // Store previous hand state before replacing it
        if (_gameModel->stack.hasHand) {
            undoRec.previousHand = _gameModel->stack.hand;
            undoRec.hadPreviousHand = true;
        } else {
            undoRec.hadPreviousHand = false;
        }
        _undoManager.record(undoRec);
    }
    if (ok) {
        // run animation via GameView
        if (_gameView) {
            cocos2d::Vec2 handPos = _stackView->getHandPosition();
            _gameView->playMoveAnimation(c, startPos.x, startPos.y, handPos.x, handPos.y, [this, c]() {
                _gameModel->stack.hand = c;
                _gameModel->stack.hasHand = true;
                _stackController.refreshView();
                updateUndoButtonState();
                });
        }
        else {
            _gameModel->stack.hand = c;
            _gameModel->stack.hasHand = true;
            _stackController.refreshView();
            updateUndoButtonState();
        }
    }
}


bool GameController::undo() {
    if (!_undoManager.canUndo() || !_gameModel || !_gameView) {
        return false;
    }

    UndoRecord rec = _undoManager.take();

    // Perform undo with animation (reverse the operation)
    if (rec.to == UndoRecord::Tray && rec.from == UndoRecord::Stack) {
        // Undo: hand back to stack (last position)
        // Card went from stack to hand, reverse it
        if (!_gameModel->stack.hasHand) {
            return false;
        }
        CardModel cardToUndo = _gameModel->stack.hand;

        // Step 1: Restore previous hand state (before this card was picked)
        if (rec.hadPreviousHand) {
            _gameModel->stack.hand = rec.previousHand;
            _gameModel->stack.hasHand = true;
        } else {
            _gameModel->stack.hasHand = false;
        }
        _stackController.refreshView();

        // Step 2: Play animation from hand position back to stack position
        _gameView->playMoveAnimation(cardToUndo, rec.toX, rec.toY, rec.fromX, rec.fromY, [this, cardToUndo]() {
            // Step 3: Put card back to stack at destination and update view
            _gameModel->stack.cards.push_back(cardToUndo);
            _stackController.refreshView();
            updateUndoButtonState();
            });
    }
    else if (rec.to == UndoRecord::Playfield && rec.from == UndoRecord::Tray) {
        // Undo: restore playfield card and restore previous hand
        // Current hand came from playfield, need to put it back
        if (!_gameModel->stack.hasHand) {
            return false;
        }

        CardModel currentHand = _gameModel->stack.hand;

        // Step 1: Restore previous hand (remove current hand from display)
        if (rec.hadPreviousHand) {
            _gameModel->stack.hand = rec.previousHand;
            _gameModel->stack.hasHand = true;
        }
        else {
            _gameModel->stack.hasHand = false;
        }
        _stackController.refreshView();

        // Step 2: Play animation from hand position (fromX, fromY) back to playfield position (toX, toY)
        _gameView->playMoveAnimation(currentHand, rec.fromX, rec.fromY, rec.toX, rec.toY, [this, rec]() {
            // Step 3: Restore playfield card at destination and update view
            _gameModel->playfieldModel.cards.push_back(rec.removedCard);
            if (_playfieldController.getView()) {
                _playfieldController.getView()->refresh(_gameModel->playfieldModel);
            }
            updateUndoButtonState();
            });
    }

    return true;
}

void GameController::updateUndoButtonState() {
    if (_gameView) {
        _gameView->setUndoEnabled(_undoManager.canUndo());
    }
}

void GameController::setupViews(GameView* view) {
    if (!(_gameModel && view)) return;
    // Create and attach subviews for playfield and stack from model
    {
        auto& pfModel = _gameModel->playfieldModel;
        auto pfView = PlayfieldView::create(&pfModel);
        if (pfView) {
            view->addChild(pfView);
            _playfieldController.init(&pfModel, pfView);
            _playfieldController.initView(pfView);
            // hook playfield card click
            pfView->setClickCallback([this](int cardId) { this->handlePlayfieldCardClick(cardId); });
        }
    }
    {
        auto& sModel = _gameModel->stack;
        auto sView = StackView::create(&sModel);
        if (sView) {
            view->addChild(sView);
            _stackController.init(&sModel, sView);
            _stackController.initView(sView);
            _stackView = sView;
            // stack per-card click
            sView->setClickCallback(std::bind(&GameController::handleStackCardClick, this, std::placeholders::_1));
        }
    }
}
