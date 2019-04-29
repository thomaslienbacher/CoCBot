//
// Created by Thomas Lienbacher on 25.04.2019.
//

#ifndef COCBOT_BOT_HPP
#define COCBOT_BOT_HPP

#include <opencv2/opencv.hpp>

class Bot {
    cv::Mat screen;

    std::vector<cv::Rect> findImageMatches(cv::Mat &ref, cv::Mat &tpl, double threshold = 240);

    bool openTrainTroops();

    bool switchFastTab();

    bool trainTroops();

    bool closeTrainTab();

    bool collectResources();

    bool checkIfTrained();

    bool findEnemy();

    bool attack();

    void captureScreen();

public:
    Bot() = default;

    ~Bot() = default;

    Bot(const Bot &other) = delete;

    Bot(Bot &&other) noexcept = delete;

    Bot &operator=(const Bot &other) = delete;

    Bot &operator=(Bot &&other) noexcept = delete;

    bool process();

    void requestBaseCenter();
};


#endif //COCBOT_BOT_HPP
