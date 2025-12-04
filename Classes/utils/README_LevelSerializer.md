# LevelSerializer 使用说明

## 功能概述

`LevelSerializer` 提供了完整的关卡配置序列化和反序列化功能，支持：
- Playfield 牌组
- Stack 牌组
- 当前手牌（可选，用于保存游戏进度）
- Undo 记录（可选，用于保存游戏进度）

## 数据结构

### LevelCardConfig
```cpp
struct LevelCardConfig {
    int face;      // 牌面值 (0-12: ACE-KING)
    int suit;      // 花色 (0-3: CLUBS, DIAMONDS, HEARTS, SPADES)
    float x;       // X 坐标
    float y;       // Y 坐标
};
```

### LevelUndoRecordConfig
```cpp
struct LevelUndoRecordConfig {
    int cardId;                      // 牌的 ID
    int from;                        // 来源区域: 0=Playfield, 1=Tray, 2=Stack
    int to;                          // 目标区域: 0=Playfield, 1=Tray, 2=Stack
    float fromX, fromY;              // 起始坐标
    float toX, toY;                  // 目标坐标
    LevelCardConfig removedCard;     // 被移除的牌（用于 Playfield 匹配）
    LevelCardConfig previousHand;    // 之前的手牌
    bool hadPreviousHand;            // 是否有之前的手牌
};
```

### LevelConfig
```cpp
class LevelConfig {
public:
    std::vector<LevelCardConfig> playfield;       // Playfield 牌组
    std::vector<LevelCardConfig> stack;           // Stack 牌组
    LevelCardConfig hand;                         // 当前手牌
    bool hasHand;                                 // 是否有手牌
    std::vector<LevelUndoRecordConfig> undoRecords; // Undo 记录
};
```

## 使用示例

### 1. 反序列化（从 JSON 字符串读取配置）

```cpp
#include "utils/LevelSerializer.h"
#include "configs/loaders/LevelConfigLoader.h"

// 从文件加载 JSON
std::string json = LevelConfigLoader::loadLevelJson(1);

// 反序列化为 LevelConfig
LevelConfig config = LevelSerializer::deserialize(json);

// 访问数据
CCLOG("Playfield cards: %zu", config.playfield.size());
CCLOG("Stack cards: %zu", config.stack.size());
CCLOG("Has hand: %s", config.hasHand ? "yes" : "no");
CCLOG("Undo records: %zu", config.undoRecords.size());
```

### 2. 序列化（保存配置到 JSON 字符串）

```cpp
#include "utils/LevelSerializer.h"

// 创建配置
LevelConfig config;

// 添加 Playfield 牌
LevelCardConfig card1;
card1.face = 12;  // KING
card1.suit = 0;   // CLUBS
card1.x = 250.0f;
card1.y = 1000.0f;
config.playfield.push_back(card1);

// 添加 Stack 牌
LevelCardConfig card2;
card2.face = 2;  // THREE
card2.suit = 0;  // CLUBS
card2.x = 0.0f;
card2.y = 0.0f;
config.stack.push_back(card2);

// 设置手牌（可选，用于保存游戏）
config.hand = card1;
config.hasHand = true;

// 序列化为 JSON（使用标准关卡格式）
std::string json = LevelSerializer::serialize(config);
CCLOG("Serialized JSON: %s", json.c_str());

// 保存到文件
auto fileUtils = cocos2d::FileUtils::getInstance();
std::string path = fileUtils->getWritablePath() + "level1.json";
fileUtils->writeStringToFile(json, path);
```

### 3. 从游戏状态创建关卡文件

```cpp
#include "utils/LevelSerializer.h"
#include "models/GameModel.h"

// 从 GameModel 创建 LevelConfig
LevelConfig createLevelConfigFromGame(const GameModel& game) {
    LevelConfig config;
    
    // 保存 Playfield 牌
    for (const auto& card : game.playfieldModel.cards) {
        LevelCardConfig c;
        c.face = static_cast<int>(card.face);
        c.suit = static_cast<int>(card.suit);
        c.x = card.position.x;
        c.y = card.position.y;
        config.playfield.push_back(c);
    }
    
    // 保存 Stack 牌
    for (const auto& card : game.stack.cards) {
        LevelCardConfig c;
        c.face = static_cast<int>(card.face);
        c.suit = static_cast<int>(card.suit);
        c.x = 0.0f;  // Stack 位置在运行时计算
        c.y = 0.0f;
        config.stack.push_back(c);
    }
    
    // 可选：保存手牌
    if (game.stack.hasHand) {
        config.hand.face = static_cast<int>(game.stack.hand.face);
        config.hand.suit = static_cast<int>(game.stack.hand.suit);
        config.hand.x = game.stack.hand.position.x;
        config.hand.y = game.stack.hand.position.y;
        config.hasHand = true;
    }
    
    return config;
}

// 使用示例：导出关卡
LevelConfig config = createLevelConfigFromGame(gameModel);
std::string json = LevelSerializer::serialize(config);

auto fileUtils = cocos2d::FileUtils::getInstance();
fileUtils->writeStringToFile(json, "Resources/Levels/level_new.json");
```

## JSON 格式示例

### 标准关卡文件格式（序列化输出格式）

序列化输出的 JSON 格式：

```json
{
    "Playfield": [
        {
            "CardFace": 12,
            "CardSuit": 0,
            "Position": {"x": 250, "y": 1000}
        },
        {
            "CardFace": 2,
            "CardSuit": 0,
            "Position": {"x": 300, "y": 800}
        }
    ],
    "Stack": [
        {
            "CardFace": 2,
            "CardSuit": 0,
            "Position": {"x": 0, "y": 0}
        },
        {
            "CardFace": 0,
            "CardSuit": 2,
            "Position": {"x": 0, "y": 0}
        }
    ],
    "hand": {
        "CardFace": 12,
        "CardSuit": 0,
        "Position": {"x": 800, "y": 300}
    },
    "undoRecords": [
        {
            "cardId": 100,
            "from": 2,
            "to": 1,
            "fromX": 200.0,
            "fromY": 300.0,
            "toX": 800.0,
            "toY": 300.0,
            "removedCard": {
                "CardFace": 0,
                "CardSuit": 0,
                "Position": {"x": 0, "y": 0}
            },
            "previousHand": {
                "CardFace": 0,
                "CardSuit": 0,
                "Position": {"x": 0, "y": 0}
            },
            "hadPreviousHand": false
        }
    ]
}
```

**关键特性：**
- ? 使用 `Playfield` 和 `Stack`（首字母大写）
- ? 牌配置使用 `CardFace` 和 `CardSuit`
- ? 位置使用嵌套的 `Position` 对象 `{"x": ..., "y": ...}`
- ? 可选字段 `hand` 和 `undoRecords` 仅在有数据时输出
- ? 与原始关卡文件格式完全兼容

### 反序列化兼容性

`LevelSerializer::deserialize` 同时支持多种格式变体：

#### 变体 1：标准格式（推荐）
```json
{
    "Playfield": [...],
    "Stack": [...]
}
```

#### 变体 2：小写字段名（兼容旧格式）
```json
{
    "playfield": [...],
    "stack": [...]
}
```

#### 变体 3：牌配置格式
支持两种牌配置格式：

**格式 A（标准）：**
```json
{
    "CardFace": 12,
    "CardSuit": 0,
    "Position": {"x": 250, "y": 1000}
}
```

**格式 B（简化）：**
```json
{
    "face": 12,
    "suit": 0,
    "x": 250,
    "y": 1000
}
```

两种格式都能正确解析！

## 注意事项

1. **序列化格式**：`serialize` 方法输出标准关卡格式（`Playfield`、`Stack`、`CardFace`、`CardSuit`、嵌套 `Position`）
2. **反序列化兼容性**：`deserialize` 方法支持多种格式变体，向后兼容
3. **可选字段**：`hand` 和 `undoRecords` 仅在有数据时输出
4. **Position 坐标**: Stack 牌的 Position 通常设为 (0, 0)，因为它们会在运行时根据布局重新定位
5. **JSON 解析错误处理**：如果 JSON 解析失败，会在日志中输出错误信息并返回空的 LevelConfig
6. **内存管理**：使用 RapidJSON 进行高效的 JSON 处理，无需手动管理内存

## CardFace 和 CardSuit 值对照表

### CardFace (牌面)
- 0 = ACE (A)
- 1 = TWO (2)
- 2 = THREE (3)
- 3 = FOUR (4)
- 4 = FIVE (5)
- 5 = SIX (6)
- 6 = SEVEN (7)
- 7 = EIGHT (8)
- 8 = NINE (9)
- 9 = TEN (10)
- 10 = JACK (J)
- 11 = QUEEN (Q)
- 12 = KING (K)

### CardSuit (花色)
- 0 = CLUBS (? 梅花)
- 1 = DIAMONDS (? 方块)
- 2 = HEARTS (? 红心)
- 3 = SPADES (? 黑桃)

## 集成到现有代码

`GameModelFromLevelGenerator` 已经集成了 LevelSerializer：

```cpp
LevelConfig GameModelFromLevelGenerator::parseLevelConfig(const std::string& json) {
    return LevelSerializer::deserialize(json);
}
```

这提供了更完整和可靠的 JSON 解析功能，替代了之前的正则表达式实现。
