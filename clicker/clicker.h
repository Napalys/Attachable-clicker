#ifndef CLICKER_H
#define CLICKER_H

#include <windows.h>
#include "clickerData.h"
#include <vector>
#include <thread>

class Clicker
{
private:
    HWND injectionWindow = nullptr;
    bool clickerStatus = false;
    std::vector<std::thread> jobs{};
public:
    Clicker();

    void setHWND(HWND hwnd);

    void setClickerStatus(bool status);

    HWND getHWND();

    bool getClickerStatus();

    //void startClicking(const clickerData& data);

    void initClickerThreads(std::vector<clickerData>& data);

    void destroyClickerThreads();
};

#endif // CLICKER_H
