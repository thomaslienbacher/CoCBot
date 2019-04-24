//
// Created by Thomas Lienbacher on 23.04.2019.
//

#include "bot.hpp"

#include <cstdlib>
#include <iostream>
#include <windows.h>

void Error(const std::string &&msg) {
    std::cerr << "Error: " << msg << std::endl;
    exit(1);
}

bool IsAdbAvailable() {
    return RunCommand("adb --version") == 0;
}

void PrintAdbDevice() {
    FILE *output;
    //RunCommandOutput("adb devices -l", &read, &write);
    RunCommandOutput("CoCBot.exe", &output);

    char buffer[256];
    fread(buffer, sizeof(char), 255, output);
    std::cout << ": " << std::string(buffer) << std::endl;
    fclose(output);
}

void CreateScreenshot() {
    RunCommand("adb shell screencap -p /sdcard/screen.png");
    RunCommand("adb pull /sdcard/screen.png");
}

bool IsDeviceConnected() {
    return RunCommand("adb get-state") == 0;
}

int RunCommandOutput(const std::string &&cmd, FILE **output) {
    *output = popen(cmd.c_str(), "r");

    if (!*output) {
        Error("couldn't start command: '" + cmd + "'!");
    }

    int ret;
    printf("cwait: %d\n", cwait(&ret, 0, 0));

    return ret;
}

int RunCommand(const std::string &&cmd) {
    FILE *output = popen(cmd.c_str(), "r");

    if (!output) {
        Error("couldn't start command: '" + cmd + "'!");
    }

    return pclose(output);
}
