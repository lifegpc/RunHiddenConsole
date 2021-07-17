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

#ifndef _RHC_UTIL_H
#define _RHC_UTIL_H

#include <string>
#include <Windows.h>

using namespace std;

namespace util {
    /**
     * @brief Convert wstring to string with UTF-8 encoding
     * @param source Wstring
     * @param dest Result
     * @return true if OK
    */
    bool WStringToUTF8(wstring source, string& dest);
    /**
     * @brief Get correct options for WideCharToMultiByte
     * @param ori_options Origin options
     * @param cp Code Page
     * @return Result options
    */
    DWORD getWideCharToMultiByteOptions(const DWORD ori_options, UINT cp);
    /**
     * @brief Convert wstring version argv to UTF-8 encoding argv
     * @param ArgvW Source
     * @param argc The count of parameters
     * @param argv dest (Need free memory by calling freeArgv)
     * @return true if OK
    */
    bool ArgvWToArgv(const LPWSTR* ArgvW, int argc, char**& argv);
    void freeArgv(char** argv, int argc);
    /**
     * @brief Convert string with UTF-8 encoding to wstring
     * @param source string (UTF-8)
     * @param dest Result
     * @return true if OK
    */
    bool UTF8ToWString(string source, wstring& dest);
    /**
     * @brief Get correct options for MultiByteToWideChar
     * @param ori_options Origin options
     * @param cp Code Page
     * @return Result options
    */
    DWORD getMultiByteToWideCharOptions(const DWORD ori_options, UINT cp);
}

#endif
