# Card - Tripeak-like Matching Demo (cocos2dx 3.17)

本项目是基于 cocos2dx 3.17 的纸牌匹配玩法示例，采用 MVC 架构，已实现基础点击匹配、手牌替换以及回退功能。适配分辨率 1080x2080，包含 Windows/Android/iOS/macOS/Linux 的工程与 CMake 构建。

## 概述
- 初始有一张底牌（手牌区顶部牌）。
- 桌面主牌区的翻开牌可点击；被覆盖的牌需先移除上方牌才能翻开。
- 消除规则：点击桌面牌，使其与底牌点数差 1 即可匹配（无花色限制）。例如底牌为 8，可匹配 7 或 9。
- 当主牌区无可匹配时，可从备用牌堆翻牌，替换当前底牌。
- 已实现回退：可按历史顺序撤销最近的点击操作与动画，直至无回退记录。

## 已实现的核心交互
- 需求1：手牌区翻牌替换
  - 点击手牌区某张备用牌（如 ♥A），执行 MoveTo 动画至手牌区顶部位置，替换为新的顶部牌（如 ♣4）。
- 需求2：桌面牌与手牌区顶部牌匹配
  - 点数差 1 即可匹配，无花色限制。点击桌面牌（如 ♦3），平移到手牌区顶部位置并替换顶部牌（如 ♣4）。
- 需求3：回退功能
  - 记录每次点击与移动操作。连续点击回退按钮，卡牌按相反路径 MoveTo 返回原位，直至无回退记录。
 
  ![eee](videos/202512050056.mp4)

## 设计与分辨率
- 设计分辨率：1080 x 2080
- 窗口大小：1080 x 2080
- 初始化示例：
  ```cpp
  auto glview = GLViewImpl::createWithRect("Test", cocos2d::Rect(0, 0, 1080, 2080), 0.5);
  glview->setDesignResolutionSize(1080, 2080, ResolutionPolicy::FIXED_WIDTH);
  ```
- 主牌区尺寸：1080 x 1500
- 堆牌区尺寸：1080 x 580

## 关卡配置示例
`Levels/level1.json`
```json
{
  "Playfield": [
    {"CardFace": 12, "CardSuit": 0, "Position": {"x": 250, "y": 1000}},
    {"CardFace": 2,  "CardSuit": 0, "Position": {"x": 300, "y": 800}},
    {"CardFace": 2,  "CardSuit": 1, "Position": {"x": 350, "y": 600}},
    {"CardFace": 2,  "CardSuit": 0, "Position": {"x": 850, "y": 1000}},
    {"CardFace": 2,  "CardSuit": 0, "Position": {"x": 800, "y": 800}},
    {"CardFace": 1,  "CardSuit": 3, "Position": {"x": 750, "y": 600}}
  ],
  "Stack": [
    {"CardFace": 2, "CardSuit": 0, "Position": {"x": 0, "y": 0}},
    {"CardFace": 0, "CardSuit": 2, "Position": {"x": 0, "y": 0}},
    {"CardFace": 3, "CardSuit": 0, "Position": {"x": 0, "y": 0}}
  ]
}
```
- 花色枚举：`CST_CLUBS/DIAMONDS/HEARTS/SPADES`
- 点数枚举：`CFT_ACE ... CFT_KING`

## 架构（MVC）
代码目录结构见 `Classes/`：
- `models/`：`CardModel`, `GameModel`, `PlayfieldModel`, `StackModel`, `UndoModel`
- `views/`：`CardView`, `GameView`, `PlayfieldView`, `StackView`
- `controllers/`：`GameController`, `PlayfieldController`, `StackController`
- `managers/`：`UndoManager`（非单例，负责记录和执行撤销）
- `services/`：`GameModelFromLevelGenerator`（无状态服务：根据关卡配置生成运行时模型）
- `utils/`：`LevelSerializer`（序列化/反序列化辅助）

交互流程（简要）：
1. View 捕获点击 → 触发已注册的回调到 Controller。
2. Controller 检查规则，先写入撤销记录（`UndoManager`/`UndoModel`），更新 `GameModel`。
3. Controller 调用对应 View 播放动画（如 `GameView::playMatchAnimation`）。

## 构建与运行
本仓库内含多平台工程：

### Windows（Visual Studio）
- 打开 `proj.win32/card.sln`，选择 `Debug.win32` 或 `Release.win32` 配置编译运行。
- 或使用 CMake：
  ```powershell
  # 在仓库根目录
  cmake -S . -B build -G "Visual Studio 17 2022" -A x64
  cmake --build build --config Release
  ```

### Android（Gradle）
- 未配置

### iOS/macOS（Xcode）
- 未配置

### Linux（CMake）
- 未配置

## 运行时入口
- Windows/Win32 示例入口：`proj.win32/main.cpp`

## 撤销设计（Undo）
- 数据：`UndoModel` 存储操作记录（源位置、目标位置、涉及卡牌 ID、动画参数等）。
- 管理器：`UndoManager` 作为 `GameController` 成员，插入、执行撤销；禁止实现单例、禁止反向依赖 Controller。
- 扩展：新增一种撤销类型时，定义对应记录结构与回放策略，`UndoManager` 通过统一接口调度。

## 扩展与维护建议
- 新增卡牌：
  - 在 `GameModel` 中添加卡牌实例与唯一 ID；
  - 在 `PlayfieldView/StackView` 中创建对应 `CardView` 并注册点击回调；
  - 在 `PlayfieldController/StackController` 中实现匹配/替换逻辑与动画；
  - 使用 `LevelSerializer` 更新关卡或运行时保存加载。
- 新增撤销类型：
  - 在 `UndoModel` 中定义新记录字段；
  - 在 `UndoManager` 中添加记录写入与回放实现；
  - 在处理流程中于逻辑变更前插入记录，确保撤销可逆。

## 代码规范
- 命名：类/文件名首字母大写；函数/变量驼峰；私有成员以 `_` 开头；常量前缀 `k`。
- 复杂度：函数 > 50 行需重构；类 > 500 行需重构。
- 责任：模块边界清晰，遵循单一职责；`services` 不持有数据；`managers` 不实现单例、不反向依赖 Controller。

## 资源与配置
- 关卡文件：`Levels/` 目录。
- 纸牌资源：`Resources/`（示例包含 `number/`, `suits/` 子目录）。
