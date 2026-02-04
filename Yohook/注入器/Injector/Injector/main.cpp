#define NOMINMAX
#include "config.h"
#include <string>
#include <memory>
#include <windows.h>
#include <filesystem>
namespace fs = std::filesystem;

// DLL注入器
// 使用 QueueUserAPC + LoadLibraryW 方法实现DLL注入
namespace
{
    // RAII 句柄包装器 - 自动管理 Windows 句柄生命周期
    struct HandleDeleter
    {
        void operator()(HANDLE h) const noexcept
        {
            if (h && h != INVALID_HANDLE_VALUE)
            {
                CloseHandle(h);
            }
        }
    };
    using UniqueHandle = std::unique_ptr<std::remove_pointer_t<HANDLE>, HandleDeleter>;

    // RAII 远程内存包装器 - 自动管理目标进程中的内存
    class RemoteMemory
    {
    public:
        RemoteMemory(HANDLE process, SIZE_T size) noexcept
            : m_process(process), m_address(VirtualAllocEx(process, nullptr, size,
                                                           MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE))
        {
        }

        ~RemoteMemory() noexcept { release(); }

        // 禁用拷贝
        RemoteMemory(const RemoteMemory &) = delete;
        RemoteMemory &operator=(const RemoteMemory &) = delete;

        // 禁用移动（本项目不需要）
        RemoteMemory(RemoteMemory &&) = delete;
        RemoteMemory &operator=(RemoteMemory &&) = delete;

        [[nodiscard]] explicit operator bool() const noexcept { return m_address != nullptr; }
        [[nodiscard]] LPVOID get() const noexcept { return m_address; }

        // 放弃所有权（用于 APC 异步执行场景）
        LPVOID detach() noexcept
        {
            LPVOID addr = m_address;
            m_address = nullptr;
            return addr;
        }

    private:
        void release() noexcept
        {
            if (m_address)
            {
                VirtualFreeEx(m_process, m_address, 0, MEM_RELEASE);
                m_address = nullptr;
            }
        }

        HANDLE m_process;
        LPVOID m_address;
    };

    // 工具函数
    // 获取当前模块所在目录（使用 GetModuleFileNameW 确保始终返回 exe 所在路径）
    [[nodiscard]] fs::path GetCurrentDir()
    {
        wchar_t path[MAX_PATH] = {};
        if (GetModuleFileNameW(nullptr, path, MAX_PATH) == 0)
        {
            return fs::current_path();
        }
        return fs::path(path).parent_path();
    }

    // 使用 QueueUserAPC 注入单个DLL到目标进程（使用 LoadLibraryW 支持任意路径）
    [[nodiscard]] bool InjectDLL(HANDLE hProcess, HANDLE hThread, const fs::path &dllPath)
    {
        const std::wstring &pathStr = dllPath.native(); // 直接引用，无拷贝
        const SIZE_T pathBytes = (pathStr.size() + 1) * sizeof(wchar_t);

        // RAII 自动管理远程内存
        RemoteMemory remoteBuf(hProcess, pathBytes);
        if (!remoteBuf)
        {
            return false;
        }

        // 写入 DLL 路径（宽字符）
        if (!WriteProcessMemory(hProcess, remoteBuf.get(), pathStr.c_str(), pathBytes, nullptr))
        {
            return false; // remoteBuf 析构时自动释放
        }

        // 获取 LoadLibraryW 地址（支持 Unicode 路径）
        const auto pLoadLibrary = reinterpret_cast<PAPCFUNC>(
            GetProcAddress(GetModuleHandleW(L"kernel32.dll"), "LoadLibraryW"));
        if (!pLoadLibrary)
        {
            return false; // remoteBuf 析构时自动释放
        }

        // 加入 APC 队列
        if (QueueUserAPC(pLoadLibrary, hThread, reinterpret_cast<ULONG_PTR>(remoteBuf.get())) == 0)
        {
            return false; // remoteBuf 析构时自动释放
        }

        // APC 异步执行，放弃内存所有权，由目标进程管理
        remoteBuf.detach();
        return true;
    }

    // 显示错误消息
    void ShowError(const wchar_t *format, ...)
    {
        wchar_t msg[512];
        va_list args;
        va_start(args, format);
        wvsprintfW(msg, format, args);
        va_end(args);
        MessageBoxW(nullptr, msg, L"错误", MB_ICONERROR);
    }

    void ShowWarning(const wchar_t *format, ...)
    {
        wchar_t msg[512];
        va_list args;
        va_start(args, format);
        wvsprintfW(msg, format, args);
        va_end(args);
        MessageBoxW(nullptr, msg, L"警告", MB_ICONWARNING);
    }

} // anonymous namespace

int WINAPI WinMain(_In_ HINSTANCE /*hInstance*/,
                   _In_opt_ HINSTANCE /*hPrevInstance*/,
                   _In_ LPSTR /*lpCmdLine*/,
                   _In_ int /*nCmdShow*/)
{
    const fs::path currentDir = GetCurrentDir();
    const fs::path exePath = currentDir / Config::TargetEXE;

    // 使用聚合初始化，自动设置 cb
    STARTUPINFOW si = {sizeof(si)};
    PROCESS_INFORMATION pi = {};

    // 以挂起状态启动目标进程
    if (!CreateProcessW(exePath.c_str(), nullptr, nullptr, nullptr, FALSE,
                        CREATE_SUSPENDED, nullptr, currentDir.c_str(), &si, &pi))
    {
        ShowError(L"启动目标程序失败!\n\n路径: %s\n错误码: %lu",
                  exePath.c_str(), GetLastError());
        return 1;
    }

    // RAII 自动管理进程句柄
    UniqueHandle hThread(pi.hThread);
    UniqueHandle hProcess(pi.hProcess);

    // 使用范围 for 遍历 DLL 列表
    bool allSuccess = true;
    for (const char *dllName : Config::TargetDLLNames)
    {
        const fs::path dllPath = currentDir / dllName;
        if (!InjectDLL(hProcess.get(), hThread.get(), dllPath))
        {
            ShowWarning(L"注入DLL失败: %S", dllName); // %S 用于在宽字符格式中打印窄字符串
            allSuccess = false;
        }
    }

    // 恢复主线程，APC 队列中的 LoadLibrary 会被依次执行
    ResumeThread(hThread.get());

    // 句柄在作用域结束时自动关闭
    return allSuccess ? 0 : 1;
}
