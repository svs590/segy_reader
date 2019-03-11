#ifndef UTILS
#define UTILS

#include <string>

#define NOT_INDEX -1

#define DLLIMPORT extern "C" __declspec(dllimport)

std::wstring utf8_to_unicode(const std::string& internal);

#endif
