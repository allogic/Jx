#ifndef TEXT_DB_H
#define TEXT_DB_H

#include <common/types.h>

#include <string>

namespace goos
{
  struct text_db
  {
    struct short_info
    {
      std::string filename = "";
      i32 line_idx_to_display = -1;
      i32 pos_in_line = -1;
      std::string line_text = "";
    };

    text_db() = default;


  };

}

#endif