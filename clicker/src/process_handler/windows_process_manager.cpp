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
            GetWindowTextW(hCurWnd, &title[0], static_cast<int>(title.size()));
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
        GetWindowTextW(hwnd, &title[0], static_cast<int>(title.size()));
        std::string str(title.begin(), title.end());
        //remove string terminator, thus after converting to char we do not lose data
        str.erase(std::find(str.begin(), str.end(), '\0'), str.end());
        if (title.size() > 1)
            errorMessage += '\n' + str;
    }
    throw std::invalid_argument(errorMessage);
}

void sendToHWNDClick(HWND hwnd, uint32_t event, uint8_t key_code){
  SendMessage(hwnd, event, key_code, 0);
}

namespace ProcessHandler {

    void WindowsProcessManager::startKey(uint8_t key_code) {
      auto it = keys.find(key_code);
      if(it != keys.end()){
        it->second.setPressedState(true);
        return;
      }
      auto result = keys.emplace(std::piecewise_construct,
                                 std::forward_as_tuple(key_code),
                                 std::forward_as_tuple(hwnd, key_code));

      if (result.second) {
        result.first->second.startWorker();
      }
    }

    void WindowsProcessManager::stopKey(uint8_t key_code) {
        if (auto it = keys.find(key_code); it != keys.end()) {
            it->second.setPressedState(false);
        }
    }
    void WindowsProcessManager::stopAll() {
        keys.clear();
    }

    WindowsProcessManager::~WindowsProcessManager() noexcept {
        stopAll();
    }

    ProcessHandler::WindowsProcessManager::WindowsProcessManager(uint32_t dwProcessID, const std::string& processName) {
        hwnd = receiveHWND(dwProcessID, processName);
    }

    void WindowsProcessManager::sendClick(ClickerData clicker_data) {
      if(clicker_data.event == ClickerData::Event::Pressed){
          startKey(clicker_data.key_code);
      }else {
        stopKey(clicker_data.key_code);

      }
    }


    HoldKey::HoldKey(HWND hwnd, uint8_t key_code) : hwnd(hwnd), key_code(key_code) {}

    void HoldKey::startWorker() {
      const auto lam = [this] {
          std::unique_lock<std::mutex> lock(this->mtx, std::defer_lock);
          while (this->running) {
            this->cv.wait(lock, [this]{ return pressed || !this->running; });
            while (pressed && this->running) {
              sendToHWNDClick(this->hwnd, WM_KEYDOWN, key_code);  // Placeholder for send key functionality
              std::this_thread::sleep_for(std::chrono::milliseconds(20));
            }
            sendToHWNDClick(this->hwnd, WM_KEYUP, key_code);
          }
      };
      thread = std::thread(lam);
    }

    void HoldKey::setPressedState(bool state) noexcept {
      pressed = state;
      cv.notify_one();
    }

    HoldKey::~HoldKey() noexcept {
      running = false;
      cv.notify_one();
      if(thread.joinable()) thread.join();
    }

    void WindowsProcessManager::takeScreenshot() {

    }
} // ProcessHandler
#endif