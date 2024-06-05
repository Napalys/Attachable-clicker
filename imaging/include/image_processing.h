//
// Created by Widok on 6/1/24.
//

#ifndef INJECTIONCLICKER_IMAGEPROCESSING_H
#define INJECTIONCLICKER_IMAGEPROCESSING_H

#include <string>

namespace ImageProcessing {

    std::string isImageWithinImage(const std::string &source, const std::string &template_image, double threshold = 0.8);

} // ImageProcessing

#endif //INJECTIONCLICKER_IMAGEPROCESSING_H
