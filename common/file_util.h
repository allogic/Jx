#ifndef FILE_UTIL_H
#define FILE_UTIL_H

#include <common/types.h>

#include <string>
#include <vector>

namespace file_util
{
  u8 collect_file_paths_by_extension(const std::string& root_path, const std::string& extension, std::vector<std::string>& out);
  u8 read_binary_file(const std::string& file_name, std::vector<u8>& out);
  u8 decompress_dgo(const std::vector<u8>& in, std::vector<u8>& out);
}

#endif