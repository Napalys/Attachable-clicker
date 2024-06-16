//
// Created by Widok on 6/1/24.
//

#ifndef INJECTIONCLICKER_ANOMALY_H
#define INJECTIONCLICKER_ANOMALY_H

#include <string>
#include "nlohmann/json.hpp"

using Percentage = int;

struct Anomaly {
    std::string template_image;
    std::string message;
    Percentage coefficient;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Anomaly, template_image, message, coefficient)


#endif //INJECTIONCLICKER_ANOMALY_H
