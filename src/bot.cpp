//
// Created by Thomas Lienbacher on 25.04.2019.
//

#include "bot.hpp"
#include "device.hpp"
#include "util.hpp"

#include <iostream>
#include <chrono>
#include <thread>


bool Bot::process() {
    captureScreen();

    if (openTrainTroops()) {
        captureScreen();

        if (switchFastTab()) {
            captureScreen();

            if (!trainTroops()) {
                std::cerr << "couldn't train any troops!" << std::endl;
            }

            if (!closeTrainTab()) {
                std::cerr << "couldn't close train tab!" << std::endl;
            }
        } else {
            std::cerr << "couldn't switch to fast tab!" << std::endl;
        }
    } else {
        std::cerr << "couldn't open train troops!" << std::endl;
    }

    captureScreen();
    collectResources();

    if (openTrainTroops()) {
        captureScreen();

        if (checkIfTrained()) {
            if (closeTrainTab()) {
                captureScreen();

                if (findEnemy()) {
                    captureScreen();

                    if (!attack()) {
                        std::cerr << "failed to attack enemy!" << std::endl;
                    }
                } else {
                    std::cerr << "couldn't find enemy!" << std::endl;
                }
            } else {
                std::cerr << "couldn't close train tab!" << std::endl;
            }
        } else {
            if (!closeTrainTab()) {
                std::cerr << "couldn't close train tab!" << std::endl;
            }
            std::cerr << "troops aren't ready to fight!" << std::endl;
        }
    } else {
        std::cerr << "couldn't open train troops!" << std::endl;
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

std::vector<cv::Rect> Bot::findImageMatches(cv::Mat &ref, cv::Mat &tpl, double threshold) {
    std::vector<cv::Rect> rects;

    cv::Mat res_32f(ref.rows - tpl.rows + 1, ref.cols - tpl.cols + 1, CV_32FC1);
    cv::matchTemplate(ref, tpl, res_32f, cv::TM_CCOEFF_NORMED);

    cv::Mat res;
    res_32f.convertTo(res, CV_8U, 255.0);

    int size = ((tpl.cols + tpl.rows) / 4) * 2 + 1; //force size to be odd
    cv::threshold(res, res, threshold, 255.0, cv::THRESH_TOZERO);
    cv::adaptiveThreshold(res, res, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY, size, -128);

    while (true) {
        double minval, maxval;
        cv::Point minloc, maxloc;
        cv::minMaxLoc(res, &minval, &maxval, &minloc, &maxloc);

        if (maxval == 255) {
            rects.emplace_back(cv::Rect(maxloc, cv::Point(maxloc.x + tpl.cols, maxloc.y + tpl.rows)));
            cv::floodFill(res, maxloc, 0);
        } else
            break;
    }

    return rects;
}

bool Bot::openTrainTroops() {
    cv::Mat trainTroops = cv::imread("train_troops.png");
    auto matches = findImageMatches(screen, trainTroops, 240);

    if (matches.empty()) {
        return false;
    }

    TapDevice(matches[0].x + matches[0].width / 2, matches[0].y + matches[0].height / 2);

    return true;
}

bool Bot::switchFastTab() {
    cv::Mat fastTrainTab = cv::imread("fast_train_tab.png");
    auto matches = findImageMatches(screen, fastTrainTab, 0);

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
    cv::Mat trainGreen = cv::imread("train_green.png");
    auto matches = findImageMatches(screen, trainGreen, 240);

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
    //gold
    auto mat = cv::imread("collect_gold.png");
    auto matches = findImageMatches(screen, mat, 190);

    if (!matches.empty()) {
        TapDevice(matches[0].x + matches[0].width / 2, matches[0].y + matches[0].height / 2);
    } else {
        std::cerr << "couldn't collect any gold!" << std::endl;
    }

    //elixir
    mat = cv::imread("collect_elixir.png");
    matches = findImageMatches(screen, mat, 190);

    if (!matches.empty()) {
        TapDevice(matches[0].x + matches[0].width / 2, matches[0].y + matches[0].height / 2);
    } else {
        std::cerr << "couldn't collect any elixir!" << std::endl;
    }

    //dark elixir
    mat = cv::imread("collect_dark_elixir.png");
    matches = findImageMatches(screen, mat, 190);

    if (!matches.empty()) {
        TapDevice(matches[0].x + matches[0].width / 2, matches[0].y + matches[0].height / 2);
    } else {
        std::cerr << "couldn't collect any dark elixir!" << std::endl;
    }

    return true;
}

bool Bot::closeTrainTab() {
    cv::Mat trainGreen = cv::imread("close.png");
    auto matches = findImageMatches(screen, trainGreen, 200);

    if (matches.empty()) {
        return false;
    }

    TapDevice(matches[0].x + matches[0].width / 2, matches[0].y + matches[0].height / 2);

    return true;
}

bool Bot::checkIfTrained() {
    auto mat = cv::imread("troops_ready.png");
    auto matches = findImageMatches(screen, mat, 233);

    if (matches.empty()) {
        return false;
    }

    mat = cv::imread("spells_ready.png");
    matches = findImageMatches(screen, mat, 222);

    if (matches.empty()) {
        std::cerr << "spells not ready, ignoring..." << std::endl;
        //return false;
    }

    mat = cv::imread("heroes_ready.png");
    matches = findImageMatches(screen, mat, 222);

    if (matches.empty()) {
        std::cerr << "heroes not ready, ignoring..." << std::endl;
        //return false;
    }

    return true;
}

bool Bot::findEnemy() {
    auto mat = cv::imread("attack.png");
    auto matches = findImageMatches(screen, mat, 220);

    if (matches.empty()) {
        return false;
    }

    TapDevice(matches[0].x + matches[0].width / 2, matches[0].y + matches[0].height / 2);

    captureScreen();

    mat = cv::imread("find_fight.png");
    matches = findImageMatches(screen, mat, 220);

    if (matches.empty()) {
        return false;
    }

    TapDevice(matches[0].x + matches[0].width / 2, matches[0].y + matches[0].height / 2);

    return true;
}

bool Bot::attack() {
    auto mat = cv::imread("end_fight.png");
    auto matches = findImageMatches(screen, mat, 180);

    while (matches.empty()) {
        captureScreen();
        matches = findImageMatches(screen, mat, 180);
        std::cout << "no enemy found" << std::endl;
    }

    std::cout << "enemy found" << std::endl;

    mat = cv::imread("select_combat.png");
    matches = findImageMatches(screen, mat, 185);

    std::sort(matches.begin(), matches.end(), [](cv::Rect &a, cv::Rect &b) -> bool {
        return a.x > b.x;
    });

    std::this_thread::sleep_for(std::chrono::duration<double, std::milli>(5000));
    std::cout << "starting attack..." << std::endl;

    for (auto &r: matches) {
        if (r.y > 1000) {
            TapDevice(r.x + r.width / 2, r.y + r.height / 2);
            TapDevice(420, 200);
            TapDevice(420, 200);
            SwipeDevice(420, 200, 420, 200, 6000);
        }
    }

    mat = cv::imread("back_home.png");

    do {
        captureScreen();
        matches = findImageMatches(screen, mat, 180);
        std::cout << "still attacking" << std::endl;
    } while (matches.empty());

    TapDevice(matches[0].x + matches[0].width / 2, matches[0].y + matches[0].height / 2);

    return true;
}
