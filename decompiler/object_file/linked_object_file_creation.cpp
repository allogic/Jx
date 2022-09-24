#include <common/bin_util.h>

#include <cassert>

#include <object_file/linked_object_file_creation.h>

namespace decompiler
{
  // Header for link data used for V2, V3, V4 objects. For V3/V4, this is found at the beginning of
  // the object data.
  struct link_header_common
  {
    u32 type_tag; // for the basic offset, is 0 or -1 depending on version
    u32 length;   // different exact meanings, but length of the link data.
    u16 version;  // what version (2, 3, 4)
  };

  // Per-segment info for V3 and V5 link data
  struct segment_info
  {
    u32 relocs; // offset of relocation table
    u32 data;   // offset of segment data
    u32 size;   // segment data size (0 if segment doesn't exist)
    u32 magic;  // always 0
  };

  struct link_header_v5
  {
    u32 type_tag;                 // 0 always 0?
    u32 length_to_get_to_code;    // 4 length.. of link data?
    u16 version;                  // 8
    u16 unknown;                  // 10
    u32 pad;                      // 12
    u32 link_length;              // 16
    u8 segments;                  // 20
    i8 name[59];                  // 21 (really??)
    segment_info segment_info[3]; // segment link data info
  };

  // The types of symbol links
  enum symbol_link_kind
  {
    EMPTY_LIST, // link to the empty list
    TYPE,       // link to a type
    SYMBOL,     // link to a symbol
  };

  static u32 c_symlink2(linked_object_file& lof, const std::vector<u8>& data, u32 code_ptr_offset, u32 link_ptr_offset, symbol_link_kind kind, const i8* name, u32 segment_id, decompiler_type_system& dts)
  {
    dts.add_symbol(name);
    u32 initial_offset = code_ptr_offset;
    do
    {
      u8 table_value = data[link_ptr_offset];
      const u8* reloc_ptr = &data[link_ptr_offset];
      // link table has a series of variable-length-encoded integers indicating the seek amount to hit
      // each reference to the symbol. It ends when the seek is 0, and all references to this symbol
      // have been patched.
      u32 seek = table_value;
      u32 next_reloc = link_ptr_offset + 1;
      if (seek & 3)
      {
        // 0b01, 0b10
        seek = (reloc_ptr[1] << 8) | table_value;
        next_reloc = link_ptr_offset + 2;
        if (seek & 2)
        {
          // 0b10
          seek = (reloc_ptr[2] << 16) | seek;
          next_reloc = link_ptr_offset + 3;
          if (seek & 1)
          {
            // 0b11
            seek = (reloc_ptr[3] << 24) | seek;
            next_reloc = link_ptr_offset + 4;
          }
        }
      }
      lof.m_stats.total_v2_symbol_links++;
      link_ptr_offset = next_reloc;
      code_ptr_offset += seek & 0xFFFFFFFC;
      // the value of the code gives us more information
      u32 code_value = *(const u32*)&data[code_ptr_offset];
      if (code_value == 0xFFFFFFFF)
      {
        // absolute link - replace entire word with a pointer.
        linked_word::kind word_kind;
        switch (kind)
        {
          case symbol_link_kind::SYMBOL:
            word_kind = linked_word::SYM_PTR;
            break;
          case symbol_link_kind::EMPTY_LIST:
            word_kind = linked_word::EMPTY_PTR;
            break;
          case symbol_link_kind::TYPE:
            dts.add_symbol(name, "type", {});
            word_kind = linked_word::TYPE_PTR;
            break;
          default:
            assert(0);
        }
        lof.symbol_link_word(segment_id, code_ptr_offset - initial_offset, name, word_kind);
      }
      else
      {
        // offset link - replace lower 16 bits with symbol table offset.
        assert((code_value & 0xFFFF) == 0 || (code_value & 0xFFFF) == 0xFFFF);
        assert(kind == symbol_link_kind::SYMBOL);
        lof.symbol_link_offset(segment_id, code_ptr_offset - initial_offset, name);
      }
    } while (data[link_ptr_offset] > 0);
    return link_ptr_offset + 1;
  }

  static u8 link_v5(linked_object_file& lof, const std::vector<u8>& data, const std::string& name, decompiler_type_system& dts)
  {
    const link_header_v5* header = (const link_header_v5*)&data[0];
    // TODO: disassemble lobby-secrets
    if (name == "lobby-secrets")
    {
      return 0;
    }
    if (header->segments == 1)
    {
      return 0;
    }
    assert(header->type_tag == 0);
    assert(header->name == name);
    assert(header->segments == 3);
    assert(header->pad == 0x50);
    assert(header->length_to_get_to_code - header->link_length == 0x50);
    lof.set_segment_count(3);
    u32 data_ptr_offset = header->length_to_get_to_code;
    u32 segment_data_offsets[3];
    u32 segment_link_offsets[3];
    u32 segment_link_ends[3];
    // extract segment offsets
    for (u32 i = 0; i < 3; i++)
    {
      segment_data_offsets[i] = data_ptr_offset + header->segment_info[i].data;
      segment_link_offsets[i] = header->segment_info[i].relocs + 0x50;
      assert(header->segment_info[i].magic == 1);
    }
    // assert that the data region is filled
    for (u32 i = 0; i < 2; i++)
    {
      assert(align16(segment_data_offsets[i] + header->segment_info[i].size) == segment_data_offsets[i + 1]);
    }
    assert(align16(segment_data_offsets[2] + header->segment_info[2].size) == data.size());
    // loop over segments in reverse order
    for (u32 segment_id = 3; segment_id-- > 0;)
    {
      // is this right?
      if (header->segment_info[segment_id].size == 0)
      {
        continue;
      }
      u32 segment_size = header->segment_info[segment_id].size;
      lof.m_stats.v3_code_bytes += segment_size; // TODO: what is this?
      u32 base_ptr = segment_data_offsets[segment_id];
      u32 data_ptr = base_ptr - 4;
      u32 link_ptr = segment_link_offsets[segment_id];
      assert((data_ptr % 4) == 0);
      assert((segment_size % 4) == 0);
      // add asm words by segment
      const u32* code_start = (const u32*)(&data[data_ptr + 4]);
      const u32* code_end = ((const u32*)(&data[data_ptr + segment_size])) + 1;
      for (u32* ptr = (u32*)code_start; ptr < code_end; ptr++)
      {
        lof.add_word_to_segment(*ptr, segment_id);
      }
      // we have pointers
      u8 fixing = 0;
      if (data[link_ptr])
      {
        while (1)
        {
          while (1)
          {
            if (!fixing)
            {
              // seeking
              data_ptr += 4 * data[link_ptr];
              lof.m_stats.v3_pointer_seeks++;
            }
            else
            {
              // fixing.
              for (u32 i = 0; i < data[link_ptr]; i++)
              {
                lof.m_stats.v3_pointers++;
                u32 old_code = *(const u32*)&data[data_ptr];
                if ((old_code >> 24) == 0)
                {
                  lof.m_stats.v3_word_pointers++;
                  if (lof.pointer_link_word(segment_id, data_ptr - base_ptr, segment_id, old_code) == 0)
                  {
                    assert(0);
                  }
                }
                else
                {
                  lof.m_stats.v3_split_pointers++;
                  u32 dest_seg = (old_code >> 8) & 0xF;
                  u32 lo_hi_offset = (old_code >> 12) & 0xF;
                  assert(lo_hi_offset > 0);
                  assert(dest_seg < 3);
                  u32 offset_upper = old_code & 0xFF;
                  u32 low_code = *(const u32*)(&data[data_ptr + 4 * lo_hi_offset]);
                  u32 offset = low_code & 0xFFFF;
                  if (offset_upper)
                  {
                    offset += (offset_upper << 16);
                  }
                  lof.pointer_link_split_word(segment_id, data_ptr - base_ptr, data_ptr + 4 * lo_hi_offset - base_ptr, dest_seg, offset);
                }
                data_ptr += 4;
              }
            }
            if (data[link_ptr] != 0xFF)
            {
              break;
            }
            link_ptr++;
            if (data[link_ptr] == 0)
            {
              link_ptr++;
              fixing = !fixing;
            }
          }
          link_ptr++;
          fixing = !fixing;
          if (data[link_ptr] == 0)
          {
            break;
          }
        }
      }
      link_ptr++;
      if (data[link_ptr])
      {
        u32 sub_link_ptr = link_ptr;
        while (1)
        {
          u32 reloc = data[sub_link_ptr];
          u32 next_link_ptr = sub_link_ptr + 1;
          link_ptr = next_link_ptr;
          if ((reloc & 0x80) == 0)
          {
            link_ptr = sub_link_ptr + 3;
            const i8* sname = (const i8*)(&data[link_ptr]);
            link_ptr += (u32)strlen(sname) + 1;
            // TODO: segment data offsets
            if (sname == "_empty_")
            {
              link_ptr = c_symlink2(lof, data, segment_data_offsets[segment_id], link_ptr, symbol_link_kind::EMPTY_LIST, sname, segment_id, dts);
            }
            else
            {
              link_ptr = c_symlink2(lof, data, segment_data_offsets[segment_id], link_ptr, symbol_link_kind::SYMBOL, sname, segment_id, dts);
            }
          }
          else if ((reloc & 0x3f) == 0x3f)
          {
            assert(0); // TODO: does this ever get hit?
          }
          else
          {
            link_ptr += 2; // ghidra misses some aliasing here and would have you think this is +1!
            const i8* sname = (const i8*)(&data[link_ptr]);
            link_ptr += (u32)strlen(sname) + 1;
            link_ptr = c_symlink2(lof, data, segment_data_offsets[segment_id], link_ptr, symbol_link_kind::TYPE, sname, segment_id, dts);
          }
          sub_link_ptr = link_ptr;
          if (data[sub_link_ptr] == 0)
          {
            break;
          }
        }
      }
      segment_link_ends[segment_id] = link_ptr;
    }
    assert(segment_link_offsets[0] == 128);
    if (header->segment_info[0].size)
    {
      assert(segment_link_ends[0] + 1 == segment_link_offsets[1]);
    }
    else
    {
      assert(segment_link_offsets[0] + 2 == segment_link_offsets[1]);
    }
    if (header->segment_info[1].size)
    {
      assert(segment_link_ends[1] + 1 == segment_link_offsets[2]);
    }
    else
    {
      assert(segment_link_offsets[1] + 2 == segment_link_offsets[2]);
    }
    assert(align16(segment_link_ends[2] + 2) == segment_data_offsets[0]);
    return 1;
  }

  u8 to_linked_object_file(const std::vector<u8>& data, const std::string& name, linked_object_file& lof, decompiler_type_system& dts)
  {
    const link_header_common* header = (const link_header_common*)&data[0];
    if (header->version == 5)
    {
      return link_v5(lof, data, name, dts);
    }
    else
    {
      return 0;
    }
  }
}