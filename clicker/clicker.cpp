#include "clicker.h"

Clicker::Clicker() = default;

void Clicker::setHWND(HWND hwnd){
    this->injectionWindow = hwnd;
}

void Clicker::setClickerStatus(bool status){
    this->clickerStatus = status;
}

HWND Clicker::getHWND(){
    return injectionWindow;
}

bool Clicker::getClickerStatus(){
    return clickerStatus;
}

void startClicking(const clickerData& data, Clicker* clicker){

    while(clicker->getClickerStatus()){
        SendMessage(clicker->getHWND(), WM_KEYDOWN,  data.key_code, 0);
        Sleep(data.delay);
    }
}

void Clicker::initClickerThreads(std::vector<clickerData>& data){
    for(auto& d : data){
        jobs.emplace_back(startClicking, d, this);
    }
}

void Clicker::destroyClickerThreads(){
    for(auto& thread : jobs){
        thread.join();
    }
    jobs.clear();
}
