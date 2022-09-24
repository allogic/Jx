#ifndef DECOMPILER_LABEL_H
#define DECOMPILER_LABEL_H

#include <common/types.h>

#include <string>

namespace decompiler
{
  struct decompiler_label
  {
    std::string name;
    u32 target_segment;
    u32 offset;
  };
}

#endif