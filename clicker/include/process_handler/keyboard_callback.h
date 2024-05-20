//
// Created by Widok on 5/20/24.
//

#ifndef INJECTIONCLICKER_KEYBOARD_CALLBACK_H
#define INJECTIONCLICKER_KEYBOARD_CALLBACK_H

#include <functional>
#include "clicker_data.h"

namespace ProcessHandler {
    using KeyboardCallback = std::function<void(ClickerData)>;

    void registerCallBack(const KeyboardCallback& cb);

    void removeCallBack();
}
#endif //INJECTIONCLICKER_KEYBOARD_CALLBACK_H
