#include "CardView.h"
using namespace cocos2d;

static const char* suitTexture(CardSuitType suit){
    switch (suit){
        case CardSuitType::CST_CLUBS: return "suits/club.png";
        case CardSuitType::CST_DIAMONDS: return "suits/diamond.png";
        case CardSuitType::CST_HEARTS: return "suits/heart.png";
        case CardSuitType::CST_SPADES: return "suits/spade.png";
        default: return nullptr;
    }
}

static const char* faceTexture(CardFaceType face, CardSuitType suit){
    const bool isRed = (suit == CardSuitType::CST_HEARTS || suit == CardSuitType::CST_DIAMONDS);
    switch (face) {
        case CardFaceType::CFT_ACE:   return isRed ? "number/small_red_A.png"   : "number/small_black_A.png";
        case CardFaceType::CFT_TWO:   return isRed ? "number/small_red_2.png"   : "number/small_black_2.png";
        case CardFaceType::CFT_THREE: return isRed ? "number/small_red_3.png"   : "number/small_black_3.png";
        case CardFaceType::CFT_FOUR:  return isRed ? "number/small_red_4.png"   : "number/small_black_4.png";
        case CardFaceType::CFT_FIVE:  return isRed ? "number/small_red_5.png"   : "number/small_black_5.png";
        case CardFaceType::CFT_SIX:   return isRed ? "number/small_red_6.png"   : "number/small_black_6.png";
        case CardFaceType::CFT_SEVEN: return isRed ? "number/small_red_7.png"   : "number/small_black_7.png";
        case CardFaceType::CFT_EIGHT: return isRed ? "number/small_red_8.png"   : "number/small_black_8.png";
        case CardFaceType::CFT_NINE:  return isRed ? "number/small_red_9.png"   : "number/small_black_9.png";
        case CardFaceType::CFT_TEN:   return isRed ? "number/small_red_10.png"  : "number/small_black_10.png";
        case CardFaceType::CFT_JACK:  return isRed ? "number/small_red_J.png"   : "number/small_black_J.png";
        case CardFaceType::CFT_QUEEN: return isRed ? "number/small_red_Q.png"   : "number/small_black_Q.png";
        case CardFaceType::CFT_KING:  return isRed ? "number/small_red_K.png"   : "number/small_black_K.png";
        default: return "";
    }
}

static const char* patternTexture(CardFaceType face, CardSuitType suit) {
    const bool isRed = (suit == CardSuitType::CST_HEARTS || suit == CardSuitType::CST_DIAMONDS);
    switch (face) {
        case CardFaceType::CFT_ACE:   return isRed ? "number/big_red_A.png"    : "number/big_black_A.png";
        case CardFaceType::CFT_TWO:   return isRed ? "number/big_red_2.png"    : "number/big_black_2.png";
        case CardFaceType::CFT_THREE: return isRed ? "number/big_red_3.png"    : "number/big_black_3.png";
        case CardFaceType::CFT_FOUR:  return isRed ? "number/big_red_4.png"    : "number/big_black_4.png";
        case CardFaceType::CFT_FIVE:  return isRed ? "number/big_red_5.png"    : "number/big_black_5.png";
        case CardFaceType::CFT_SIX:   return isRed ? "number/big_red_6.png"    : "number/big_black_6.png";
        case CardFaceType::CFT_SEVEN: return isRed ? "number/big_red_7.png"    : "number/big_black_7.png";
        case CardFaceType::CFT_EIGHT: return isRed ? "number/big_red_8.png"    : "number/big_black_8.png";
        case CardFaceType::CFT_NINE:  return isRed ? "number/big_red_9.png"    : "number/big_black_9.png";
        case CardFaceType::CFT_TEN:   return isRed ? "number/big_red_10.png"   : "number/big_black_10.png";
        case CardFaceType::CFT_JACK:  return isRed ? "number/big_red_J.png"    : "number/big_black_J.png";
        case CardFaceType::CFT_QUEEN: return isRed ? "number/big_red_Q.png"    : "number/big_black_Q.png";
        case CardFaceType::CFT_KING:  return isRed ? "number/big_red_K.png"    : "number/big_black_K.png";
        default: return "";
    }
}

CardView* CardView::create(const CardModel& model){
    auto v = new (std::nothrow) CardView();
    if (v && v->init()){
        v->setContentSize(Size(100.f, 140.f));
        v->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE);
        v->setPosition(cocos2d::Vec2(model.position.x, model.position.y));

        // card background
        Sprite* bg = Sprite::create("card_general.png");
        if (!bg){
            bg = Sprite::create("cc_2x2_white_image");
        }
        if (!bg){
            bg = Sprite::create();
            bg->setTextureRect(Rect(0,0,100,140));
            bg->setColor(Color3B::WHITE);
        }
        bg->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE);
        bg->setPosition(cocos2d::Vec2::ZERO);
        v->addChild(bg);

        // update click size to match background sprite size (with scale)
        cocos2d::Size clickSize = bg->getContentSize();
        float sx = bg->getScaleX();
        float sy = bg->getScaleY();
        if (sx != 0.f && sy != 0.f) {
            clickSize.width *= sx;
            clickSize.height *= sy;
        }
        v->setContentSize(clickSize);

        // suit icon
        const char* suitTex = suitTexture(model.suit);
        if (suitTex){
            if (auto suit = Sprite::create(suitTex)){
                suit->setPosition(cocos2d::Vec2(45, 100));
                v->addChild(suit);
            }
        }

        // face label
        const char* faceTex = faceTexture(model.face, model.suit);
        if (faceTex && faceTex[0] != '\0'){
            if (auto face = Sprite::create(faceTex)) {
                face->setPosition(cocos2d::Vec2(-50, 100));
                v->addChild(face);
            }
        }

        // face pattern
        const char* patternTex = patternTexture(model.face, model.suit);
        if (patternTex && patternTex[0] != '\0') {
            if (auto pattern = Sprite::create(patternTex)) {
                pattern->setPosition(cocos2d::Vec2(0, -20));
                v->addChild(pattern);
            }
        }

        // touch listener for this card
        auto listener = EventListenerTouchOneByOne::create();
        listener->setSwallowTouches(true);
        const int id = model.id;
        listener->onTouchBegan = [v, id](Touch* t, Event*){
            auto loc = v->convertToNodeSpace(t->getLocation());
            Rect r( -v->getContentSize().width*0.5f, -v->getContentSize().height*0.5f,
                    v->getContentSize().width, v->getContentSize().height);
            if (r.containsPoint(loc)){
                if (v->_onClick) v->_onClick(id);
                return true;
            }
            return false;
        };
        v->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, v);

        v->autorelease();
        return v;
    }
    delete v; 
    return nullptr;
}