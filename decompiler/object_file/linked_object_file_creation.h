#ifndef LINKED_OBJECT_FILE_CREATION_H
#define LINKED_OBJECT_FILE_CREATION_H

#include <common/types.h>

#include <string>
#include <vector>

#include <object_file/linked_object_file.h>

#include <util/decompiler_type_system.h>

namespace decompiler
{
  u8 to_linked_object_file(const std::vector<u8>& data, const std::string& name, linked_object_file& lof, decompiler_type_system& dts);
}

#endif