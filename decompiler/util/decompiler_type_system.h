#ifndef DECOMPILER_TYPE_SYSTEM_H
#define DECOMPILER_TYPE_SYSTEM_H

#include <common/type_system/type.h>
#include <common/type_system/type_spec.h>

#include <string>

namespace decompiler
{
  struct decompiler_type_system
  {
    decompiler_type_system() = default;

    void add_symbol(const std::string& name);
    void add_symbol(const std::string& name, const std::string& base_type, const definition_metadata& symbol_metadata);
    void add_symbol(const std::string& name, const type_spec& type_spec, const definition_metadata& symbol_metadata);
  };
}

#endif