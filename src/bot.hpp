//
// Created by Thomas Lienbacher on 25.04.2019.
//

#ifndef COCBOT_BOT_HPP
#define COCBOT_BOT_HPP

#include <opencv2/opencv.hpp>

class Bot {
    cv::Mat screen;

    std::vector<cv::Rect> findImageMatches(cv::Mat &ref, cv::Mat &tpl);

    bool openTrainTroops();

    bool switchFastTab();

    bool trainTroops();

    bool collectResources();

    void captureScreen();

public:
    Bot();

    ~Bot();

    Bot(const Bot &other) = delete;

    Bot(Bot &&other) noexcept = delete;

    Bot &operator=(const Bot &other) = delete;

    Bot &operator=(Bot &&other) noexcept = delete;

    bool process();

    void requestBaseCenter();
};


#endif //COCBOT_BOT_HPP
