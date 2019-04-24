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
    HANDLE read = nullptr;
    HANDLE write = nullptr;
    //RunCommandOutput("adb devices -l", &read, &write);
    RunCommandOutput("CoCBot.exe", &read, &write);

    char buffer[256];
    fread(buffer, sizeof(char), 255, _popen("CoCBot.exe", "r"));
    std::cout << ": " << std::string(buffer) << std::endl;
}

void CreateScreenshot() {
    RunCommand("adb shell screencap -p /sdcard/screen.png");
    RunCommand("adb pull /sdcard/screen.png");
}

bool IsDeviceConnected() {
    return RunCommand("adb get-state") == 0;
}

int RunCommandOutput(const std::string &&cmd, HANDLE *readStdout, HANDLE *writeStdout) {
    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = nullptr;

    CreatePipe(readStdout, writeStdout, &sa, 0);
    SetHandleInformation(*readStdout, HANDLE_FLAG_INHERIT, 0);

    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.hStdOutput = writeStdout;
    si.hStdError = writeStdout;
    si.dwFlags |= STARTF_USESTDHANDLES;
    ZeroMemory(&pi, sizeof(pi));

    if (!CreateProcessA(nullptr,
                        const_cast<char *>(cmd.c_str()),
                        nullptr,
                        nullptr,
                        TRUE,
                        0,
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

int RunCommand(const std::string &&cmd) {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    if (!CreateProcessA(nullptr,
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
