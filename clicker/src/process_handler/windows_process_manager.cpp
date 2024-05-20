#ifdef _WIN32
#include <stdexcept>
#include <algorithm>
#include <vector>
#include "process_handler/windows_process_manager.h"

HWND receiveHWND(const uint32_t &dwProcessID, const std::string &processName) {
    std::vector<HWND> vhWnds;
    HWND targetWindow = nullptr;
    /// Find all hWnds (vhWnds) associated with a process id (dwProcessID)
    HWND hCurWnd = nullptr;
    do {
        hCurWnd = FindWindowEx(nullptr, hCurWnd, nullptr, nullptr);
        DWORD checkProcessID = 0;
        GetWindowThreadProcessId(hCurWnd, &checkProcessID);
        if (checkProcessID == dwProcessID) {
            vhWnds.emplace_back(hCurWnd);  // add the found hCurWnd to the vector

            std::wstring title(GetWindowTextLength(hCurWnd) + 1, L'\0');
            GetWindowTextW(hCurWnd, &title[0], title.size());
            std::string str(title.begin(), title.end());
            if (str.find(processName) != std::string::npos)
                targetWindow = hCurWnd;

        }
    } while (hCurWnd != nullptr);

    //Throw error is no processes found with such PID
    if (vhWnds.empty()) {
        throw std::invalid_argument("There are no processes PID!");
        return nullptr;
    }
    std::string errorMessage;
    if (targetWindow) return targetWindow;

    for (auto &hwnd: vhWnds) {
        std::wstring title(GetWindowTextLength(hwnd) + 1, L'\0');
        GetWindowTextW(hwnd, &title[0], title.size());
        std::string str(title.begin(), title.end());
        //remove string terminator, thus after converting to char we do not lose data
        str.erase(std::find(str.begin(), str.end(), '\0'), str.end());
        if (title.size() > 1)
            errorMessage += '\n' + str;
    }
    throw std::invalid_argument(errorMessage);
}

namespace ProcessHandler {
    ProcessHandler::WindowsProcessManager::WindowsProcessManager(uint32_t dwProcessID, const std::string& processName) {
        hwnd = receiveHWND(dwProcessID, processName);
    }

    void WindowsProcessManager::sendClick(ClickerData clicker_data) {
        const auto action = (clicker_data.event == ClickerData::Event::Pressed) ? WM_KEYDOWN : WM_KEYUP;
        SendMessage(hwnd, action, clicker_data.key_code, 0);
    }


} // ProcessHandler
#endif