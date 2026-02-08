# 使用指南
## 📋 核心功能概览
- ✅ 字体替换
- ✅ 字符替换
- ✅ 虚拟文件系统（VFS）
- ✅ 虚拟注册表
- ✅ 区域模拟（LocaleEmulator）
- ✅ 窗口标题自定义

---

## ⚙️ 配置文件基础要求

- **编码格式**：必须保存为 **UTF-8**
- **开关值**：`true` 开启功能，`false` 关闭功能

---

## 📝 字体管理 (Font)

解决游戏内中文字体显示异常问题。

### 基础配置
```ini
[Font]
EnableCharset = true
Charset = 0x80

EnableFontManager = true
FontFileName = Font.ttf
FontName = Font
```

### 参数说明
| 参数名              | 说明                 | 可选值                              |
| ------------------- | -------------------- | ----------------------------------- |
| `EnableCharset`     | 是否启用字符集控制   | true / false                        |
| `Charset`           | 字符集选择           | `0x80` (日语字符集)<br>`0x86` (GBK) |
| `EnableFontManager` | 是否启用字体管理器   | true / false                        |
| `FontFileName`      | 指定加载的字体文件名 | 如：`Font.ttf`                      |
| `FontName`          | 强制使用的字体名称   | 如：`Font`                          |

### API 控制
```ini
EnableCreateFontA = false
EnableCreateFontW = false
EnableCreateFontIndirectA = false
EnableCreateFontIndirectW = false
EnableGDIPlus = false
```
> **📌 提示**
> 可以把游戏程序拖到bat自动获取api

> **⚠️ 注意事项**
> - 必须配置对应的API，字体修改才会生效
> - 请根据游戏实际使用的 API 开启对应选项
> - 不建议全部开启，可能导致兼容性问题

---

## 🔄 字符替换 (ReCharacter)

功能与 UniversalInjector 的 `character_substitution` 相同，实现特定字符替换，SExtractor生成的uif_config.json修改成示例模样即可使用

### 基础配置
```ini
[ReCharacter]
Enable = true
CharsetPage = 0x80
EnableExtTextOutA = false
EnableExtTextOutW = false
```

### 配置文件
在游戏目录创建 **`UIF.json`**，内容如下：

```json
{
    "character_substitution": {
        "source_characters": "開門",
        "target_characters": "开门"
    }
}
```

> **📌 使用提示**
> 1. `UIF.json` 必须保存为 **UTF-8** 格式
> 2. 若替换无效，可尝试开启 `EnableExtTextOutA` 或 `EnableExtTextOutW`
> 3. CharsetPage：<br>`0x80` (日语字符集)<br>`0x86` (GBK)<br>`0` System ACP
> 4. 如仍无效，建议使用字体替换方案

---

## 📁 虚拟文件系统 (VFS)

将游戏文件读取操作重定向到外部文件夹或资源包，优先读取外部文件，方便替换游戏资源

### 基础配置
```ini
[VFS]
Enable = true
VFSMode = 1
Folder = 0
ArchiveFile = 0
SavePath = 0
```
> **提示**：<br>
> 将任意参数设为 `0` 可单独关闭对应功能。<br>
> Folder = script | 从script文件夹中读取<br>
> ArchiveFile = script.chs | 从script.chs压缩包中读取

### 文件读取优先级
1. **外部文件夹** (`Folder`)
2. **资源包文件** (`ArchiveFile`)
3. **游戏原始文件**

### VFS 模式说明 (VFSMod)
| 模式值 | 模式名称 | 特点                                 |
| ------ | -------- | ------------------------------------ |
| 1      | 内存模式 | 完全在内存里运行                     |
| 2      | 硬盘模式 | 如果内存模式遇到问题，可以尝试此模式 |
> **注意**：`VFSMod` 仅用于ArchiveFile(资源包模式)

### 存档路径重定向
格式：`原路径:目标文件夹`

**示例**
```ini
SavePath = %APPDATA%\GameCompany\GameName:savedata
```
#### 支持的环境变量
| 变量名           | 对应路径                          |
| ---------------- | --------------------------------- |
| `%SAVEDGAMES%`   | `C:\Users\用户名\Saved Games`     |
| `%DOCUMENTS%`    | `C:\Users\用户名\Documents`       |
| `%APPDATA%`      | `C:\Users\用户名\AppData\Roaming` |
| `%LOCALAPPDATA%` | `C:\Users\用户名\AppData\Local`   |
| `%USERPROFILE%`  | `C:\Users\用户名`                 |

#### 使用Packer打包资源
**使用方法：**
1. 将需要替换的游戏资源放入一个文件夹，保持原有的目录结构
2. 将该文件夹拖到Packer.exe上
---

## 🗂️ 虚拟注册表 (Registry)

加载外部注册表文件到虚拟层，实现免安装运行。

### 基础配置
```ini
[Registry]
Enable = true
File = game.reg
```
### 注册表文件要求
- **编码**：必须保存为 **UTF-16LE**
- **格式**：标准 Windows 注册表导出格式

**示例文件 (`game.reg`) 内容**
```reg
Windows Registry Editor Version 5.00

[HKEY_CURRENT_USER\Software\Will]
[HKEY_CURRENT_USER\Software\Will\Unskort]
"InstallDir"="."
"InstallSrc"="."
```
---
## 🌏 区域模拟 (LocaleEmulator)

使用 LocaleEmulator 实现区域模拟

### 前置要求
需要以下 DLL 文件位于程序目录：
- `LoaderDll.dll`
- `LocaleEmulator.dll`

### 配置示例
```ini
[LocaleEmulator]
Enable = true
LocaleCodepage = 932
LocaleId = 1041
Locharset = 128
Timezone = Tokyo Standard Time
```

---

## 🪟 窗口标题自定义 (WindowTitle)
自定义游戏窗口标题，支持 Unicode

```ini
[WindowTitle]
Enable = true
Title = 自定义标题内容
```

---

## 🐛 调试功能 (Debug)

用于问题排查和日志记录。

```ini
[Debug]
EnableDebugConsole = true   ; 显示调试控制台窗口
EnableLogToFile = true      ; 将日志输出到文件
EnableCrashHandler = true   ; 启用崩溃信息捕获
```
---

## 注入yohook
### 方式1：setdll

### 方式2：远程注入
1. 把config.h里的exe改成里需要的内容
2. 编译 Injector
- Visual Studio 2022 (v143 工具集)
- Windows SDK

```cpp
inline constexpr const char *TargetEXE = "*.exe";
```
### 方式3：winmm.dll
