//
// Created by Thomas Lienbacher on 23.04.2019.
//

#ifndef COCBOT_DEVICE_HPP
#define COCBOT_DEVICE_HPP

#include <string>

bool IsAdbAvailable();

bool IsDeviceConnected();

void PrintAdbDevice();

void CreateScreenshot();

int RunCommandOutput(const std::string &&cmd, std::ifstream *output);

int RunCommand(const std::string &&cmd);

void StartCoC();

bool DeviceUnlockedAndAwake();

void TapDevice(int x, int y);


#endif //COCBOT_DEVICE_HPP
