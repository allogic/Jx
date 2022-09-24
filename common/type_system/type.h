#ifndef TYPE_H
#define TYPE_H

#include <common/goos/text_db.h>

#include <string>
#include <optional>

// Various metadata that can be associated with a symbol or form
struct definition_metadata
{
  std::optional<goos::text_db::short_info> definition_info;
  std::optional<std::string> docstring;
};

#endif