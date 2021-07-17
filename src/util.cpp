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

#include "util.h"
#include <stdio.h>
#include <list>

#ifdef HAVE_FPRINTF_S
#define fprintf fprintf_s
#endif

bool stringToChar(string input, char*& output) {
	auto sz = input.size();
	auto s = (char*)malloc(sz + 1);
	if (!s) {
		fprintf(stderr, "Can not allocate memory, needed memory: %zi", sz + 1);
		return false;
	}
	memcpy(s, input.c_str(), sz);
	s[sz] = 0;
	output = s;
	return true;
}

void freeChar(char* input) {
	if (input) free(input);
}

template <typename T, typename R>
bool listToPointer(list<T> li, R*& result, bool (*convert)(T input, R& output), void (*free)(R input)) {
	if (!convert) return false;
	auto sz = li.size();
	if (sz <= 0) return false;
	R* r = (R*)malloc(sz * sizeof(void*));
	if (!r) {
		fprintf(stderr, "Can not allocate memory, needed memory: %zi", sz * sizeof(void*));
		return false;
	}
	size_t j = 0;
	for (auto i = li.begin(); i != li.end(); ++i) {
		auto s = *i;
		R t;
		if (!convert(s, t)) {
			if (!free) {
				for (size_t z = 0; z < j; z++) {
					free(r[z]);
				}
			}
			::free(r);
			return false;
		}
		r[j] = t;
		j++;
	}
	result = r;
	return true;
}

template <typename T>
void freePointerList(T* li, size_t count, void (*free)(T)) {
	if (!li) return;
	for (size_t i = 0; i < count; i++) {
		if (!free) ::free(li[i]);
		else free(li[i]);
	}
	::free(li);
}

bool util::WStringToUTF8(wstring source, string& dest) {
    if (source.empty()) {
        dest = "";
        return true;
    }
	auto opt = getWideCharToMultiByteOptions(WC_ERR_INVALID_CHARS, CP_UTF8);
	int len;
	char* s;
	len = WideCharToMultiByte(CP_UTF8, opt, source.c_str(), -1, nullptr, 0, nullptr, FALSE);
	if (!len) {
		fprintf(stderr, "Can not convert wstring to UTF8.");
		return false;
	}
	s = (char*)malloc(len + 1);
	if (!s) {
		fprintf(stderr, "Can not allocate memory, needed memory: %i", len + 1);
		return false;
	}
	if (!WideCharToMultiByte(CP_UTF8, opt, source.c_str(), -1, s, len, nullptr, FALSE)) {
		fprintf(stderr, "Can not convert wstring to UTF8.");
		free(s);
		return false;
	}
	s[len] = 0;
	dest = s;
	free(s);
	return true;
}

DWORD util::getWideCharToMultiByteOptions(const DWORD ori_options, UINT cp) {
	if (cp == CP_ACP) cp = GetACP();
	if (cp == CP_OEMCP) cp = GetOEMCP();
	switch (cp)
	{
	case 50220:
	case 50221:
	case 50222:
	case 50225:
	case 50227:
	case 50229:
	case CP_UTF7:
	case 42:
		return 0;
	default:
		break;
	}
	if (cp >= 57002 && cp <= 57011) return 0;
	if (cp == CP_UTF8 || cp == 54936) {
		return WC_ERR_INVALID_CHARS & ori_options;
	}
	return ori_options;
}

bool util::ArgvWToArgv(const LPWSTR* ArgvW, int argc, char**& argv) {
	if (!ArgvW) return false;
	list<string> li;
	for (int i = 0; i < argc; i++) {
		if (!ArgvW[i]) return false;
		std::wstring s = ArgvW[i];
		std::string r;
		if (!WStringToUTF8(s, r)) return false;
		li.push_back(r);
	}
	char** t;
	if (!listToPointer<string, char*>(li, t, &stringToChar, &freeChar)) {
		return false;
	}
	argv = t;
	return true;
}

void util::freeArgv(char** argv, int argc) {
	freePointerList<char*>(argv, argc, nullptr);
}

bool util::UTF8ToWString(string source, wstring& dest) {
	if (source.empty()) {
		dest = L"";
		return true;
	}
	auto opt = getMultiByteToWideCharOptions(MB_ERR_INVALID_CHARS, CP_UTF8);
	int len;
	wchar_t* s;
	len = MultiByteToWideChar(CP_UTF8, opt, source.c_str(), -1, nullptr, 0);
	if (!len) {
		fprintf(stderr, "Can not convert UTF8 to wstring.");
		return false;
	}
	s = (wchar_t*)malloc((len + 1) * sizeof(wchar_t));
	if (!s) {
		fprintf(stderr, "Can not allocate memory, needed memory: %zi", (len + 1) * sizeof(wchar_t));
		return false;
	}
	if (!MultiByteToWideChar(CP_UTF8, opt, source.c_str(), -1, s, len)) {
		fprintf(stderr, "Can not convert UTF8 to wstring.");
		free(s);
		return false;
	}
	s[len] = 0;
	dest = s;
	free(s);
	return true;
}

DWORD util::getMultiByteToWideCharOptions(const DWORD ori_options, UINT cp) {
	if (cp == CP_ACP) cp = GetACP();
	if (cp == CP_OEMCP) cp = GetOEMCP();
	switch (cp)
	{
	case 50220:
	case 50221:
	case 50222:
	case 50225:
	case 50227:
	case 50229:
	case CP_UTF7:
	case 42:
		return 0;
	default:
		break;
	}
	if (cp >= 57002 && cp <= 57011) return 0;
	if (cp == CP_UTF8 || cp == 54936) {
		return MB_ERR_INVALID_CHARS & ori_options;
	}
	return ori_options;
}
