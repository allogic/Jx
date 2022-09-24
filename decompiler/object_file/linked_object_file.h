#ifndef LINKED_OBJECT_FILE_H
#define LINKED_OBJECT_FILE_H

#include <common/types.h>

#include <vector>
#include <unordered_map>

#include <object_file/linked_word.h>

#include <function/function.h>

#include <disasm/decompiler_label.h>

namespace decompiler
{
  struct linked_object_file
  {
    linked_object_file() = default;

    void set_segment_count(u32 count);
    void add_word_to_segment(u32 word, u32 segment);
    u8 pointer_link_word(u32 source_segment, u32 source_offset, u32 dest_segment, u32 dest_offset);
    void pointer_link_split_word(u32 source_segment, u32 source_hi_offset, u32 source_lo_offset, u32 dest_segment, u32 dest_offset);
    u32 get_label_id_for(u32 segment, u32 offset);
    void symbol_link_word(u32 source_segment, u32 source_offset, const i8* name, linked_word::kind kind);
    void symbol_link_offset(u32 source_segment, u32 source_offset, const i8* name);
    void find_code();
    void find_functions();
    void disassemble_functions();

    u32 m_segments = 0;

    std::vector<std::vector<linked_word>> m_words_by_seg;
    std::vector<u32> m_offset_of_data_zone_by_seg;
    std::vector<std::vector<function>> m_functions_by_seg;
    std::vector<decompiler_label> m_labels;

    std::vector<std::unordered_map<u32, u32>> m_label_per_seg_by_offset;

    struct stats
    {
      u32 total_code_bytes = 0;
      u32 total_v2_code_bytes = 0;
      u32 total_v2_pointers = 0;
      u32 total_v2_pointer_seeks = 0;
      u32 total_v2_link_bytes = 0;
      u32 total_v2_symbol_links = 0;
      u32 total_v2_symbol_count = 0;

      u32 v3_code_bytes = 0;
      u32 v3_pointers = 0;
      u32 v3_split_pointers = 0;
      u32 v3_word_pointers = 0;
      u32 v3_pointer_seeks = 0;
      u32 v3_link_bytes = 0;

      u32 v3_symbol_count = 0;
      u32 v3_symbol_link_offset = 0;
      u32 v3_symbol_link_word = 0;

      u32 data_bytes = 0;
      u32 code_bytes = 0;

      u32 function_count = 0;
      u32 decoded_ops = 0;

      u32 n_fp_reg_use = 0;
      u32 n_fp_reg_use_resolved = 0;

      void add(const stats& other);
    } m_stats;
  };
}

#endif