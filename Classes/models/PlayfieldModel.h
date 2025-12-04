#pragma once
#include <vector>
#include "CardModel.h"

class PlayfieldModel {
public:
    // multiple trees roots; only root cards are faceUp=true; children faceUp=false
    std::vector<CardModel> cards;
};