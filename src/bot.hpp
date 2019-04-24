//
// Created by Thomas Lienbacher on 23.04.2019.
//

#ifndef COCBOT_BOT_HPP
#define COCBOT_BOT_HPP

#include <string>
#include <windows.h>

void Error(const std::string &&msg) __attribute__((noreturn));

bool IsAdbAvailable();

bool IsDeviceConnected();

void PrintAdbDevice();

void CreateScreenshot();

int RunCommandOutput(const std::string &&cmd, HANDLE *readStdout, HANDLE *writeStdout);

int RunCommand(const std::string &&cmd);

#endif //COCBOT_BOT_HPP
