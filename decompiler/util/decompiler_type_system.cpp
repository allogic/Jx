#include <cassert>

#include <util/decompiler_type_system.h>

namespace decompiler
{
  void decompiler_type_system::add_symbol(const std::string& name)
  {
    //if (symbols.find(name) == symbols.end())
    //{
    //  symbols.insert(name);
    //  symbol_add_order.push_back(name);
    //}
  }

  void decompiler_type_system::add_symbol(const std::string& name, const std::string& base_type, const definition_metadata& symbol_metadata)
  {
    //add_symbol(name, type_spec{ base_type }, symbol_metadata);
  }

  void decompiler_type_system::add_symbol(const std::string& name, const type_spec& type_spec, const definition_metadata& symbol_metadata)
  {
    //add_symbol(name);
    //auto it = symbol_types.find(name);
    //if (it == symbol_types.end() || it->second == type_spec)
    //{
    //  symbol_types[name] = type_spec;
    //  if (symbol_metadata.definition_info)
    //  {
    //    symbol_metadata_map[name] = symbol_metadata;
    //  }
    //}
    //else
    //{
    //  if (ts.tc(type_spec, it->second))
    //  {
    //
    //  }
    //  else
    //  {
    //    assert(0);
    //  }
    //}
  }
}
