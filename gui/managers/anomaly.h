//
// Created by Widok on 6/1/24.
//

#ifndef INJECTIONCLICKER_ANOMALY_H
#define INJECTIONCLICKER_ANOMALY_H

#include <string>

using Percentage = int;

struct Anomaly {
    std::string template_image;
    std::string message;
    Percentage coefficient;
};


#endif //INJECTIONCLICKER_ANOMALY_H
