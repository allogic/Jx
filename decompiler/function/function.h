#ifndef FUNCTION_H
#define FUNCTION_H

#include <common/types.h>

namespace decompiler
{
  struct function
  {
    function(u32 start_word, u32 end_word);

    u32 m_start_word;
    u32 m_end_word;
  };
}

#endif