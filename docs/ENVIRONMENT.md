# Qt + Cursor 开发环境参考

> 环境已配置完成，**系统环境变量一个都没改**。所有配置都写在项目文件里。
> 本文件是查阅用的参考手册：路径、原理、脚本模板、排错表。

---

## 一、工具链（已实测确认）

| 组件 | 版本 | 路径 |
|---|---|---|
| Qt | **6.11.2** | `D:\Qt_Develop_Tool\6.11.2\mingw_64` |
| 编译器 | **MinGW 13.1.0** (`g++`) | `D:\Qt_Develop_Tool\Tools\mingw1310_64\bin\g++.exe` |
| 调试器 | `gdb` | `D:\Qt_Develop_Tool\Tools\mingw1310_64\bin\gdb.exe` |
| CMake | 3.30.5 | `D:\Qt_Develop_Tool\Tools\CMake_64\bin\cmake.exe` |
| Ninja | — | `D:\Qt_Develop_Tool\Tools\Ninja\ninja.exe` |
| Qt 部署工具 | `windeployqt` | `D:\Qt_Develop_Tool\6.11.2\mingw_64\bin\windeployqt.exe` |
| Qt 安装器目录 | — | `D:\Qt_Develop_Tool`（**不是** `D:\Qt`，那里只有安装器） |
| Cursor | — | `D:\Cursor\Cursor.exe` |
| 备选 IDE | Qt Creator | `D:\Qt_Develop_Tool\Tools\QtCreator\bin\qtcreator.exe` |

**已装的 Cursor 扩展**：`ms-vscode.cmake-tools`、`theqtcompany.qt` / `qt-core` / `qt-cpp` / `qt-qml` / `qt-ui` / `qt-python`。
**缺失**：`ms-vscode.cpptools` —— 没有它 **F5 调试不可用**（`cppdbg` 调试器由它提供）。首次打开项目时 Cursor 会提示安装，装一下即可。

---

## 二、为什么必须配 PATH：一个实测结论

把编译好的 `QtLearn.exe` 在**干净环境**（PATH 不含 Qt 目录）下运行，结果是：

```
EXITCODE=-1073741515        # 即 0xC0000135 = STATUS_DLL_NOT_FOUND
```

原因是 exe 依赖这些 DLL，且它们**不在** exe 旁边：

| DLL | 位置 |
|---|---|
| `Qt6Widgets.dll` / `Qt6Gui.dll` / `Qt6Core.dll` | `6.11.2\mingw_64\bin` |
| `libstdc++-6.dll` / `libgcc_s_seh-1.dll` / `libwinpthread-1.dll` | `6.11.2\mingw_64\bin`（MinGW 运行库，Qt 目录里自带了） |
| `qwindows.dll`（平台插件） | `6.11.2\mingw_64\plugins\platforms` |

**结论：开发阶段运行时，PATH 里必须有 `6.11.2\mingw_64\bin`。**
好消息是**一个目录就够了** —— `mingw_64\bin` 里同时有 Qt 库和 MinGW 运行库。

配 PATH 的三种方式（本项目用的是**方式 1**）：

1. **项目内配置**（本项目采用，推荐）：`.vscode/settings.json` 的 `cmake.envPath` + `.vscode/launch.json` 的 `environment` + 你自己的构建脚本。
2. 系统环境变量：一劳永逸，但污染全局，且可能让其他编译器（`D:\mingw64a`、Strawberry Perl 自带的 cmake/ninja）与 Qt 版本混用 → **本项目的用户明确指出不要这么做，已按此执行**。
3. 部署时把 DLL 拷到 exe 旁边：正式发布用（第 7 天内容）。

---

## 三、配置架构：三个地方各管一段

```
                    ┌─────────────────────────────────────────┐
                    │  CMakePresets.json                      │
                    │  configure/build 时的环境               │
                    │  （preset 的 environment 字段）          │
                    │  → Cursor 里 Ctrl+Shift+B 构建走这里     │
                    └─────────────────────────────────────────┘
                                     │ 管
                    ┌────────────────▼────────────────────────┐
                    │  .vscode/settings.json                  │
                    │  · cmake.cmakePath 用哪一个 cmake        │
                    │  · cmake.envPath   configure/build 环境  │
                    │  · qt-core.*       告诉 Qt 扩展 Qt 在哪  │
                    │  · C_Cpp.*         IntelliSense / 跳转   │
                    └─────────────────────────────────────────┘

                    ┌─────────────────────────────────────────┐
                    │  .vscode/launch.json                    │
                    │  F5 调试时的环境（PATH + 插件路径）       │
                    │  → 这是唯一能给"运行中的 exe"加环境的地方 │
                    └─────────────────────────────────────────┘
```

**关键点：`launch.json` 的 `environment` 是必须的。** CMake Tools 的 `cmake.envPath` 只管 configure/build，管不到你按 F5 启动的那个进程 —— 缺了它程序会以 `0xC0000135` 静默退出。

---

## 四、写你自己脚本的模板

脚本的作用：**让环境只在脚本进程内生效**，跑完即净，不动系统。下面是模板，按你的习惯改。

### `build.bat`（配置 + 构建 Debug）

```bat
@echo off
setlocal
set "QT=D:\Qt_Develop_Tool"
set "PATH=%QT%\6.11.2\mingw_64\bin;%QT%\Tools\mingw1310_64\bin;%QT%\Tools\Ninja;%QT%\Tools\CMake_64\bin;%PATH%"
cd /d "%~dp0"
cmake --preset mingw-debug || exit /b 1
cmake --build --preset mingw-debug || exit /b 1
```

### `run.bat`（构建 + 运行）

```bat
@echo off
setlocal
set "QT=D:\Qt_Develop_Tool"
set "PATH=%QT%\6.11.2\mingw_64\bin;%QT%\Tools\mingw1310_64\bin;%PATH%"
set "QT_QPA_PLATFORM_PLUGIN_PATH=%QT%\6.11.2\mingw_64\plugins\platforms"
cd /d "%~dp0"
call build.bat || exit /b 1
start "" "%~dp0build\mingw-debug\QtLearn.exe"
```

### 要点
- `setlocal` 必须有：`endlocal`（脚本结束）后环境自动还原。
- 应该把**四行 PATH 前缀**当成"项目的一部分"复制到每个脚本里；不想重复就抽一个 `env.bat` 用 `call` 引入。
- 如果你用 PowerShell 而不是 cmd，写法是：
  ```powershell
  $env:PATH = "D:\Qt_Develop_Tool\6.11.2\mingw_64\bin;D:\Qt_Develop_Tool\Tools\mingw1310_64\bin;$env:PATH"
  ```
- **`cmake --preset` 自己会设好环境**（见 `CMakePresets.json` 的 `environment` 字段）。所以如果脚本里只调 cmake 命令，其实连 PATH 前缀都可以省 —— 只有**运行 exe** 时才必须加。

---

## 五、Cursor 里的日常工作流

| 操作 | 方式 |
|---|---|
| 构建 | `Ctrl+Shift+B`（走 `.vscode/tasks.json` 的 `build`），或状态栏 CMake 的 Build 按钮 |
| 调试 | `F5`（走 `launch.json`，自动先构建） |
| 加断点 | 行号左侧点一下；`F10` 单步跳过、`F11` 单步进入、`Shift+F5` 停止 |
| 跳进 Qt 头文件 | `Ctrl+左键` 或 `F12`（跳**定义**）；`Alt+F12` 看定义预览不跳走 |
| 查 Qt 文档 | 悬停在类名/函数上看签名；或 `Ctrl+Shift+P` → 输入 `Qt:` 看 Qt 扩展的命令 |
| 看编译参数 | `build/mingw-debug/compile_commands.json`（`CMAKE_EXPORT_COMPILE_COMMANDS=ON` 已开） |
| 构建目录 | `build/mingw-debug`（Debug）、`build/mingw-release`（Release） |
| 出问题时彻底重建 | `Ctrl+Shift+P` → `Tasks: Run Task` → `clean-rebuild` |

### 首次打开项目要做的事
1. 装缺失的 `ms-vscode.cpptools`（Cursor 会弹推荐，或扩展面板搜 C++ 装 Microsoft 版）。
2. `Ctrl+Shift+P` → `CMake: Select Configure Preset` → 选 `mingw-debug`。
3. 先手动跑一次构建（`Ctrl+Shift+B`），生成 `compile_commands.json` 后 IntelliSense 才会完全准确。
4. 状态栏左下角应显示 kit 与 preset；若显示 `No kit` 之类，检查 `cmake-kits.json` 里的 `isTrusted`（已设为 `true`）。

### 关于 AI 辅助（Cursor 的核心价值 + 它的坑）
- **适合问 AI**：报错信息怎么解、某个 API 的参数含义、布局怎么写、帮我解释这段 Qt 代码。
- **必须警惕**：**AI 的 Qt 知识大量来自 Qt 5**。看到 `SIGNAL()/SLOT()` 宏、`QRegExp`、`qrand()`、`QMouseEvent::pos()`、`QDesktopWidget` 就是 Qt 5 写法，本项目是 Qt 6.11，**直接抄会编译失败或行为不符**。见学习计划附录 C 的版本差异表。
- **正确用法**：让 AI 讲思路，然后自己 `Ctrl+左键` 进头文件核对签名，再写 20 行最小例子跑通。
- 可以在项目里放一个 `.cursor/rules` 或 `.cursorrules` 文件，写上"本项目使用 Qt 6.11.2 + C++17，请只给 Qt6 API，不要用 SIGNAL/SLOT 宏"之类的约束。

---

## 六、常见错误 → 原因 → 解法

| 现象 | 原因 | 解法 |
|---|---|---|
| 双击 exe 一闪而过，退出码 `-1073741515`（`0xC0000135`） | 缺 Qt6/MinGW DLL | PATH 里加 `6.11.2\mingw_64\bin`；发布时用 `windeployqt` |
| 跑起来报 `could not find or load the Qt platform plugin "windows"` | 缺平台插件 | 设 `QT_QPA_PLATFORM_PLUGIN_PATH` 或 `QT_PLUGIN_PATH`；发布时确保 `platforms\qwindows.dll` 在 exe 附近 |
| `CMake Error: could not find CMAKE_CXX_COMPILER` | PATH 里没有 MinGW | 加 `Tools\mingw1310_64\bin` |
| 构建报找不到 `Qt6Config.cmake` | `CMAKE_PREFIX_PATH` 没指向 Qt | 用 preset 构建；确认 `CMAKE_PREFIX_PATH=D:/Qt_Develop_Tool/6.11.2/mingw_64` |
| `error: 'QLineEdit' was not declared` | 缺头文件 | `#include <QLineEdit>` |
| `undefined reference to vtable for X` | 有 `Q_OBJECT` 但 moc 没重跑 | 跑 `clean-rebuild` |
| 链接到了错误的 g++ / cmake / ninja | 系统 PATH 里有 `D:\mingw64a`(GCC 14.2)、Strawberry Perl 的 cmake/ninja | **这正是本方案改 preset/PATH 的原因**：preset 会把正确路径放最前面 |
| Cursor 里 `F5` 提示没有调试器 | 缺 `ms-vscode.cpptools` | 装上即可 |
| IntelliSense 大量红色波浪线但能编译 | `compile_commands.json` 还没生成 | 先构建一次；或 `Ctrl+Shift+P` → `C/C++: Select a Configuration` |

---

## 七、改动清单与还原

### 我实际改了什么
| 文件 | 改动 | 性质 |
|---|---|---|
| `CMakePresets.json` | ① 修正 `$penv{PATH}`（原来丢了 `$`，PATH 里插入了字面量 `penv{PATH}`）② 补上 `QT_PLUGIN_PATH` / `QT_QPA_PLATFORM_PLUGIN_PATH` | 修复 + 配置 |
| `.vscode/settings.json` | 加 `cmake.envPath`（项目内 PATH）、`cmake.cmakePath` 指向 Qt 自带 CMake、确认 `qt-core.*` 指向 Qt 安装 | 配置 |
| `.vscode/launch.json` | 加 `Release` 调试配置、加 gdb 跳过 libstdc++ 单步设置 | 配置 |
| `.vscode/tasks.json` | 改为依赖 preset 环境；新增 `build-release`、`clean-rebuild` | 配置 |
| `.vscode/c_cpp_properties.json` | 补 `include` / `defines` / 配置源 | 配置 |
| `.vscode/extensions.json` | 增加 `ms-vscode.cpptools` 推荐 | 配置 |
| `.vscode/cmake-kits.json` | **未改**（159 行原样保留） | — |
| `main.cpp` / `widget.*` / `CMakeLists.txt` | **未改** | — |

### 没有改什么
- **Windows 系统环境变量（用户级 / 系统级 PATH）：全程未改动。** 过程中曾临时写入过一次用户 PATH，已按你的要求立即还原，并与备份逐字节一致（长度 801，首项 `C:\Users\13368\AppData\Local\kdocs-cli`）。
- 没有删除任何文件。

### 备份位置
**`D:\MyProject\QtLearn\.config-backup\`**

```
cmake-kits.json              原 .vscode 配置
c_cpp_properties.json
extensions.json
launch.json
settings.json
tasks.json
CMakePresets.json.bak        原 CMakePresets.json
user-PATH.before.txt         用户 PATH 原始值（用于核对"确实没改"）
machine-PATH.before.txt      系统 PATH 原始值
```

想回到改动前的状态，把这些文件拷回 `.vscode\` 即可：

```bat
copy /Y .config-backup\*.json .vscode\
copy /Y .config-backup\CMakePresets.json.bak CMakePresets.json
```

> `.config-backup\` 已加入 `settings.json` 的 `files.exclude`，不会干扰编辑器视图。
> `.gitignore` 目前不会忽略它 —— 如果你不想提交，往 `.gitignore` 加一行 `.config-backup/`。

---

## 八、验证记录（配置后实测）

| 检查项 | 结果 |
|---|---|
| `CMakePresets.json` 为严格合法 JSON | ✅ |
| `$penv{PATH}` 正确展开（configure 日志确认） | ✅ 环境变量打印出完整 PATH |
| 干净 PATH 下 `cmake --preset mingw-debug` 配置成功 | ✅ 自动识别 `GNU 13.1.0`、找到 Qt 6.11.2 |
| 干净 PATH 下 `cmake --build --preset mingw-debug` 构建成功 | ✅ 5/5 步通过，链接出 `QtLearn.exe` |
| 用 `launch.json` 同款环境启动 GUI | ✅ 进程存活，窗口标题 `Hello World1` |
| 用**不含 Qt 目录**的 PATH 启动 | ❌ 退出码 `-1073741515` —— 证明 PATH 配置是必要的，不是多余的 |
| `.vscode/*.json` 全部严格合法 JSON | ✅ 6/6 |
| 系统环境变量未被改动 | ✅ 与备份逐字节一致 |
