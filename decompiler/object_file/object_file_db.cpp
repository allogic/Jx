#include <common/binary_reader.h>
#include <common/bin_util.h>
#include <common/file_util.h>

#include <cassert>

#include <object_file/object_file_db.h>
#include <object_file/linked_object_file_creation.h>

namespace decompiler
{
  void object_file_db::get_objects_from_dgo(const std::vector<u8>& in, u8 is_pal)
  {
    binary_reader reader = binary_reader{ (u8*)in.data(), (u32)in.size() };
    dgo_header main_header = reader.read<dgo_header>();
    for (u32 i = 0; i < main_header.size; i++)
    {
      dgo_header obj_header = reader.read<dgo_header>();
      if (i == main_header.size - 1)
      {
        if (reader.bytes_left() == obj_header.size - 0x30)
        {
          if (is_pal)
          {
            reader.fwd(reader.bytes_left());
            continue;
          }
          else
          {
            assert(0);
          }
        }
      }
      else
      {
        assert(reader.bytes_left() >= obj_header.size);
      }
      if (std::string{ obj_header.name }.find("-ag") != std::string::npos)
      {
        assert(0);
      }
      // TODO: Check name
      add_object_from_dgo(main_header.name, obj_header.name, reader.here(), obj_header.size);
      reader.fwd(align16(obj_header.size));
    }
    assert(reader.bytes_left() == 0);
  }

  void object_file_db::add_object_from_dgo(const std::string& dgo_name, const std::string& obj_name, const u8* data, u32 size)
  {
    assert(size > 128);
    // TODO: Check banned
    // TODO: Check duplicates
    // TODO: Check CRC32
    object_file_data obj;
    obj.meta.name = obj_name;
    obj.meta.version = (u32)(*(u16*)data + 8);
    obj.meta.hash = 0;
    obj.data.resize(size);
    std::memcpy(obj.data.data(), data, size);
    obj.dgo_names.emplace_back(dgo_name);
    m_objects.emplace_back(obj);
  }

  void object_file_db::extract_dgos(const std::string& root_path)
  {
    std::printf("=============================================\n");
    std::printf("Extracting objects... ");
    std::vector<std::string> dgos;
    if (file_util::collect_file_paths_by_extension(root_path + "\\DGO", ".DGO", dgos))
    {
      for (const std::string& dgo : dgos)
      {
        std::vector<u8> comp;
        if (file_util::read_binary_file(dgo, comp))
        {
          std::vector<u8> decomp;
          if (file_util::decompress_dgo(comp, decomp))
          {
            get_objects_from_dgo(decomp, 1);
          }
        }
      }
    }
    std::printf("OK!\n");
    std::printf("Extracted %u objects\n", (u32)m_objects.size());
    std::printf("=============================================\n");
    std::printf("\n");
  }

  void object_file_db::process_linked_data()
  {
    std::printf("=============================================\n");
    std::printf("Processing linked data... ");
    for (std::vector<object_file_data>::iterator it = m_objects.begin(); it != m_objects.end();)
    {
      if (to_linked_object_file(it->data, it->meta.name, it->linked_obj, m_dts) == 1)
      {
        it++;
      }
      else
      {
        m_objects_failed.emplace_back(*it);
        it = m_objects.erase(it);
      }
    }
    std::printf("OK!\n");
    std::printf("Found %u valid objects\n", (u32)m_objects.size());
    std::printf("Found %u invalid objects\n", (u32)m_objects_failed.size());
    std::printf("=============================================\n");
    std::printf("\n");
  }

  void object_file_db::find_code()
  {
    std::printf("=============================================\n");
    std::printf("Finding code in object files... ");
    linked_object_file::stats stats;
    for (object_file_data& obj : m_objects)
    {
      obj.linked_obj.find_code();
      obj.linked_obj.find_functions();
      obj.linked_obj.disassemble_functions();
      stats.add(obj.linked_obj.m_stats);
    }
    std::printf("OK!\n");
    std::printf("Code %.3f MB\n", stats.code_bytes / (f32)(1 << 20));
    std::printf("Data %.3f MB\n", stats.data_bytes / (f32)(1 << 20));
    std::printf("Functions %u\n", stats.function_count);
    std::printf("=============================================\n");
    std::printf("\n");
  }
}