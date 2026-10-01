# Qt 国庆 8 天冲刺：ACM 选手 · Cursor 开发

> **目标（验收标准）**：10 月 8 日结束时，你能在 Cursor 里独立地从零搭出一个带界面的 Qt 程序，遇到不会的 API 时知道怎么查、怎么验证、怎么沉淀成自己的笔记。
>
> **环境已配置完成**：Qt 6.11.2 + MinGW 13.1.0 + Ninja + CMake，全部写在项目配置里，**没有改动任何 Windows 环境变量**。
> 具体路径、原理、脚本模板、排错表 → 见 [docs/ENVIRONMENT.md](docs/ENVIRONMENT.md)。
>
> **构建脚本由你自己写** —— 本文只给模板和要点，不给现成脚本。

---

## 0. 你的起点

### 0.1 你已经有的（不要浪费时间学）
- **C++ 语言本身**：模板、RAII、STL、引用语义、编译错误定位 —— 跳过所有 C++ 语法章节。
- **算法与复杂度直觉**：性能估算、边界条件 —— 这是很多培训班学员的短板，是你的加分项。
- **调试耐心**：面对 WA 时二分定位的能力，迁移到 UI bug 上就是最大优势。

### 0.2 你真正要跨的坎（本计划的重心）

| ACM 的直觉 | Qt 桌面开发的现实 |
|---|---|
| 程序从上到下线性跑完 | **事件循环**：`main` 里 `exec()` 一进去就再也不返回，所有代码都是"被回调"的 |
| 数据是数据，算法是算法 | **对象树 + 父子关系**：内存管理靠 `parent`，`new` 了不用 `delete` |
| 输出靠 `printf` / 返回值 | 通信靠**信号槽**，一个动作触发一串响应，且默认是**同步直连** |
| 一个 `main.cpp` 两千行 | **多文件 + 构建系统**：改一个字要重新构建 |
| 程序是给人判题的 | 程序是**给人点的**：布局要自适应、窗口要能缩放、操作要能撤销 |
| 暴力循环无所谓 | **UI 线程不能被阻塞**：一个 3 秒的循环会让窗口"卡死"变白 |

> **一句话**：ACM 训练你"把一段计算做对"，Qt 训练你"让一个长期运行、响应外部输入的对象系统保持正确"。8 天里请刻意把注意力从"算法"转到"**生命周期、消息流、状态同步**"这三个词上。

### 0.3 技术路线选择（重要，别走弯路）

- **8 天走 Qt Widgets（C++），不碰 QML。** Widgets 用你已有的 C++ 能力直接发力，反馈快、能立刻做出"像那么回事"的应用；QML 是另一套声明式语言，8 天内同时学两套会两头空。QML 放到国庆后第 2 周。
- **C++17 + CMake + Ninja**，在 **Cursor** 里开发（`F5` 调试、`Ctrl+Shift+B` 构建）。

---

## 1. 每天的时间块（建议 6.5 小时有效时间）

| 时段 | 时长 | 内容 |
|---|---|---|
| 09:00–12:00 | 3h | **学 + 写最小例子** |
| 14:00–17:30 | 3.5h | **推进当天项目** |
| 20:00–21:00 | 1h | **写日志 + git 提交 + 明天预热**（必做，不许省） |

> 晚上这 1 小时是"自主拓展闭环"能成立的关键。**没有它，8 天后你只是"跟着做过一遍"；有了它，你才有自己的知识库。**

每天开工前 5 分钟：打开昨天的 `docs/log/dayN.md`，把昨天没解决的 3 个问题抄到纸上，今天解决掉。

---

## 2. 八天总览

| 天 | 日期 | 主题 | 当天可验收产出 |
|---|---|---|---|
| D1 | 10/01 周四 | Cursor 开发闭环：构建、调试、脚本 | 一键构建脚本跑通；F5 断点命中；git 首次提交 |
| D2 | 10/02 周五 | Qt 核心机制：对象树、信号槽、事件循环、布局 | 文本框实时联动窗口 + 自适应计算器布局 |
| D3 | 10/03 周六 | 自定义控件与绘制：QPainter、事件、定时器 | 自己写的可交互画板/波形控件 |
| D4 | 10/04 周日 | 数据与视图、持久化 | `QAbstractTableModel` 渲染真实数据 + 配置/文件读写 |
| D5 | 10/05 周一 | 主窗口架构、多线程、网络 | 带菜单/工具栏/状态栏、后台任务不卡界面 |
| D6 | 10/06 周二 | **综合项目日 1**：从零搭骨架 | 项目能跑通主流程（一条垂直切片） |
| D7 | 10/07 周三 | **综合项目日 2** + 工程化发布 | 功能完整 + 打包成能拷给别人的程序 |
| D8 | 10/08 周四 | 复盘、建闭环、规划后续 | 私人 API 速查表 + 项目模板仓库 + 4 周路线图 |

**缓冲策略**：D1–D5 是地基，宁可往后挤也要打牢；D6/D7 项目落后就砍功能，不砍"跑通"。见第 11 节。

---

## 3. D1（10/01 周四）把 Cursor 变成你的开发主场

**目标**：拥有"改一行代码 → 3 秒内看到界面变化"的快循环，并使用**你自己写的脚本**。这是后面 7 天的效率乘数。

> 环境配置、路径清单、脚本要点已在 [docs/ENVIRONMENT.md](docs/ENVIRONMENT.md) 写好。今天你只需要**验证它 + 把它变成自己的习惯**。

### 3.1 上午：验证环境（约 1h）

**⓪ 先看一眼工作区状态（1 分钟）**
```bat
git status
```
你应该看到 `.vscode/` 下 5 个文件、`CMakePresets.json`、`.gitignore` 是已修改（`M`），`docs/`、`QT-8-DAY-PLAN.md` 是未跟踪（`??`）。
**`main.cpp` / `widget.*` / `CMakeLists.txt` 应该是干净的** —— 如果它们显示被修改，先用 `git diff <文件>` 看清改了什么再决定（想退回原始版本用 `git checkout -- <文件>`）。
> 养成这个习惯：**动手前先确认工作区是干净的**，否则之后出了问题你分不清是自己改坏的还是本来就坏的。

**① 打开项目**
用 Cursor 打开 `D:\MyProject\QtLearn` 文件夹（不是打开单个文件）。首次会提示安装推荐扩展 —— **装上 `ms-vscode.cpptools`**，没有它 `F5` 不能用。

**② 选 preset**
`Ctrl+Shift+P` → `CMake: Select Configure Preset` → 选 `mingw-debug`。

**③ 先构建一次**
`Ctrl+Shift+B`。预期成功，产出 `build\mingw-debug\QtLearn.exe`。这一步还会生成 `compile_commands.json`，之后 IntelliSense 和 `Ctrl+左键` 跳转才准确。

**④ 验证"窗口能起来"**
这是今天第一个关键实验。在 **不含 Qt 目录的 PATH** 下直接跑 exe：
```powershell
$env:PATH = "C:\Windows\System32;C:\Windows"
.\build\mingw-debug\QtLearn.exe
```
你**应该**看到它立刻退出，`$LASTEXITCODE` 是 **`-1073741515`**（即 `0xC0000135`，缺 DLL）。
再用正确环境跑：
```powershell
$env:PATH = "D:\Qt_Develop_Tool\6.11.2\mingw_64\bin;D:\Qt_Develop_Tool\Tools\mingw1310_64\bin;$env:PATH"
.\build\mingw-debug\QtLearn.exe
```
窗口正常弹出。
> **这个对比是你今天最该记住的一件事**：Qt 程序"跑不起来"的第一大原因就是运行库找不到，而且错误码看起来毫无信息量。以后遇到 `-1073741515`，你第一反应就是查 PATH —— 别人要踩三次坑才记得住，你一次就够。

**⑤ 验证断点调试**
在 `Widget` 构造函数（`widget.cpp` 第 3 行）打个断点，`F5`。应该停在断点。**观察调用栈**：`Widget::Widget` 被 `main` 直接调用。先记住这点，D2 你会看到一个完全不同的调用栈（被事件循环调用的）。

### 3.2 下午：写你自己的构建脚本（约 2.5h）

**需求**：你要在 Cursor 的终端里一句话完成"构建 + 运行"，且**不依赖任何系统环境变量**。

**约束（这就是练习的价值所在）**：
1. 环境变量只在脚本进程内生效，脚本结束即还原（cmd 用 `setlocal`）。
2. 至少两个脚本：一个只管构建，一个构建后运行。
3. 构建走 `cmake --preset mingw-debug`，不要手写 `-DCMAKE_PREFIX_PATH=...`（preset 里已经有了，重复配置是坏味道）。
4. **运行 exe 的那一步必须自己设 PATH** —— 否则就是 3.1 ④ 里的 `-1073741515`。
5. 构建失败要能看出来（`exit /b` 非零），不要让脚本"失败了还显示成功"。

**模板和要点见** [docs/ENVIRONMENT.md 第四节](docs/ENVIRONMENT.md)。**先自己写，写不出来再看。**

**为什么让你自己写而不是我塞给你一个**：脚本是你和构建系统之间的接口。自己写过一遍，你才知道 PATH、`setlocal`、退出码分别解决什么问题；以后换机器、换 Qt 版本、加第三方库（比如 OpenCV、QtCharts），你就知道该改哪一行。**这是"进入自主拓展"的第一个练习。**

**可选加餐**：把脚本改成"增量构建 + 只有源文件变了才重新链接"，或者加一个 `-r`（release）参数。测试一下 `cmake --build` 在没有改动时是不是真的什么都不做（你会发现它输出 `ninja: no work to do`）。

### 3.3 给 `main.cpp` 做一次"体检"（15 分钟）

`main.cpp` 当前是 git HEAD 里的原始状态（我已把它恢复原样，没有替你改）。它现在**能编译能运行**，但有 4 个值得注意的地方。**不要直接删，先想清楚每处为什么：**

```cpp
12:    w.setFont(QFont("Arial", 12));
13:    w.setWindowIcon(QIcon(":/icon.png"));
14:    w.setWindowFlags(Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
15:    w.setWindowFlags(Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
```

**① 第 12、13 行用了 `QFont`/`QIcon` 却没有 `#include` 它们，为什么能编译过？**
因为 `<QApplication>` 通过包含链间接引入了它们（`<QApplication>` → `<QGuiApplication>` → `<QFont>`）。
**结论：不要依赖这个。** 这是实现细节，Qt 换个小版本或换平台就可能失效。用到某个类就主动 `#include` 它的头文件。
> 记住这条经验：**能编译过 ≠ 依赖关系正确。** 这也是一个很好的练习 —— 你自己动手删掉 `<QApplication>` 换成 `<QWidget>` 试试，看它是不是立刻编译失败，从而亲手确认这个包含链存在。

**② 第 14、15 行完全重复了**
`setWindowFlags` 是**有副作用**的：它会隐藏窗口，必须再 `show()` 才出现。重复调用在简单场景下看着没事，复杂场景（比如先 `show()` 再改 flags）会导致窗口不显示或位置跳变。**这是"重复调用有副作用的 setter"这类 bug 的入门样本** —— 删掉一行。

**③ 第 13 行 `QIcon(":/icon.png")` 引用了不存在的资源**
项目里没有 `.qrc` 文件，运行时会提示图标无效。今天先注释掉，D7 学资源系统时正式加回来。

**④ 第 17 行 `return QApplication::exec();`**
用类名调用静态函数是合法的，效果和 `a.exec()` 一样。但写 `a.exec()` 更清楚 —— 它是在**那个具体的 application 对象**上跑事件循环。可以改成 `a.exec()`，也可以留着。

**自己动手改这几处，然后重新构建运行确认没坏。** 这是你第一次"读别人的代码 → 判断 → 修改 → 验证"的完整练习，比照抄教程有价值得多。

### 3.4 建立 git 习惯（20 分钟）
```bat
git add -A
git commit -m "D1: verify toolchain, add build scripts"
```
每天结束提交一次，message 用 `D<N>: 一句话`。你的仓库已有历史，保持下去。

### 3.5 Cursor 的三个必备操作（今天就要练熟）

| 操作 | 快捷键 | 用途 |
|---|---|---|
| 跳转到**定义** | `Ctrl+左键` / `F12` | **进 Qt 头文件看真实签名** —— 你最重要的自学习惯 |
| 预览定义（不跳走） | `Alt+F12` | 快速看一眼参数，不打断思路 |
| 找符号 | `Ctrl+T` | 在当前项目里搜类/函数名 |

**练习**：`Ctrl+左键` 点进 `setWindowTitle`，看它在哪个头文件、参数是什么类型。**Qt 头文件本身就是最好的文档。**

### ✅ D1 验收清单
- [ ] `Ctrl+Shift+B` 构建成功
- [ ] 自己写的构建脚本能在 Cursor 终端里跑通
- [ ] 运行脚本能弹出窗口，标题 `Hello World1`
- [ ] 亲手复现过 `-1073741515`，并知道它意味着什么
- [ ] `F5` 能在 `Widget` 构造函数断点命中
- [ ] 能用自己的话解释：为什么 `QApplication a(argc, argv);` 必须是 `main` 里第一个对象
- [ ] `git log` 能看到 D1 的提交

> **如果今天只完成一半**：唯一不可妥协的是"改代码 → 看到界面变化"这个循环。

---

## 4. D2（10/02 周五）Qt 核心机制 —— 理解"对象系统"，而不是背 API

**目标**：搞懂 Qt 的三大机制，此后所有 API 都只是这三大机制的应用。

### 4.1 三个必须彻底搞懂的概念

**① QObject 对象树与所有权**
```cpp
auto *label = new QLabel("hi", this);   // 指定 parent = this，这是"正确"写法，不是泄漏
```
规则：**一个 QObject 指定了 parent，生命周期就交给了 parent。** 你需要能回答：
- 为什么 `Widget w;` 在栈上、它的子控件在堆上，却都不会泄漏？
- 什么时候**不能**用 parent 机制？（子对象生命周期比 parent 长时，比如要 `moveToThread` 的对象）
- `delete` 一个 QObject 时孩子会怎样？（一起被删，并从 parent 的孩子列表移除）

**② 信号槽（signals & slots）—— 你以后 90% 的时间都在写这个**
```cpp
// 新语法（Qt5+ 推荐）：编译期检查类型
connect(ui->lineEdit, &QLineEdit::textChanged, this, &Widget::onTextChanged);

// 老语法，别用（字符串匹配，拼错了运行时才发现）
connect(ui->lineEdit, SIGNAL(textChanged(QString)), this, SLOT(onTextChanged(QString)));
```
必须掌握的 4 件事：
1. **第 5 个参数**：默认 `Qt::AutoConnection`（同线程 = 同步直连，**信号发出后槽立即执行完才返回**）。这个"同步"语义和直觉不同，今天务必用断点验证：在发射信号那行和槽函数里各打一个断点，看调用栈是不是嵌套的。
2. **`Q_OBJECT` 宏**：加了才能用信号槽；改了信号槽要重新构建（moc 重新生成）。忘了加 → `undefined reference to vtable`。
3. **`emit` 是空宏**，写不写编译结果一样，写它是给人看的。
4. **槽没有返回值**（有也会被忽略）。要"取回结果"就用参数引用/指针，或用信号回传。

**③ 事件循环与事件处理**
```cpp
int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    Widget w;
    w.show();
    return a.exec();     // ← 程序从此"住"在这个循环里
}
```
- `exec()` 内部是消息队列循环：取事件 → 分发到 widget 的 `event()` → 可能再交给 `mousePressEvent()` 等虚函数。
- **重写事件处理函数 = 在事件循环的分发路径上插一脚**：`void mousePressEvent(QMouseEvent *e) override;`
- 事件可**拦截/放行**：`e->accept()` / `e->ignore()`，配合 `eventFilter`。这是很多"为什么我的点击没反应"的答案。

### 4.2 布局系统 —— 别再用绝对坐标

**铁律：控件不用 `setGeometry` 摆位置，全部用布局管理器。**
（`main.cpp` 里 `w.setGeometry(100,100,800,600)` 设的是**窗口自身**的位置和大小，这是对的；但**窗口内的控件**不允许绝对定位。）

| 布局 | 用途 |
|---|---|
| `QHBoxLayout` / `QVBoxLayout` | 水平 / 垂直排列 |
| `QGridLayout` | 网格，做计算器键盘最合适 |
| `QFormLayout` | 表单（左标签右输入框） |
| `QStackedLayout` | 多页面切换（配合菜单/侧边栏） |

关键 API：`addWidget`、`addLayout`、`addStretch`（弹簧）、`setContentsMargins`、`setSpacing`、`setStretchFactor`。

### 4.3 上午动手（3h）：最小例子，每个 15 分钟

**例 1 —— 信号槽与实时联动**（必做）
```cpp
auto *layout = new QVBoxLayout(this);
auto *edit   = new QLineEdit(this);
auto *label  = new QLabel(this);
layout->addWidget(edit);
layout->addWidget(label);

connect(edit, &QLineEdit::textChanged, this, [label](const QString &text){
    label->setText(QString("长度: %1 | 内容: %2").arg(text.size()).arg(text));
});
```
**验证点**：每敲一个字符标签立刻变。然后**故意**把 `textChanged` 换成 `textEdited`，观察区别（后者只在用户手动输入时触发，`setText` 不触发）。
> 这种"改一个字看差异"的练习，就是你以后自学 API 的标准动作。

**例 2 —— 定时器与 QTime**
```cpp
auto *timer = new QTimer(this);
connect(timer, &QTimer::timeout, this, [label]{
    label->setText(QTime::currentTime().toString("hh:mm:ss"));
});
timer->start(1000);
```
**验证点**：秒针每秒跳一次。然后想清楚：**为什么这个每秒一次的循环不会像 `while(true){}` 那样卡死界面？**

### 4.4 下午动手（3.5h）：布局练习 + 第一个真界面
1. **布局四连练**（1h）：H/V/Grid/Form 各做一个小窗口，都要求**拉动窗口边缘时控件自适应**，不重叠、不留大片空白。
2. **计算器界面**（2.5h）：`QGridLayout` 摆 4×4 按钮，**只做界面不实现运算**，把布局手感练出来。
3. 进阶：把按钮点击连到 lambda，把表达式拼进 `QLineEdit`。

### 4.5 卡住时的排查顺序
1. `error: 'QLineEdit' was not declared` → **缺 `#include <QLineEdit>`**（最高频错误）
2. `undefined reference to vtable for Widget` → 有 `Q_OBJECT` 但 moc 没重跑 → 跑 `clean-rebuild` 任务
3. 点击按钮没反应 → 检查 `connect` 参数类型；在槽里 `qDebug() << "clicked";` 打点确认有没有进去

### ✅ D2 验收清单
- [ ] 能用自己的话解释"对象树如何管理内存"并举反例
- [ ] 能用**新语法**写 `connect`，知道第 5 个参数的作用
- [ ] 用断点证明过"同线程信号槽是同步调用"
- [ ] 计算器界面拉动窗口时能正确自适应
- [ ] `docs/log/day2.md` 记录至少 2 个踩过的坑

**今晚必做（1h）**：建 `docs/templates/`，写下你的**第一个"最小可编译片段"模板**：空 `QWidget` + 布局 + 一个信号槽。以后每学一个新 API，往这个模板里加一个函数。

---

## 5. D3（10/03 周六）自定义控件与绘制 —— 从"拼控件"到"造控件"

**目标**：现成控件不够用时，你能自己画。这是"会用 Qt"和"能做产品"的分水岭。

### 5.1 QPainter 与 paintEvent

```cpp
class WaveWidget : public QWidget {
    Q_OBJECT
public:
    explicit WaveWidget(QWidget *parent = nullptr) : QWidget(parent) {
        setMinimumSize(400, 200);
        setMouseTracking(true);          // 不按键也收 mouseMoveEvent
    }
protected:
    void paintEvent(QPaintEvent *) override {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        p.fillRect(rect(), QColor("#1e1e1e"));
        p.setPen(QPen(QColor("#4ec9b0"), 2));

        QPainterPath path;
        for (int x = 0; x < width(); ++x) {
            const double y = height() / 2.0 + 60 * std::sin((x + m_phase) * 0.02);
            if (x == 0) path.moveTo(x, y); else path.lineTo(x, y);
        }
        p.drawPath(path);
    }
private:
    double m_phase = 0.0;
};
```

必须理解的 5 点：
1. **`paintEvent` 不是"你调用"的**，是 Qt 在需要重绘时调用。改画面 = 改状态 + 调 `update()` 请求重绘。
2. **坐标系**：原点左上，y 轴**向下**（和数学坐标相反）。画图表时用 `p.translate() / p.scale(1, -1)` 翻转。
3. **`QPainter` 必须在 `paintEvent` 内创建，且只能在你自己的控件上画。**
4. **性能**：`paintEvent` 里不做重计算、不 `new`。要缓存就缓存到成员变量。
5. **`QPen` 画线/边框，`QBrush` 填充，`QPainterPath` 画复杂路径，`QLinearGradient` 做渐变。**

### 5.2 输入事件

| 事件 | 场景 |
|---|---|
| `mousePressEvent` / `mouseMoveEvent` / `mouseReleaseEvent` | 拖拽、画板、选中 |
| `keyPressEvent` / `keyReleaseEvent` | 快捷键、游戏输入 |
| `wheelEvent` | 缩放（图表必备） |
| `resizeEvent` | 自适应重算缓存 |
| `enterEvent` / `leaveEvent` | 悬停高亮 |
| `dragEnterEvent` / `dropEvent` | 拖放文件（配 `setAcceptDrops(true)`） |

常用查询：`e->position()`（Qt6，返回 `QPointF`）、`e->button()`、`e->modifiers()`、`e->angleDelta()`。

> **Qt5 → Qt6 的坑**：`QMouseEvent::pos()` 在 Qt6 已废弃，改用 `position()` / `globalPosition()`。网上老教程大量用 `pos()`。这是你查资料时最容易踩的版本坑。

### 5.3 定时器驱动动画
```cpp
auto *t = new QTimer(this);
connect(t, &QTimer::timeout, this, [this]{ m_phase += 0.1; update(); });
t->start(16);                 // ≈ 60 FPS
```
**思考题**：为什么用 `QTimer` 而不是 `while(true){ m_phase += 0.1; repaint(); }`？（回顾 D2 的事件循环）

### 5.4 动手任务（今天的核心产出）

**主任务：鼠标画板**
- 需求：按住左键拖动留笔迹；可切颜色和线宽；`Ctrl+Z` 撤销上一笔；窗口缩放时笔迹跟着重绘而不消失。
- **关键设计点（先自己想 10 分钟再动手）**：笔迹不能只画在屏幕上，必须把**每一笔**存进 `QVector<QVector<QPointF>> m_strokes`，在 `paintEvent` 里全量重绘。否则最小化再恢复，笔迹就没了。
- 这个"**状态与显示分离**"的思想，就是 D4 学 Model/View 的伏笔。

**备选/加餐：实时波形控件** —— `QTimer` 每 16ms 往环形缓冲区塞一个数，`paintEvent` 画最近 200 个点，`wheelEvent` 调纵轴缩放。

### ✅ D3 验收清单
- [ ] 能独立写出一个继承 `QWidget`、重写 `paintEvent` 的自定义控件
- [ ] 能解释 `update()` 和 `repaint()` 的区别（一个异步合并、一个立即同步）
- [ ] 画板最小化再恢复，笔迹不丢
- [ ] 知道 Qt6 里 `QMouseEvent::pos()` 已被 `position()` 取代
- [ ] `docs/log/day3.md` + 提交

---

## 6. D4（10/04 周日）数据与视图 + 持久化 —— 把 ACM 的数据处理能力接进来

**目标**：让界面显示**真实数据**，能存到磁盘、下次打开还在。这一天你会明显感到 ACM 背景开始发挥优势。

### 6.1 三级递进：从 QListWidget 到自定义 Model

**第一级：`QListWidget`（5 分钟上手，只适合小规模）**
```cpp
auto *list = new QListWidget(this);
list->addItem("第一项");
connect(list, &QListWidget::itemClicked, this, [](QListWidgetItem *it){
    qDebug() << it->text();
});
```

**第二级：`QTableView` + `QStandardItemModel`（中小规模、结构规整）**
```cpp
auto *model = new QStandardItemModel(0, 3, this);
model->setHorizontalHeaderLabels({"名称", "大小", "修改时间"});
model->appendRow({new QStandardItem("a.txt"),
                  new QStandardItem("1.2 KB"),
                  new QStandardItem("2026-10-04 10:00")});
auto *view = new QTableView(this);
view->setModel(model);
view->setSelectionBehavior(QAbstractItemView::SelectRows);
view->horizontalHeader()->setStretchLastSection(true);
```

**第三级：自定义 `QAbstractTableModel`（专业做法，必学）**
```cpp
class ProblemModel : public QAbstractTableModel {
    Q_OBJECT
public:
    int rowCount(const QModelIndex & = {}) const override { return m_rows.size(); }
    int columnCount(const QModelIndex & = {}) const override { return 4; }

    QVariant data(const QModelIndex &idx, int role) const override {
        if (role != Qt::DisplayRole) return {};
        const auto &r = m_rows[idx.row()];
        switch (idx.column()) {
        case 0: return r.name;
        case 1: return r.difficulty;
        case 2: return r.solved ? "✔" : "✘";
        case 3: return r.tags.join(", ");
        }
        return {};
    }
    QVariant headerData(int s, Qt::Orientation o, int role) const override {
        if (role != Qt::DisplayRole || o != Qt::Horizontal) return {};
        static const char *h[] = {"题目", "难度", "状态", "标签"};
        return h[s];
    }
private:
    struct Row { QString name; int difficulty; bool solved; QStringList tags; };
    QVector<Row> m_rows;
};
```

**为什么值得学自定义 Model**（用你熟悉的话说）：Model 就是"**视图对数据的只读接口 + 变更通知协议**"。视图不持有数据，只按 `index` 问 Model 要显示什么。你的算法能力正好用在 `data()` 里做格式化、排序、过滤（配 `QSortFilterProxyModel`）。

> **关键**：要更新界面时**不要**去操作 `view`，而是改数据 + 发信号（`dataChanged` / `beginInsertRows`+`endInsertRows` / `beginResetModel`+`endResetModel`）。**忘了配对调用 `begin/end`，程序会直接崩或界面错乱** —— 自定义 Model 最经典的 bug。

### 6.2 持久化

| 需求 | 方案 |
|---|---|
| 记住窗口大小、上次目录、用户偏好 | `QSettings` |
| 存结构化数据 | `QJsonDocument` + `QJsonObject` |
| 读写文本/日志 | `QFile` + `QTextStream` |
| 选文件 | `QFileDialog::getOpenFileName` |
| 路径拼接 | `QDir` / `QFileInfo`（**永远不要手拼 `\\`**） |

```cpp
// QSettings
QSettings s("MyCompany", "QtLearn");
s.setValue("window/geometry", saveGeometry());
// 下次启动
restoreGeometry(s.value("window/geometry").toByteArray());

// JSON
QJsonArray arr;
for (const auto &r : m_rows)
    arr.append(QJsonObject{ {"name", r.name}, {"difficulty", r.difficulty}, {"solved", r.solved} });
QFile f("data.json");
if (f.open(QIODevice::WriteOnly))
    f.write(QJsonDocument(arr).toJson(QJsonDocument::Indented));
```

### 6.3 动手任务：把 ACM 训练记录搬进 Qt（推荐主题）

**做一个"训练记录管理器"**：
1. 自定义 `QAbstractTableModel` 展示：题目名 / 难度 / 是否 AC / 标签。
2. `QSortFilterProxyModel` 实现按标签过滤 + 点表头排序。
3. `QSettings` 记住窗口尺寸和上次打开的路径。
4. JSON 保存/读取全部记录，重启程序数据还在。
5. 加"统计"标签页：总题数、AC 率、难度分布（先用 `QLabel` 显示文字，图表明天做）。

> **为什么选这个主题**：数据结构和算法你天然熟悉，能把全部注意力放在 **Model/View 协议**这个新东西上。而且这是**你自己会真的用**的工具 —— 项目有真实动机，才撑得住 8 天。

### ✅ D4 验收清单
- [ ] 能独立写出自定义 `QAbstractTableModel`，正确实现四个虚函数
- [ ] 知道什么时候用 `dataChanged`，什么时候用 `begin/endInsertRows`
- [ ] 程序关掉再开，数据还在（JSON + QSettings 都生效）
- [ ] 能把 "ACM 的 vector/排序/去重" 迁移到 `QVector` + `QSortFilterProxyModel`
- [ ] `docs/log/day4.md` + 提交

---

## 7. D5（10/05 周一）主窗口架构、多线程、网络

**目标**：把"一个窗口"升级成"一个像样的应用程序"，并解决 UI 卡顿这个 ACM 选手最易踩的坑。

### 7.1 QMainWindow 三件套

```cpp
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow() {
        auto *fileMenu = menuBar()->addMenu("文件(&F)");
        auto *openAct  = fileMenu->addAction("打开(&O)...");
        openAct->setShortcut(QKeySequence::Open);
        connect(openAct, &QAction::triggered, this, &MainWindow::onOpen);

        auto *tb = addToolBar("常用");
        tb->addAction(openAct);

        statusBar()->showMessage("就绪");
    }
};
```
> **Qt6 注意**：`QAction` 现在 `#include <QAction>`（属 QtGui）。Qt5 里它在 QtWidgets，头文件路径变过，老教程会误导你。

**中央区域**：`QTabWidget`（最省事）或 `QStackedWidget` + 侧边列表（更像现代应用）。

### 7.2 多线程 —— 今天最重要的一节

**先做实验感受问题**（10 分钟，必做）：
```cpp
connect(btn, &QPushButton::clicked, this, [label]{
    qint64 sum = 0;
    for (qint64 i = 0; i < 3000000000LL; ++i) sum += i;   // 故意卡住
    label->setText(QString::number(sum));
});
```
点下去 → **窗口变白、拖不动、标题栏显示"无响应"**。把现象记在心里，这是桌面开发最核心的工程约束。

**正确做法：worker + moveToThread**
```cpp
class Worker : public QObject {
    Q_OBJECT
public slots:
    void doHeavyWork(int n) {                  // 在工作线程执行
        qint64 sum = 0;
        for (int i = 0; i < n; ++i) {
            sum += i;
            if (i % 1000000 == 0) emit progress(i * 100 / n);
        }
        emit finished(sum);
    }
signals:
    void progress(int percent);
    void finished(qint64 result);
};

// 使用
auto *thread = new QThread(this);
auto *worker = new Worker;                     // 注意：不指定 parent！
worker->moveToThread(thread);
connect(thread, &QThread::finished, worker, &QObject::deleteLater);
connect(this, &MainWindow::startWork, worker, &Worker::doHeavyWork);  // 跨线程，自动排队
connect(worker, &Worker::progress, progressBar, &QProgressBar::setValue);
connect(worker, &Worker::finished, this, [this](qint64 r){
    statusBar()->showMessage(QString::number(r));
});
thread->start();
```

**必须搞清的 5 条**：
1. **`worker` 不能有 parent**，否则 `moveToThread` 失败（有 parent 的对象和 parent 同线程）。
2. **跨线程 `connect` 自动变成 `Qt::QueuedConnection`**（参数拷贝进事件队列，在目标线程的事件循环里执行）—— 所以你不用加锁就能安全跨线程传数据。
3. **绝不在工作线程里碰任何 UI 对象**。用信号把数据送回主线程。
4. **线程退出要干净**：`thread->quit(); thread->wait();` 再析构，否则退出时可能崩。
5. **类比**：`Qt::QueuedConnection` ≈ 线程安全的消息队列；`QThread` ≈ 一个有自己事件循环的执行体，**不是"一个要 run 的函数"** —— 这是最容易误解的地方。

**替代方案**：`QtConcurrent::run` + `QFutureWatcher`（代码更短，适合一次性重计算，比如你的算法）；`QThreadPool` + `QRunnable`（任务池）。今天至少掌握 `moveToThread`。

### 7.3 网络（一个最小例子即可，别陷进去）
```cpp
auto *mgr = new QNetworkAccessManager(this);
connect(mgr, &QNetworkAccessManager::finished, this, [](QNetworkReply *r){
    if (r->error() == QNetworkReply::NoError)
        qDebug() << r->readAll().left(200);
    r->deleteLater();                        // 必须！
});
mgr->get(QNetworkRequest(QUrl("https://example.com")));
```
**三个认知**：
1. `QNetworkAccessManager` 内部就是异步的 —— 天然不阻塞 UI，和 7.2 的卡顿形成对照。
2. **`QNetworkReply` 必须 `deleteLater()`**，否则持续泄漏。
3. 若报 TLS 错误，检查 `QSslSocket::supportsSsl()`。

### 7.4 动手任务
把 D4 的"训练记录管理器"升级成 **`MainWindow` 架构**：
- 菜单（文件→打开/保存/退出；帮助→关于）、工具栏、状态栏
- 中央 `QTabWidget`：`数据` 页 + `统计` 页
- 加"**分析**"按钮，用 `QtConcurrent` 或 worker 线程后台跑 O(n log n) 统计（按标签聚合 + 去重计数），**期间界面必须能拖动**，并用 `QProgressBar` 显示进度
- 加"网络"菜单项，异步拉一个公开 JSON 接口（如 `https://api.github.com/zen`），结果显示在标签上

### ✅ D5 验收清单
- [ ] 能解释为什么大循环会让窗口"无响应"
- [ ] 能独立写出 `worker + moveToThread` 完整模式（含优雅退出）
- [ ] 知道 `Qt::QueuedConnection` 为什么能免锁传数据，以及它的代价
- [ ] 后台任务运行期间窗口能正常拖动重绘
- [ ] `docs/log/day5.md` + 提交

---

## 8. D6（10/06 周二）综合项目日 1 —— 从零搭骨架

**目标**：从今天起**不再跟着教程写**，自己从需求出发搭出一个能跑通主流程的程序。这是从"学习者"到"开发者"的转折点。

### 8.1 选题（三选一，选你真正会用的）

| 选项 | 说明 | 难度 |
|---|---|---|
| **A. 算法可视化器**（推荐给 ACM） | 选 3–4 个算法（排序、BFS/DFS、Dijkstra、KMP），随机生成数据，逐步动画演示；可调速度、可单步、可暂停 | ★★★ 正好复用 D3 绘制 + D2 定时器 + D4 数据 |
| **B. ACM 训练看板** | D4 项目的完整版：多视图、图表统计、标签体系、导出报告 | ★★ 复用 D4/D5 最多 |
| **C. 本地题解管理器** | 管理本地 Markdown 题解，全文搜索、标签、代码高亮 | ★★★ 需文件系统 + 文本处理 |

> **建议选 A**：把你最强的算法能力直接变成可见的产品，成就感最强；而且"单步执行 + 状态回放"会逼你把**状态机和绘制彻底分离**——这是最有价值的工程训练。

### 8.2 今天的硬性流程（按顺序做，不许跳）

**① 先写需求文档（30 分钟，`docs/spec.md`）**
不要一上来就写代码。写清楚：
- 一句话定义：这程序给谁、解决什么问题
- 功能列表，每项标 `[必须]` / `[可选]` / `[不做]`（**"不做"清单和功能清单一样重要**）
- 界面草图（文字描述分区即可）
- 数据从哪来、存哪去
- **明确的完成定义**：什么状态下算"8 天内做完"

**② 设计数据与接口（30 分钟，`docs/design.md`）**
- 核心数据结构（等价于比赛时"想清楚用什么存状态"）
- 类划分：谁负责数据、谁负责显示、谁负责控制。**先定信号槽接口**（谁发什么信号、谁接），再写实现
- 主线流程先写成伪代码，确认逻辑通顺

**③ 搭骨架并跑起来（2h）**
- 建立目录结构（见 8.3）
- 所有类先写空实现，让程序**先能编译运行**（出现空窗口也算成功）
- 然后按主线流程一段一段填，**每填一小段就运行一次**

**④ 完成第一条"垂直切片"（2h）**
垂直切片 = 从界面到底层**打通一条完整的最窄路径**。比如算法可视化器：点"生成数据" → 界面上画出一排柱子 → 点"单步" → 柱子高亮变化一次。
**不要横向铺功能**（所有算法都搭了框架但一个都不动）。**一条路走通，比十条路各走一半有价值得多。**

### 8.3 建议的目录结构
```
QtLearn/
├── CMakeLists.txt
├── src/
│   ├── main.cpp
│   ├── MainWindow.h/.cpp
│   ├── core/          # 纯逻辑，不依赖 Qt Widgets（可单独测试）
│   ├── model/         # 数据模型
│   └── view/          # 自定义控件、绘制
├── resources/         # .qrc、图标、示例数据
├── docs/
│   ├── spec.md / design.md
│   ├── ENVIRONMENT.md
│   ├── log/day1.md ... day8.md
│   └── snippets.md    # API 速查（D8 汇总）
└── tests/
```
> **重点**：把 `core/`（纯逻辑）和 `view/`（界面）分开，`core/` 里不 `#include <QWidget>`。这样你能用写题解的方式给核心逻辑写测试，也符合"状态与显示分离"。

### ✅ D6 验收清单
- [ ] `docs/spec.md` 有明确的 `[必须]/[可选]/[不做]` 三张清单
- [ ] 程序能编译运行，主界面骨架完整（功能为空也算）
- [ ] **至少一条完整路径能跑通**（点到点，界面有可见反馈）
- [ ] 用了有意义的分支名（如 `feat/scaffold`）

---

## 9. D7（10/07 周三）综合项目日 2 + 工程化发布

**目标**：功能收口，并**打包成能拷给别人直接双击运行的程序**。

### 9.1 上午：功能收口（3h）
- 按 `[必须]` 清单逐项完成并自测
- **交互细节**（决定"像不像产品"）：
  - 窗口尺寸/位置/上次打开路径要记住（`QSettings`）
  - 危险操作（清空、退出有未保存数据）要 `QMessageBox::question` 确认
  - 耗时操作要有进度反馈或等待光标
  - 状态栏始终显示有用信息；按钮不可用时 `setEnabled(false)`
  - 支持快捷键；菜单项加 `&` 助记符
- **错误处理**：文件打不开、格式错误、除零 —— 用 `QMessageBox::warning` 提示，**绝不让程序静默崩溃**。ACM 习惯是"出错就 RE 无所谓"，桌面开发不允许。

### 9.2 下午：调试与工程质量（2h）
1. **调试器实战**：故意制造 3 个 bug（数组越界、空指针、信号重复连接导致槽执行多次），用 `F5` + 断点 + 调用栈逐个定位。
2. **警告清零**：把 `unused variable`、隐式类型转换、`signed/unsigned` 比较全清掉。**这是从"能跑"到"专业"的分界线。**
3. **`qDebug` 技巧**：`qDebug() << __FUNCTION__ << var;` 是最快的排查手段。
4. **可选：给 `core/` 加单元测试**（`QtTest`），用你对拍的思路 —— **随机生成 200 组数据和暴力版本比对**，这是你的独门优势：
   ```cpp
   for (int t = 0; t < 200; ++t) {
       auto data = randomCase();
       QCOMPARE(fastAlgo(data), bruteForce(data));
   }
   ```

### 9.3 傍晚：打包发布（1.5h）

**① 加资源文件** `resources/resources.qrc`：
```xml
<RCC>
  <qresource prefix="/">
    <file>icon.png</file>
  </qresource>
</RCC>
```
在 `CMakeLists.txt` 里：
```cmake
qt_add_resources(QtLearn "app_resources"
    PREFIX "/"
    FILES resources/icon.png
)
```
这样 `QIcon(":/icon.png")` 就能用了（D1 里注释掉的那行现在加回来）。

**② 构建 Release**
```bat
cmake --preset mingw-release
cmake --build --preset mingw-release
```

**③ 用 `windeployqt` 收集依赖** —— "能拷给别人"的关键
```bat
set "PATH=D:\Qt_Develop_Tool\6.11.2\mingw_64\bin;D:\Qt_Develop_Tool\Tools\mingw1310_64\bin;%PATH%"
mkdir dist
copy build\mingw-release\QtLearn.exe dist\
windeployqt --release --no-translations dist\QtLearn.exe
```
它会把所需的 Qt DLL 和平台插件（`platforms\qwindows.dll`）拷到 `dist`。**做完必须验证**：
- 关掉所有相关进程，把整个 `dist` 文件夹拷到桌面（或另一台机器）
- **双击 `dist\QtLearn.exe`**，不依赖任何环境就能运行

> **这一步极其重要**：很多人做完程序却从没发布过，导致"在我机器上能跑"。今天做完你就跨过了这道门槛。
> **注意**：`.gitignore` 里忽略了 `*.dll` 和 `*.exe`，所以 `dist\` 不会被 git 跟踪 —— 这是合理的（发布产物不进版本库），但要在 `README.md` 里写清楚怎么生成。

**④ 写 `README.md`**：一句话介绍 + 截图 + 构建方法 + 已知问题。

### ✅ D7 验收清单
- [ ] `[必须]` 清单全部完成，逐条点过
- [ ] 编译器警告基本为零
- [ ] `dist\QtLearn.exe` 拷到别处双击能运行
- [ ] `README.md` 有截图和构建说明
- [ ] 打了一个 tag（如 `v0.1`）

---

## 10. D8（10/08 周四）复盘与"自主拓展闭环"

**目标**：今天不写新功能。今天交付的是**你自己的一套学习系统**，它决定你国庆后是继续成长还是停在原地。

### 10.1 上午：把知识压缩成你自己的资产（2.5h）

**① `docs/snippets.md` —— 你的私人 API 速查表（最重要产出）**
按**你自己的理解**组织，不要抄文档：

```markdown
## 定时器
QTimer *t = new QTimer(this);
connect(t, &QTimer::timeout, this, []{ /*...*/ });
t->start(1000);        // 毫秒；t->stop();
// 坑：QTimer 必须在有事件循环的线程里创建

## 自定义绘制
class X : public QWidget {
    Q_OBJECT
protected:
    void paintEvent(QPaintEvent *) override;   // 不要在这里 new 或做重计算
};
// 改画面：改状态 + update()
// 坑：原点在左上、y 向下；Qt6 鼠标坐标用 position() 不是 pos()
```
**目标：把 8 天学到的东西压缩到 3–5 页。** 这份文档你以后每天都会查，它会越写越厚、也越用越快。

**② `docs/stuck-playbook.md` —— 卡住时的标准动作**

| 症状 | 第一反应 | 具体动作 |
|---|---|---|
| 编译报 `was not declared` | 缺头文件 | 查类名 → 加 `#include <同名头文件>` |
| 链接报 `undefined reference to vtable` | moc 没过 | 加 `Q_OBJECT` 了吗？→ `clean-rebuild` |
| 编译过但启动崩溃 | 生命周期问题 | 断点定位；查空指针、悬垂指针、`begin/end` 是否配对 |
| 界面卡死/变白 | UI 线程被阻塞 | 找耗时循环 → 移到 worker 线程 |
| 信号槽没反应 | 连接失败 | 检查参数类型；在槽里 `qDebug()` 打点 |
| 控件看不见/错位 | 布局问题 | 是否 `addWidget` 到布局？是否手工 `setGeometry`？ |
| 双击 exe 立刻退出（`-1073741515`） | 缺 DLL | PATH 加 `6.11.2\mingw_64\bin`；发布用 `windeployqt` |
| 行为和你预期不符 | 版本差异 | 确认是 Qt6 还是 Qt5 写法（附录 C） |

**③ 建立"查 API 的三步法"（写成卡片贴显示器上）**
1. **`Ctrl+左键` / `F12` 跳定义**：进 Qt 头文件看真实签名和注释。**头文件本身就是最好的文档。**
2. **悬停看签名 + `Ctrl+Shift+P` → `Qt:` 命令**：Qt 扩展提供的文档入口。
3. **写最小例子**：新建 20 行的测试函数，只验证这一个 API 的行为，跑起来看输出。
   > **这一步是你和"只会抄教程的人"的根本区别。** 永远不要在没跑通最小例子的情况下，把它塞进大项目。
4. **沉淀**：学完立刻写进 `snippets.md`。**没写进笔记的知识，等于没学。**

### 10.2 下午：复盘 + 建可复用的项目模板（2.5h）

**① `docs/retro.md`（KPT 复盘）**
- **Keep**：哪些做法有效？
- **Problem**：哪里浪费了最多时间？最大的知识空洞是什么？
- **Try**：下次怎么改？
- **自评**：对照 8 天验收清单，诚实打分（`✔ 完全会 / △ 会用但说不清原理 / ✘ 还不会`）
- **特别标注所有 △ 项** —— 这些是你下一阶段的靶子

**② 做一个"Qt 项目脚手架"模板仓库（高价值，强烈建议）**
把 D6/D7 的工程抽象成空模板：`CMakeLists.txt` + preset + `.vscode/` 配置 + `MainWindow` + 一个自定义控件 + `resources.qrc` + `QSettings` 读写 + worker 线程骨架 + 你的构建脚本。
```bat
git init qt-template
rem 放入上述文件，提交，推送到 GitHub
```
**以后每次有新想法，`git clone` 这个模板 → 5 分钟就能开始写真正有意思的部分**，而不是又花半天配环境。这是"进入自主拓展闭环"最实在的基建。

**③ 建一个"问题清单"**：把还不会但想学的写成 issue，比如"QML 是什么""怎么用 Git 做多分支协作""怎么给 Qt 程序加自动更新"。

### 10.3 傍晚：规划后续（1.5h）

**① 定一个"下一个项目"**：比这次难 20% 左右（带数据库、带图表、导出 PDF，或开始 QML）。写在 `docs/next.md`。

**② 国庆后 4 周路线图**

| 周 | 主题 | 产出 |
|---|---|---|
| W1 | 巩固 Widgets + 把 D6 项目做完整（补测试、补文档） | 项目 v1.0 发布 |
| W2 | **QML / Qt Quick 入门**：声明式 UI、`QML ↔ C++` 交互（`Q_PROPERTY`、`Q_INVOKABLE`） | 同功能用 QML 重写一个页面 |
| W3 | 数据 + 图表：`QtCharts`、SQLite（`QSqlDatabase`） | 项目接入数据库和图表 |
| W4 | 工程质量：`QtTest`、`ctest`、GitHub Actions 自动构建、多平台打包 | CI 绿了 + 可发布安装包 |

**③ 建立长期节奏（闭环的最后一环）**
- **每周固定 2 次动手**（不要"等有大块时间"），每次 2 小时，做完就 commit
- **每次收工前写 5 行日志**：今天做了什么、卡在哪、明天第一步做什么
- **每学一个新东西，往 `snippets.md` 加 3 行**
- **每季度读一个开源 Qt 项目的源码**（从小控件库开始）

### 10.4 用 ACM 的方式持续练 Qt（你的独特优势）
把算法能力**产品化**，难度可控、成就感强，且直接复用你已有的算法库：
- 算法动画演示器（D6 选项 A 的增强版）
- 大整数/矩阵运算可视化计算器
- 数独/八数码求解器（带搜索过程可视化）
- 随机数据生成器 + 对拍工具（Qt 版图形化对拍控制台）
- 文本相似度 / 正则测试工具
- 图论可视化编辑器（自己画图、跑最短路/最小生成树）

> 每个都是"**算法 + 界面**"的完整闭环，既练 Qt，又能沉淀成简历上的东西。

### ✅ D8 验收清单
- [ ] `docs/snippets.md` 至少 3 页，覆盖定时器/信号槽/绘制/Model/线程/打包
- [ ] `docs/stuck-playbook.md` 有你自己真实踩过的 ≥6 个坑
- [ ] `docs/retro.md` 完成 KPT + 自评，△ 项已标出
- [ ] Qt 项目模板仓库建好并可 clone 使用
- [ ] `docs/next.md` 写好下一个项目和 4 周路线图
- [ ] **能回答**：明天给你一个没学过的 Qt 控件需求，你的前三步是什么？
  （答案：跳头文件看签名 → 悬停/查文档 → 写最小例子验证 → 沉淀进 snippets）

---

## 11. 常见陷阱与应对

### 11.1 进度落后怎么办（按此顺序砍）
1. **先砍加餐**（D3 波形控件、D5 网络、D7 单元测试）
2. **再砍 D6 项目功能**（只保留 `[必须]` 里最核心的 3 项）
3. **绝不砍**：D1 的开发闭环、D2 的信号槽/事件循环理解、D5 的多线程阻塞实验、D7 打包、D8 笔记
4. **最后手段**：D6+D7 合并，做一个**玩具级**完整项目（跑通一条路径即可），保证 D8 复盘和笔记一定完成

> **原则**：8 天的目标是"**建立能力 + 建立系统**"，不是"做完一个软件"。笔记和闭环比功能更重要。

### 11.2 从 ACM 带来的坏习惯（逐条改）
| ACM 习惯 | Qt 里必须改成 |
|---|---|
| 一个大文件写 500 行 | 按职责拆类拆文件；头文件只放声明 |
| 全局数组、全局变量 | 成员变量 + 对象树；跨对象通信用信号槽 |
| `new` 完不管 | 给 parent，或用智能指针；不确定时 `deleteLater` |
| 出错就 `exit(1)` / RE 无所谓 | 用户可见的错误提示；不崩溃是底线 |
| 只关心时间复杂度 | 还要关心**响应性**（UI 线程永不阻塞）和**内存**（长跑不涨） |
| 一次写完再测 | 写 20 行就运行一次 |
| `printf` 调试 | `qDebug()` + 断点 + 调用栈 |
| `long long` + `0x3f3f3f3f` 走天下 | `QString`、`QVariant`、容器；注意编码（Qt6 默认 UTF-8） |

### 11.3 用 Cursor AI 辅助时的专属陷阱

**Cursor 的 AI 补全和对话是巨大助力，但有系统性偏差：它的 Qt 知识大量来自 Qt 5。**

| 看到 AI 给出 | 问题 | 正确写法 |
|---|---|---|
| `connect(a, SIGNAL(x()), b, SLOT(y()))` | Qt5 老语法，拼错运行时才发现 | `connect(a, &A::x, b, &B::y)` |
| `QMouseEvent::pos()` | Qt6 已废弃 | `position()`（返回 `QPointF`） |
| `QRegExp` | Qt6 移除 | `QRegularExpression` |
| `qrand()` / `qsrand()` | Qt6 移除 | `QRandomGenerator::global()` |
| `#include <QtWidgets/QAction>` | 路径变了 | `#include <QAction>`（属 QtGui） |
| `QDesktopWidget` | Qt6 移除 | `QScreen` |
| `setMargin()` | Qt6 移除 | `setContentsMargins()` |
| `QString::SkipEmptyParts` | 枚举位置变了 | `Qt::SkipEmptyParts` |

**正确姿势**：
1. **让 AI 讲思路和结构，不让 AI 直接决定 API 签名** —— 签名自己跳头文件核对。
2. **AI 给的代码，先跑最小例子**再并入项目。
3. **报错时把完整报错贴给 AI**，它很擅长解编译错误。
4. 可以在项目里放 `.cursor/rules`（或根目录 `.cursorrules`），写上约束，比如：
   > 本项目使用 Qt 6.11.2 + C++17 + CMake。只给 Qt6 API，禁止 SIGNAL/SLOT 宏、QRegExp、qrand、pos()。给出代码时附上需要的 #include。
5. **AI 越强，你自己核对 API 的习惯越重要** —— 这是 8 天里最该养成的独立能力。

---

## 12. 交付物总清单（10/08 收工时）

```
D:\MyProject\QtLearn\
├── src/                      # 分层源码（core/model/view）
├── resources/resources.qrc   # 资源系统
├── build.bat / run.bat       # ★ 你自己写的构建脚本
├── .vscode/                  # ★ Cursor 配置（已配好）
├── .config-backup/           # 原配置备份（见 ENVIRONMENT.md 第七节）
├── docs/
│   ├── ENVIRONMENT.md        # 环境手册（已写好）
│   ├── spec.md / design.md   # 需求 + 设计
│   ├── snippets.md           # ★ 私人 API 速查表
│   ├── stuck-playbook.md     # ★ 排查手册
│   ├── retro.md              # ★ KPT 复盘 + 能力自评
│   ├── next.md               # 下一阶段规划
│   └── log/day1..day8.md     # 每日日志
├── tests/                    # 至少 3 个单元测试（含随机对拍）
├── README.md                 # 含截图
└── dist/                     # ★ 可独立分发的程序
```

**四个 ★ 是你的真正收获**：自己写的构建脚本、一份能天天查的速查表、一套遇到问题时的标准动作、一个可 clone 的项目模板。有了这些，你就有了**自主拓展的闭环**。

---

## 附录 A：环境状态（已配置完成）

| 项目 | 状态 |
|---|---|
| Qt 6.11.2 + MinGW 13.1.0 + CMake 3.30.5 + Ninja | ✅ 已装，路径见 [ENVIRONMENT.md](docs/ENVIRONMENT.md) |
| `CMakePresets.json` | ✅ 已修好 `$penv{PATH}` 并补上插件路径 |
| `.vscode/`（Cursor 读取） | ✅ `settings.json` / `tasks.json` / `launch.json` / `c_cpp_properties.json` / `extensions.json` 已配置 |
| Cursor 打开项目即可构建 | ✅ 实测：干净 PATH 下 configure + build 成功 |
| `F5` 调试 | ✅ 实测：用 `launch.json` 环境启动，窗口正常；⚠️ 需装 `ms-vscode.cpptools` |
| **Windows 环境变量** | ✅ **未改动**（与备份逐字节一致） |
| 源码（`main.cpp` 等） | ⬜ 未改动，留给你按 D1 自己修 |

**关键实测结论**：在**不含 Qt 目录的 PATH** 下运行 exe 会以 `-1073741515`（`0xC0000135`，缺 DLL）退出。所以运行/调试时必须提供 `D:\Qt_Develop_Tool\6.11.2\mingw_64\bin` —— 这是本项目所有 PATH 配置存在的原因，不是多余的。

---

## 附录 B：官方文档与资料优先级

| 优先级 | 资料 | 说明 |
|---|---|---|
| ⭐⭐⭐ | **头文件本身**（`Ctrl+左键`/`F12`） | 最准，永远和你用的版本一致 |
| ⭐⭐⭐ | 官方文档 `doc.qt.io/qt-6/` | 注意 URL 里要是 **qt-6**，不是 qt-5 |
| ⭐⭐⭐ | Qt 自带示例 `D:\Qt_Develop_Tool\Examples\Qt-6.11.2` | 本地、可运行、版本正确。**比任何教程都值钱** |
| ⭐⭐ | Qt 官方博客 / `QWidget` 类文档的 "Examples" 小节 | |
| ⭐ | 网上博客 / 视频教程 | **默认按 Qt5 对待**，抄之前核对 |
| ⭐ | AI 补全 | 讲思路可以，API 签名必须自己核对（见 11.3） |

> **建议今天就做的事**：打开 `D:\Qt_Develop_Tool\Examples\Qt-6.11.2\widgets\`，随便挑一个（比如 `calculator` 或 `painter` 相关）在 Cursor 里打开、跑起来。**有一个本地且版本正确的示例库，是你未来自学最可靠的起点。**

---

## 附录 C：Qt 6 版本坑速查

| Qt 5 写法 | Qt 6 正确写法 |
|---|---|
| `QMouseEvent::pos()` | `position()`（`QPointF`）/ `globalPosition()` |
| `#include <QtWidgets/QAction>` | `#include <QAction>`（属 QtGui） |
| `QString::split(..., QString::SkipEmptyParts)` | `Qt::SkipEmptyParts` |
| `QLayout::setMargin()` | `setContentsMargins()` |
| `qrand()` / `qsrand()` | `QRandomGenerator::global()` |
| `QDesktopWidget` | `QScreen` / `QGuiApplication::primaryScreen()` |
| `endl` for `QTextStream` | `Qt::endl` |
| `QRegExp` | `QRegularExpression` |
| `QPainter::HighQualityAntialiasing` | 已移除，用 `Antialiasing` + `SmoothPixmapTransform` |
| `QTextStream` 默认编码非 UTF-8 | Qt6 文本类默认 UTF-8 |

> **自检**：教程里出现 `SIGNAL()`/`SLOT()` 宏、`qrand()`、`QRegExp`，基本就是 Qt 5 时代。**看方法，不要抄代码。**

---

## 附录 D：一页速查卡（建议打印贴墙）

**每天开工**：读昨天日志 → 列出 3 个待解决问题
**每天收工**：写日志 → `git commit -m "D<N>: ..."` → 往 snippets.md 加 3 行

**卡住时的顺序**：
1. 读完整报错（不只是第一行）
2. 缺头文件？→ `#include`
3. 链接错？→ `Q_OBJECT` + `clean-rebuild`
4. 逻辑错？→ 断点 + 调用栈 + `qDebug()`
5. 界面不响应？→ 找阻塞 UI 线程的循环
6. 还不行？→ **写 20 行最小复现**（这步能解决 80% 的疑难）
7. 解决后**立刻写进 `stuck-playbook.md`**

**学新 API 的四步**：
`F12 跳头文件看签名` → `悬停/查 qt-6 文档` → `写最小例子跑通` → `写进 snippets.md`

**三条铁律**：
1. 不用绝对坐标，用布局
2. UI 线程永不阻塞
3. 改数据 + 发信号，不直接操作视图

**Cursor 三键**：`F12` 跳定义 · `Alt+F12` 预览定义 · `F5` 调试

---

*祝国庆顺利。你是 ACM 选手——别人 8 天学不会的东西，你能；但请把这 8 天用在"机制"和"系统"上，而不是"语法"上。*
