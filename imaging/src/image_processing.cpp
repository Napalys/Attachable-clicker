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

        if (minVal >= threshold) {
            cv::rectangle(img, minLoc, cv::Point(minLoc.x + templ.cols, minLoc.y + templ.rows), cv::Scalar(0, 255, 0), 2, 8, 0);
            std::string confidence_text = "Confidence: " + std::to_string(minVal * 100) + "%";

            int font = cv::FONT_HERSHEY_SIMPLEX;
            double font_scale = 0.5;
            int thickness = 1;
            cv::Point text_origin(minLoc.x, minLoc.y - 10);
            cv::putText(img, confidence_text, text_origin, font, font_scale, cv::Scalar(0, 255, 0), thickness, 8, false);
            const static std::string file_name = "detection.png";
            cv::imwrite(file_name, img);
            std::cout << "Match found with adjusted confidence: " << (minVal) * 100 << "%" << std::endl;
            return file_name;
        }
        return "";
    }

} // ImageProcessing