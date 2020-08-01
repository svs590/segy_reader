#pragma once

#include <string>
#include <map>
#include <any>

#include <boost/filesystem.hpp>
namespace bfs = boost::filesystem;

#include "data_types.h"

#define NOT_INDEX -1

#define TO_USERFRIENDLY_STRING(str)                                             \
    first_to_upper(regex_replace(str, regex("_"), " "))

#define FROM_USERFRIENDLY_STRING(str)                                           \
    first_to_lower(regex_replace(str, regex(" "), "_"))

enum class modifier { read_only, read_write };

std::wstring utf8_to_unicode(const std::string& internal);

std::string tab_to_space(const std::string& str);

long long get_available_memory();

void seekg_relative(int64_t rel_pos, bfs::ifstream &stream);

bool is_little_endian();

std::string date_time();

std::string first_to_upper(const std::string &s);
std::string first_to_lower(const std::string &s);
