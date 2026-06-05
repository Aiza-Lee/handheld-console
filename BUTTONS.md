# 按钮设计哲学

> 本规范约束**所有非核心玩法的场景间交互**（暂停、失败、成功、模式选择、设置、菜单等）。
> **核心玩法键位**（snake 移动、pong 挡板、tetris 旋转、breakout 发球 等）由各 game 自己决定，**不在本规范下**。

## 1. 核心原则（每按钮 = 一个角色）

| 按钮 | 角色 | 不允许的用法 |
|---|---|---|
| **A** | **确认 / 主操作**：执行高亮项、当前 modal 的 "OK" | 不做 cancel、不做 system 动作 |
| **B** | **返回 / 取消**：上一级 screen / dismiss 当前 modal | 不做 confirm、不做主操作 |
| **START** | **系统**：全局暂停（任何 game phase 一律暂停） | 不做"重启"以外的语义 |
| **SELECT** | **次要**：mode 切换 / guide / 副选项（可选） | 不做主导航、不做 confirm |
| **D-pad** | **方向 / 移动**：4 方向导航 + in-game 移动 | 不做"确认" |

**口诀**：A 选 / B 退 / START 停 / SELECT 辅 / D-pad 移。

## 2. 各场景标准交互

| 场景 | UP/DOWN | LEFT/RIGHT | A | B | START | SELECT |
|---|---|---|---|---|---|---|
| **主菜单** | 切换项 | — | 选中 | 上一级 / 根无操作 | 选中（a11y） | — |
| **暂停覆盖层** | — | — | 继续 | 回 menu | 继续 | — |
| **游戏结束覆盖层** | — | — | 重玩 | 回 menu | 重玩（a11y） | — |
| **模式选择页** | — | 切换选项 | 确认 | 回 menu | 确认 | — |
| **设置** | 切换项 | 调整值 | 调整值 | 保存退出 | 保存退出 | — |
| **Boot** | — | — | 进入 menu | — | 进入 menu | — |

**a11y 副键**：A 和 START 在菜单 / modal 中可互换（都是 confirm / resume / restart），让单手玩家也能操作。

## 3. 不可侵犯的核心玩法区

本规范**只约束场景间共性交互**。各 game 内部玩法按键：

- 不被本规范约束
- 不应在 modal / menu 中被覆盖
- 例外：Pong 的 A 键在 PLAY 阶段是"P1 paddle up"（in-game 移动），**但** Pong 的 modal 仍按本规范——A 在 SERVE / MODE_SELECT / PAUSE 中意为"确认"

## 4. 冲突解决原则

1. **核心原则优先**：A=confirm / B=back / START=pause 不能动摇
2. **核心玩法可破例**：某 game 内部需要 A 做"发球" / "fire" 等，OK；但该 game 的 modal 仍按规范
3. **a11y 双键**：菜单里 A 和 START 都能 confirm；游戏内不混用

## 5. 错误用法（反例）

| 反例 | 错在哪 |
|---|---|
| Game over 时按 A 调出"再来一次 / 返回 menu"两个选项 | A 应该是直接重玩，不是再选 |
| 设置时按 A 选中项目 | A 不该做导航；UP/DOWN 导航 |
| Tetris 中 START=硬降 | START 应只做"系统" |
| 暂停时按 SELECT 切换 mode | 暂停时 D-pad/SLECT 应冻结 |
| 模式选择中按 B 后回到 menu 再回来，mode 选回默认 | 应该记住玩家上次选择 |

## 6. 实现约束

- 在所有 `update()` / `render()` 中，按本规范**双键 a11y**（A 和 START 等价）实现
- overlay 文字用 `A/START: ...` 表达双键一致
- 每个 screen 的 BUTTONS.md 偏离需在 `BUTTONS.md` 的 § 7 列出例外（**当前无例外**）

## 7. 当前偏离（实时更新）

| Screen | 例外按键 | 原因 |
|---|---|---|
| Playground | `START` = 切 mode（不做"系统暂停"） | D-pad 是 5 个 mode 的瞄准器（核心玩法），与 `SELECT`=guide 一起占用了可作为"系统暂停"的键。无全局暂停 |

## 8. 测试

- 手动 SDL（`./build/clang-debug/host-sim-sdl`）：进入每个 game 的暂停 / 失败 / 模式选择 / 设置，验证按键表现
- 自动化覆盖：现有 smoke test 只验"不崩"，**不**验按键哲学
