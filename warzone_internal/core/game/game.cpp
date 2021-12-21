#include "game.h"
#include <string>

namespace game {
    HWND window_handle{};

    BOOL CALLBACK callback(HWND hwnd, LPARAM param) {
        DWORD pid;
        GetWindowThreadProcessId(hwnd, &pid);
        if (pid == param) {
            TCHAR classNameBuf[MAX_PATH];
            GetClassName(hwnd, classNameBuf, MAX_PATH);
            std::string className(&classNameBuf[0]);
            if (className != ("MSCTFIME UI") && className != ("IME") && className != ("ConsoleWindowClass")) {
                window_handle = hwnd;
                return false;
            }
        }
        return true;
    }

    HWND get_window() {
        EnumWindows(callback, GetCurrentProcessId());
        return window_handle;
    }
}