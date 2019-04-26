//
// Created by Thomas Lienbacher on 25.04.2019.
//

#include "bot.hpp"

#include <iostream>


Bot::Bot() {

}

Bot::~Bot() {

}

bool Bot::process() {
    std::cout << "Processing screenshot..." << std::endl;

    //do work

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
