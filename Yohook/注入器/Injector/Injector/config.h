#pragma once

// YoInjector 配置文件
// 只需要修改这个文件中的配置即可
namespace Config
{
    // 目标EXE程序名称（与注入器放在同一目录）
    inline constexpr const char *TargetEXE = "*.exe";

    // 要注入的DLL列表（按顺序注入）
    inline constexpr const char *TargetDLLNames[] = {
        "yohook.dll",
        // "yohook2.dll",  // 可以添加更多DLL
    };
}
