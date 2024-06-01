//
// Created by Widok on 6/1/24.
//

#include <iostream>
#include <opencv2/opencv.hpp>
#include "image_processing.h"

namespace ImageProcessing {

    std::pair<double, cv::Point> perform(cv::Mat& img, cv::Mat& templ){
        cv::Mat result;
        cv::matchTemplate(img, templ, result, cv::TM_CCOEFF_NORMED);
        double maxVal; cv::Point maxLoc;
        cv::minMaxLoc(result, nullptr, &maxVal, nullptr, &maxLoc);
        return {maxVal, maxLoc};
    }

    std::string isImageWithinImage(const std::string &source, const std::string &template_image, double threshold) {
        cv::Mat img = cv::imread(source);
        cv::Mat templ = cv::imread(template_image);

        if (img.empty() || templ.empty()) {
            std::cout << "Could not open or find the image(s)!\n";
            throw std::invalid_argument("Could open image");
        }

        cv::Mat img_hsv, templ_hsv;
        cv::cvtColor(img, img_hsv, cv::COLOR_BGR2GRAY);
        cv::cvtColor(templ, templ_hsv, cv::COLOR_BGR2GRAY);


        int kernelSize = 9;
        cv::GaussianBlur(img_hsv, img_hsv, cv::Size(kernelSize, kernelSize), 0);
        cv::GaussianBlur(templ_hsv, templ_hsv, cv::Size(kernelSize, kernelSize), 0);

        auto [minVal, minLoc] = perform(img_hsv, templ_hsv);

        std::cout << "Minimum value (error score): " << minVal << std::endl;
        if (minVal >= threshold) {
            cv::rectangle(img, minLoc, cv::Point(minLoc.x + templ.cols, minLoc.y + templ.rows), cv::Scalar(0, 255, 0), 2, 8, 0);
            cv::imwrite("detection.bmp", img);
            std::cout << "Match found with adjusted confidence: " << (minVal) * 100 << "%" << std::endl;
            return "detection.bmp";
        }
        return "";
    }

} // ImageProcessing