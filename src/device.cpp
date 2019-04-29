//
// Created by Thomas Lienbacher on 23.04.2019.
//

#include "device.hpp"
#include "util.hpp"

#include <cstdlib>
#include <iostream>
#include <iostream>
#include <fstream>
#include <sstream>

#define WIN32_LEAN_AND_MEAN

#include <windows.h>

bool IsAdbAvailable() {
    return RunCommand("adb --version") == 0;
}

void PrintAdbDevice() {
    std::ifstream output;
    RunCommandOutput("adb devices -l", &output);

    std::string line;
    std::getline(output, line), std::getline(output, line);
    std::cout << "Connected device:\n" << line << std::endl;
}

void CreateScreenshot() {
    std::cout << "Capturing screen..." << std::endl;
    RunCommand("adb shell screencap -p /sdcard/screen.png");
    RunCommand("adb pull /sdcard/screen.png");
}

bool IsDeviceConnected() {
    return RunCommand("adb get-state") == 0;
}

int RunCommandOutput(const std::string &&cmd, std::ifstream *output) {
    HANDLE temp = CreateFileA(
            "temp",
            GENERIC_WRITE | GENERIC_WRITE,
            FILE_SHARE_WRITE | FILE_SHARE_READ,
            nullptr,
            CREATE_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,
            nullptr);
    SetHandleInformation(temp, HANDLE_FLAG_INHERIT, 1);

    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    SECURITY_ATTRIBUTES sa;

    ZeroMemory(&si, sizeof(si));
    ZeroMemory(&pi, sizeof(pi));
    ZeroMemory(&sa, sizeof(sa));

    si.cb = sizeof(si);
    si.hStdOutput = temp;
    si.hStdError = nullptr;
    si.hStdInput = nullptr;
    si.dwFlags |= STARTF_USESTDHANDLES;

    sa.nLength = sizeof(sa);
    sa.lpSecurityDescriptor = nullptr;
    sa.bInheritHandle = TRUE;

    if (!CreateProcessA(
            nullptr,
            const_cast<char *>(cmd.c_str()),
            &sa,
            nullptr,
            TRUE,
            CREATE_NO_WINDOW,
            nullptr,
            nullptr,
            &si,
            &pi)
            ) {
        Error("couldn't start command: '" + cmd + "'!");
    }

    WaitForSingleObject(pi.hProcess, INFINITE);

    DWORD ret;
    GetExitCodeProcess(pi.hProcess, &ret);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    CloseHandle(temp);

    *output = std::ifstream("temp");

    return ret;
}

int RunCommand(const std::string &&cmd) {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    if (!CreateProcessA(
            nullptr,
            const_cast<char *>(cmd.c_str()),
            nullptr,
            nullptr,
            FALSE,
            CREATE_NO_WINDOW,
            nullptr,
            nullptr,
            &si,
            &pi)
            ) {
        Error("couldn't start command: '" + cmd + "'!");
    }

    WaitForSingleObject(pi.hProcess, INFINITE);

    DWORD ret;
    GetExitCodeProcess(pi.hProcess, &ret);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return ret;
}

void StartCoC() {
    std::cout << "Starting Clash of Clans..." << std::endl;
    RunCommand("adb shell monkey -p com.supercell.clashofclans 1");
}

bool DeviceUnlockedAndAwake() {
    std::ifstream output;
    RunCommandOutput("adb shell dumpsys window", &output);

    bool awake = false;
    bool showingLockscreen = false;
    bool showingDream = false;
    bool dreamingLockscreen = false;

    std::string line;
    while (std::getline(output, line)) {
        if (line.find("mAwake") != std::string::npos) {
            awake = line.find("mAwake=true") != std::string::npos;
        }

        if (line.find("mShowingLockscreen") != std::string::npos) {
            showingLockscreen = line.find("mShowingLockscreen=true") != std::string::npos;
        }

        if (line.find("mShowingDream") != std::string::npos) {
            showingDream = line.find("mShowingDream=true") != std::string::npos;
        }

        if (line.find("mDreamingLockscreen") != std::string::npos) {
            dreamingLockscreen = line.find("mDreamingLockscreen=true") != std::string::npos;
        }
    }

    return awake && !showingLockscreen && !showingDream && !dreamingLockscreen;
}

void TapDevice(int x, int y) {
    std::cout << "Tap at: " << x << " " << y << std::endl;
    RunCommand("adb shell input tap " + std::to_string(x) + " " + std::to_string(y));
}

void SwipeDevice(int x1, int y1, int x2, int y2, int ms) {
    std::cout << "Swipe from: " << x1 << " " << y2
              << " to " << x2 << " " << y2 << " for " << ms << "ms" << std::endl;
    RunCommand("adb shell input touchscreen swipe "
               + std::to_string(x1) + " "
               + std::to_string(y1) + " "
               + std::to_string(x2) + " "
               + std::to_string(y2) + " "
               + std::to_string(ms));
}

