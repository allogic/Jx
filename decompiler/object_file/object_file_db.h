#ifndef OBJECT_FILE_DB_H
#define OBJECT_FILE_DB_H

#include <common/types.h>
#include <common/link_types.h>

#include <string>
#include <vector>

#include <object_file/linked_object_file.h>

#include <util/decompiler_type_system.h>

namespace decompiler
{
  struct object_file_meta
  {
    std::string name;
    u32 version;
    u32 hash;
  };

  struct object_file_data
  {
    object_file_meta meta;
    linked_object_file linked_obj;
    std::vector<u8> data;
    std::vector<std::string> dgo_names;
  };

  struct object_file_db
  {
    object_file_db() = default;

    void get_objects_from_dgo(const std::vector<u8>& in, u8 is_pal);
    void add_object_from_dgo(const std::string& dgo_name, const std::string& obj_name, const u8* data, u32 size);

    void extract_dgos(const std::string& root_path);
    void process_linked_data();
    void find_code();

    decompiler_type_system m_dts = {};

    std::vector<object_file_data> m_objects = {};
    std::vector<object_file_data> m_objects_failed = {};
  };
}

#endif