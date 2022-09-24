#ifndef DEBUG_UTIL_H
#define DEBUG_UTIL_H

#include <common/types.h>

#include <vector>

namespace debug_util
{
  void dump_hex(const std::vector<u8>& in, u32 offset, u32 amount, u8 mod);
}

#endif