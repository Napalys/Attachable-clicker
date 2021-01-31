#include <mutex>
#include <iostream>
#include "clicker.h"

static std::mutex m;

Clicker::Clicker() = default;

void Clicker::setHWND(HWND hwnd) {
    this->injectionWindow = hwnd;
}

void Clicker::setClickerStatus(bool status) {
    this->clickerStatus = status;
}

HWND Clicker::getHWND() {
    return injectionWindow;
}

bool Clicker::getClickerStatus() const {
    return clickerStatus;
}

void startClicking(const clickerData &data, Clicker *clicker) {

    while (clicker->getClickerStatus()) {
        m.lock();
        if(data.longClick) {
            int counter = 0;
            while (counter < 1000){
                SendMessage(clicker->getHWND(), WM_KEYDOWN, data.key_code, 0);
                Sleep(50);
                counter += 50;
            }
            SendMessage(clicker->getHWND(), WM_KEYUP, data.key_code, 0);
        }
        else {
            SendMessage(clicker->getHWND(), WM_KEYDOWN, data.key_code, 0);
            SendMessage(clicker->getHWND(), WM_KEYUP, data.key_code, 0);
        }
        m.unlock();
        Sleep(data.longClick? data.delay * 1000 : data.delay);
    }
}

void Clicker::initClickerThreads(std::vector<clickerData> &data) {
    for (auto &d : data) {
        jobs.emplace_back(startClicking, d, this);
    }
}

void Clicker::destroyClickerThreads() {
    for (auto &thread : jobs) {
        thread.join();
    }
    jobs.clear();
}
