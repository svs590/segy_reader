#pragma once

#include <string>
#include <map>
#include <any>

#include <boost/filesystem.hpp>
namespace bfs = boost::filesystem;

#include "data_types.h"

#define NOT_INDEX -1

#define DLLIMPORT extern "C" __declspec(dllimport)

enum class modifier { read_only, read_write };

std::wstring utf8_to_unicode(const std::string& internal);

std::string tab_to_space(const std::string& str);

long long get_available_memory();

void seekg_relative(int64_t rel_pos, bfs::ifstream &stream);

bool is_little_endian();
