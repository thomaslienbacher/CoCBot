//
// Created by Thomas Lienbacher on 25.04.2019.
//

#include <iostream>
#include "util.hpp"

void Error(const std::string &&msg) {
    std::cerr << "Error: " << msg << std::endl;
    std::exit(1);
}

void PrintTitle() {
    std::cout << "Computer Assistet Clash of Clans\n";
    std::cout << "by Thomas Lienbacher\n";
    std::cout << "built on: " << std::string(__TIMESTAMP__) << "\n";
    std::cout << std::endl;
}