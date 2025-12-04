#include "LevelSerializer.h"
#include "json/document.h"
#include "json/writer.h"
#include "json/stringbuffer.h"
#include "cocos2d.h"

// Forward declarations for helper functions
namespace {
    LevelCardConfig parseCard(const rapidjson::Value& cardObj);
    LevelUndoRecordConfig parseUndoRecord(const rapidjson::Value& undoObj);
    void serializeCard(rapidjson::Value& cardObj, const LevelCardConfig& card, rapidjson::Document::AllocatorType& allocator);
    void serializeUndoRecord(rapidjson::Value& undoObj, const LevelUndoRecordConfig& undo, rapidjson::Document::AllocatorType& allocator);
}

LevelConfig LevelSerializer::deserialize(const std::string& json) {
    LevelConfig config;
    
    rapidjson::Document doc;
    doc.Parse(json.c_str());
    
    if (doc.HasParseError()) {
        CCLOG("LevelSerializer: JSON parse error at offset %zu: %d", 
              doc.GetErrorOffset(), doc.GetParseError());
        return config;
    }
    
    // Parse playfield cards (support both "Playfield" and "playfield")
    if (doc.HasMember("Playfield") && doc["Playfield"].IsArray()) {
        const auto& arr = doc["Playfield"].GetArray();
        for (rapidjson::SizeType i = 0; i < arr.Size(); ++i) {
            config.playfield.push_back(parseCard(arr[i]));
        }
    } else if (doc.HasMember("playfield") && doc["playfield"].IsArray()) {
        const auto& arr = doc["playfield"].GetArray();
        for (rapidjson::SizeType i = 0; i < arr.Size(); ++i) {
            config.playfield.push_back(parseCard(arr[i]));
        }
    }
    
    // Parse stack cards (support both "Stack" and "stack")
    if (doc.HasMember("Stack") && doc["Stack"].IsArray()) {
        const auto& arr = doc["Stack"].GetArray();
        for (rapidjson::SizeType i = 0; i < arr.Size(); ++i) {
            config.stack.push_back(parseCard(arr[i]));
        }
    } else if (doc.HasMember("stack") && doc["stack"].IsArray()) {
        const auto& arr = doc["stack"].GetArray();
        for (rapidjson::SizeType i = 0; i < arr.Size(); ++i) {
            config.stack.push_back(parseCard(arr[i]));
        }
    }
    
    // Parse hand card (only lowercase "hand" for save files)
    if (doc.HasMember("hand") && doc["hand"].IsObject()) {
        config.hand = parseCard(doc["hand"]);
        config.hasHand = true;
    } else {
        config.hasHand = false;
    }
    
    // Parse undo records (only for save files)
    if (doc.HasMember("undoRecords") && doc["undoRecords"].IsArray()) {
        const auto& arr = doc["undoRecords"].GetArray();
        for (rapidjson::SizeType i = 0; i < arr.Size(); ++i) {
            config.undoRecords.push_back(parseUndoRecord(arr[i]));
        }
    }
    
    return config;
}

std::string LevelSerializer::serialize(const LevelConfig& config) {
    rapidjson::Document doc;
    doc.SetObject();
    auto& allocator = doc.GetAllocator();
    
    // Serialize playfield cards (use "Playfield" with capital P)
    rapidjson::Value playfieldArr(rapidjson::kArrayType);
    for (const auto& card : config.playfield) {
        rapidjson::Value cardObj(rapidjson::kObjectType);
        serializeCard(cardObj, card, allocator);
        playfieldArr.PushBack(cardObj, allocator);
    }
    doc.AddMember("Playfield", playfieldArr, allocator);
    
    // Serialize stack cards (use "Stack" with capital S)
    rapidjson::Value stackArr(rapidjson::kArrayType);
    for (const auto& card : config.stack) {
        rapidjson::Value cardObj(rapidjson::kObjectType);
        serializeCard(cardObj, card, allocator);
        stackArr.PushBack(cardObj, allocator);
    }
    doc.AddMember("Stack", stackArr, allocator);
    
    // Serialize hand card (only if present, use lowercase "hand" for save files)
    if (config.hasHand) {
        rapidjson::Value handObj(rapidjson::kObjectType);
        serializeCard(handObj, config.hand, allocator);
        doc.AddMember("hand", handObj, allocator);
    }
    
    // Serialize undo records (only if present, for save files)
    if (!config.undoRecords.empty()) {
        rapidjson::Value undoArr(rapidjson::kArrayType);
        for (const auto& undo : config.undoRecords) {
            rapidjson::Value undoObj(rapidjson::kObjectType);
            serializeUndoRecord(undoObj, undo, allocator);
            undoArr.PushBack(undoObj, allocator);
        }
        doc.AddMember("undoRecords", undoArr, allocator);
    }
    
    // Convert to string
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);
    
    return buffer.GetString();
}

// Helper function implementations
namespace {

LevelCardConfig parseCard(const rapidjson::Value& cardObj) {
    LevelCardConfig card;
    
    if (cardObj.IsObject()) {
        // Try "CardFace" first (level file format), then "face" (serialized format)
        if (cardObj.HasMember("CardFace") && cardObj["CardFace"].IsInt()) {
            card.face = cardObj["CardFace"].GetInt();
        } else if (cardObj.HasMember("face") && cardObj["face"].IsInt()) {
            card.face = cardObj["face"].GetInt();
        }
        
        // Try "CardSuit" first (level file format), then "suit" (serialized format)
        if (cardObj.HasMember("CardSuit") && cardObj["CardSuit"].IsInt()) {
            card.suit = cardObj["CardSuit"].GetInt();
        } else if (cardObj.HasMember("suit") && cardObj["suit"].IsInt()) {
            card.suit = cardObj["suit"].GetInt();
        }
        
        // Try "Position" object first (level file format)
        if (cardObj.HasMember("Position") && cardObj["Position"].IsObject()) {
            const auto& pos = cardObj["Position"];
            if (pos.HasMember("x") && pos["x"].IsNumber()) {
                card.x = static_cast<float>(pos["x"].GetDouble());
            }
            if (pos.HasMember("y") && pos["y"].IsNumber()) {
                card.y = static_cast<float>(pos["y"].GetDouble());
            }
        } 
        // Otherwise try direct x, y (serialized format)
        else {
            if (cardObj.HasMember("x") && cardObj["x"].IsNumber()) {
                card.x = static_cast<float>(cardObj["x"].GetDouble());
            }
            if (cardObj.HasMember("y") && cardObj["y"].IsNumber()) {
                card.y = static_cast<float>(cardObj["y"].GetDouble());
            }
        }
    }
    
    return card;
}

LevelUndoRecordConfig parseUndoRecord(const rapidjson::Value& undoObj) {
    LevelUndoRecordConfig undo;
    
    if (undoObj.IsObject()) {
        if (undoObj.HasMember("cardId") && undoObj["cardId"].IsInt()) {
            undo.cardId = undoObj["cardId"].GetInt();
        }
        if (undoObj.HasMember("from") && undoObj["from"].IsInt()) {
            undo.from = undoObj["from"].GetInt();
        }
        if (undoObj.HasMember("to") && undoObj["to"].IsInt()) {
            undo.to = undoObj["to"].GetInt();
        }
        if (undoObj.HasMember("fromX") && undoObj["fromX"].IsNumber()) {
            undo.fromX = static_cast<float>(undoObj["fromX"].GetDouble());
        }
        if (undoObj.HasMember("fromY") && undoObj["fromY"].IsNumber()) {
            undo.fromY = static_cast<float>(undoObj["fromY"].GetDouble());
        }
        if (undoObj.HasMember("toX") && undoObj["toX"].IsNumber()) {
            undo.toX = static_cast<float>(undoObj["toX"].GetDouble());
        }
        if (undoObj.HasMember("toY") && undoObj["toY"].IsNumber()) {
            undo.toY = static_cast<float>(undoObj["toY"].GetDouble());
        }
        if (undoObj.HasMember("removedCard") && undoObj["removedCard"].IsObject()) {
            undo.removedCard = parseCard(undoObj["removedCard"]);
        }
        if (undoObj.HasMember("previousHand") && undoObj["previousHand"].IsObject()) {
            undo.previousHand = parseCard(undoObj["previousHand"]);
        }
        if (undoObj.HasMember("hadPreviousHand") && undoObj["hadPreviousHand"].IsBool()) {
            undo.hadPreviousHand = undoObj["hadPreviousHand"].GetBool();
        }
    }
    
    return undo;
}

void serializeCard(rapidjson::Value& cardObj, const LevelCardConfig& card, rapidjson::Document::AllocatorType& allocator) {
    // Use level file format: CardFace, CardSuit, Position
    cardObj.AddMember("CardFace", card.face, allocator);
    cardObj.AddMember("CardSuit", card.suit, allocator);
    
    // Create nested Position object
    rapidjson::Value posObj(rapidjson::kObjectType);
    posObj.AddMember("x", card.x, allocator);
    posObj.AddMember("y", card.y, allocator);
    cardObj.AddMember("Position", posObj, allocator);
}

void serializeUndoRecord(rapidjson::Value& undoObj, const LevelUndoRecordConfig& undo, rapidjson::Document::AllocatorType& allocator) {
    undoObj.AddMember("cardId", undo.cardId, allocator);
    undoObj.AddMember("from", undo.from, allocator);
    undoObj.AddMember("to", undo.to, allocator);
    undoObj.AddMember("fromX", undo.fromX, allocator);
    undoObj.AddMember("fromY", undo.fromY, allocator);
    undoObj.AddMember("toX", undo.toX, allocator);
    undoObj.AddMember("toY", undo.toY, allocator);
    
    rapidjson::Value removedCardObj(rapidjson::kObjectType);
    serializeCard(removedCardObj, undo.removedCard, allocator);
    undoObj.AddMember("removedCard", removedCardObj, allocator);
    
    rapidjson::Value previousHandObj(rapidjson::kObjectType);
    serializeCard(previousHandObj, undo.previousHand, allocator);
    undoObj.AddMember("previousHand", previousHandObj, allocator);
    
    undoObj.AddMember("hadPreviousHand", undo.hadPreviousHand, allocator);
}

} // anonymous namespace
