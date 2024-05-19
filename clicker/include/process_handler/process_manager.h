//
// Created by Widok on 5/19/24.
//

#ifndef INJECTIONCLICKER_PROCCESS_MANAGER_H
#define INJECTIONCLICKER_PROCCESS_MANAGER_H

#include "clicker_data.h"

namespace ProcessHandler {

    class ProcessManager {
    public:
        virtual ~ProcessManager() = default;
        virtual void sendClick(ClickerData clicker_data) = 0;
    };
}
#endif //INJECTIONCLICKER_PROCCESS_MANAGER_H
