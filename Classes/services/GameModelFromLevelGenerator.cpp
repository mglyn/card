#include "GameModelFromLevelGenerator.h"

void GameModelFromLevelGenerator::seedDemo(GameModel& gm){
    gm.stack.cards.push_back(CardModel{1, CardFaceType::CFT_FOUR, CardSuitType::CST_CLUBS, {540, 200}, true});
    gm.playfieldModel.cards.push_back(CardModel{2, CardFaceType::CFT_THREE, CardSuitType::CST_DIAMONDS, {540, 1200}, true});
}

GameModel GameModelFromLevelGenerator::generateGameModel(const LevelConfig& cfg){
    GameModel gm;
    for (auto const& c : cfg.playfield){
        CardModel card;
        card.id = static_cast<int>(gm.playfieldModel.cards.size()) + 1;
        card.face = static_cast<CardFaceType>(c.face);
        card.suit = static_cast<CardSuitType>(c.suit);
        card.position = {c.x, c.y};
        card.faceUp = true;
        gm.playfieldModel.cards.push_back(card);
    }
    for (auto const& c : cfg.stack){
        CardModel card;
        card.id = static_cast<int>(gm.stack.cards.size()) + 100;
        card.face = static_cast<CardFaceType>(c.face);
        card.suit = static_cast<CardSuitType>(c.suit);
        card.position = {c.x, c.y};
        card.faceUp = false;
        gm.stack.cards.push_back(card);
    }
    return gm;
}