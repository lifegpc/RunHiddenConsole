/*  This file is a part of RunHiddenConsole.
    Copyright (C) 2021 lifegpc

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifdef HAVE_RHC_CONFIG_H
#include "config.h"
#endif

#include "getopt.h"
#include <stdio.h>
#include <string>
#include <Windows.h>
#include "util.h"
#include <list>

#ifdef HAVE_PRINTF_S
#define printf printf_s
#endif

#ifdef HAVE_FPRINTF_S
#define fprintf fprintf_s
#endif

using namespace std;

int main(int argc, char* argv[]) {
    const struct option opts[] = {
        {"help", 0, nullptr, 'h'},
        {"version", 0, nullptr, 'v'},
        nullptr
    };
    bool use_wv = false;
#if defined(HAVE_GETCOMMANDLINEW) && defined(HAVE_COMMANDLINETOARGVW)
    char** nargv = nullptr;
    auto cl = GetCommandLineW();
    int argcw = 0;
    auto argvw = CommandLineToArgvW(cl, &argcw);
    if (argvw != nullptr) {
        if (util::ArgvWToArgv(argvw, argcw, nargv)) {
            use_wv = true;
        }
        LocalFree(argvw);
    }
    auto rargc = use_wv ? argcw : argc;
    auto rargv = use_wv ? nargv : argv;
#else
    auto rargc = argc;
    auto rargv = argv;
#endif
    list<string> cml;
    int c;
    while ((c = getopt_long(rargc, rargv, "+vh", opts, nullptr)) != -1) {
        switch (c)
        {
        case 'h':
            printf("RunHiddenConsole: Start console program without console window.\n\
RunHiddenConsole [options] <command line>\n\
Options:\n\
-v  --version   Show program version and then exit.\n\
-h  --help      Show help information and then exit.\n");
            if (use_wv) util::freeArgv(rargv, rargc);
            return 0;
        case 'v':
        {
            string cc = "";
#ifdef _MSC_VER
            cc += "MSVC v" + to_string(_MSC_FULL_VER / 10000000) + "." + to_string((_MSC_FULL_VER % 10000000) / 100000) + "." + to_string(_MSC_FULL_VER % 100000) + "." + to_string(_MSC_BUILD);
#endif
#ifdef __GNUC__
            cc += string("GCC v") + __VERSION__;
#endif
#ifdef __clang__
            cc += string("Clang v") + __clang_version__;
#endif
            if (cc.empty()) cc = "Unknown";
            printf("RunHiddenConsole  Copyright (C) 2021  lifegpc\n\
This program comes with ABSOLUTELY NO WARRANTY.\n\
This is free software, and you are welcome to redistribute it\n\
under GNU GPL Version 3.\n\
Version: %s\n\
Build with %s\n", "1.0.1", cc.c_str());
        }
            if (use_wv) util::freeArgv(rargv, rargc);
            return 0;
        case '?':
        default:
            fprintf(stderr, "Use -h to see help.");
            if (use_wv) util::freeArgv(rargv, rargc);
            return -1;
        }
    }
    for (int i = optind; i < rargc; i++) {
        cml.push_back(rargv[i]);
    }
    if (use_wv) util::freeArgv(rargv, rargc);
    if (cml.empty()) {
        fprintf(stderr, "No command line find.");
        return -1;
    }
    string AppName, CommandLine;
    AppName = "";
    for (auto i = cml.begin(); i != cml.end(); ++i) {
        auto s = *i;
        if (s.find(' ') != string::npos) s = "\"" + s + "\"";
        CommandLine += CommandLine.empty() ? s : " " + s;
    }
    if (use_wv) {
        std::wstring wAppName, wCommandLine;
        if (util::UTF8ToWString(AppName, wAppName) && util::UTF8ToWString(CommandLine, wCommandLine)) {
            auto temp = (wchar_t*)malloc(sizeof(wchar_t) * (wCommandLine.size() + 1000));
            if (!temp) {
                fprintf(stderr, "Can not allocate memory, needed memory: %zi", sizeof(wchar_t) * (wCommandLine.size() + 1000));
                return -1;
            }
            memcpy(temp, wCommandLine.c_str(), sizeof(wchar_t) * wCommandLine.size());
            temp[wCommandLine.size()] = 0;
            STARTUPINFOW si;
            PROCESS_INFORMATION pi;
            ZeroMemory(&si, sizeof(si));
            si.cb = sizeof(si);
            ZeroMemory(&pi, sizeof(pi));
            if (CreateProcessW(nullptr, temp, nullptr, nullptr, FALSE, CREATE_NO_WINDOW, nullptr, nullptr, &si, &pi)) {
                CloseHandle(pi.hProcess);
                CloseHandle(pi.hThread);
            } else {
                fprintf(stderr, "CreateProcess failed (%d).", GetLastError());
            }
            free(temp);
        }
        else {
            STARTUPINFOA si;
            PROCESS_INFORMATION pi;
            ZeroMemory(&si, sizeof(si));
            si.cb = sizeof(si);
            ZeroMemory(&pi, sizeof(pi));
            auto temp = (char*)malloc(CommandLine.size() + 1000);
            if (!temp) {
                fprintf(stderr, "Can not allocate memory, needed memory: %zi", CommandLine.size() + 1000);
                return -1;
            }
            memcpy(temp, CommandLine.c_str(), CommandLine.size());
            temp[CommandLine.size()] = 0;
            if (CreateProcessA(nullptr, temp, nullptr, nullptr, FALSE, CREATE_NO_WINDOW, nullptr, nullptr, &si, &pi)) {
                CloseHandle(pi.hProcess);
                CloseHandle(pi.hThread);
            } else {
                fprintf(stderr, "CreateProcess failed (%d).", GetLastError());
            }
            free(temp);
        }
    }
    return 0;
}
