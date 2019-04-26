//
// Created by Thomas Lienbacher on 25.04.2019.
//

#include "bot.hpp"
#include "device.hpp"
#include "util.hpp"

#include <iostream>


Bot::Bot() {

}

Bot::~Bot() {

}

bool Bot::process() {
    if (!openTrainTroops()) {
        std::cerr << "couldn't open train troops!" << std::endl;
    }

    if (!switchFastTab()) {
        std::cerr << "couldn't switch to fast tab!" << std::endl;
    }

    if (!trainTroops()) {
        std::cerr << "couldn't train any troops!" << std::endl;
    }

    return true;
}


void Bot::requestBaseCenter() {
    std::cout << "Please center your base and zoom out:" << std::endl;
    std::string input;

    while (true) {
        std::cout << "  OK? [y/Y] > ";
        std::cin >> input;

        if (input == "y" || input == "Y") {
            break;
        }
    }
}

std::vector<cv::Rect> Bot::findImageMatches(cv::Mat &ref, cv::Mat &tpl) {
    std::vector<cv::Rect> rects;

    cv::Mat res_32f(ref.rows - tpl.rows + 1, ref.cols - tpl.cols + 1, CV_32FC1);
    cv::matchTemplate(ref, tpl, res_32f, cv::TM_CCOEFF_NORMED);

    cv::Mat res;
    res_32f.convertTo(res, CV_8U, 255.0);

    int size = ((tpl.cols + tpl.rows) / 4) * 2 + 1; //force size to be odd
    cv::threshold(res, res, 240.0, 255.0, cv::THRESH_TOZERO);
    cv::adaptiveThreshold(res, res, 240, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY, size, -128);

    while (true) {
        double minval, maxval, threshold = 0.85;
        cv::Point minloc, maxloc;
        cv::minMaxLoc(res, &minval, &maxval, &minloc, &maxloc);

        if (maxval >= threshold * 255) {
            rects.emplace_back(cv::Rect(maxloc, cv::Point(maxloc.x + tpl.cols, maxloc.y + tpl.rows)));
            cv::floodFill(res, maxloc, 0);
        } else
            break;
    }

    return rects;
}

bool Bot::openTrainTroops() {
    captureScreen();
    cv::Mat trainTroops = cv::imread("train_troops.png");
    auto matches = findImageMatches(screen, trainTroops);

    if (matches.empty()) {
        return false;
    }

    TapDevice(matches[0].x + matches[0].width / 2, matches[0].y + matches[0].height / 2);

    return true;
}

bool Bot::switchFastTab() {
    captureScreen();
    cv::Mat fastTrainTab = cv::imread("fast_train_tab.png");
    auto matches = findImageMatches(screen, fastTrainTab);

    if (matches.empty()) {
        return false;
    }

    TapDevice(matches[0].x + matches[0].width / 2, matches[0].y + matches[0].height / 2);

    return true;
}

void Bot::captureScreen() {
    if (DeviceUnlockedAndAwake()) {
        CreateScreenshot();
    } else {
        Error("please unlock device!");
    }

    screen = cv::imread("screen.png");
    cv::resize(screen, screen, {1920, 1080});
}

bool Bot::trainTroops() {
    captureScreen();
    cv::Mat trainGreen = cv::imread("train_green.png");
    auto matches = findImageMatches(screen, trainGreen);

    if (matches.empty()) {
        return false;
    }

    std::sort(matches.begin(), matches.end(), [](cv::Rect &a, cv::Rect &b) -> bool {
        return a.y < b.y;
    });

    TapDevice(matches[0].x + matches[0].width / 2, matches[0].y + matches[0].height / 2);

    return true;
}

bool Bot::collectResources() {
    captureScreen();

    //todo

    return false;
}
