#include <cassert>

#include <object_file/linked_object_file.h>

namespace decompiler
{
  void linked_object_file::set_segment_count(u32 count)
  {
    assert(m_segments == 0);
    m_segments = count;
    m_words_by_seg.resize(count);
    m_label_per_seg_by_offset.resize(count);
    m_offset_of_data_zone_by_seg.resize(count);
    m_functions_by_seg.resize(count);
  }

  void linked_object_file::add_word_to_segment(u32 word, u32 segment)
  {
    m_words_by_seg[segment].emplace_back(word);
  }

  u8 linked_object_file::pointer_link_word(u32 source_segment, u32 source_offset, u32 dest_segment, u32 dest_offset)
  {
    assert((source_offset % 4) == 0);
    linked_word& word = m_words_by_seg[source_segment][source_offset / 4];
    assert(word.get_kind() == linked_word::PLAIN_DATA);
    if (dest_offset / 4 > m_words_by_seg[dest_segment].size())
    {
      return 0;
    }
    assert(dest_offset / 4 <= m_words_by_seg[dest_segment].size());
    word.set_to_pointer(linked_word::PTR, get_label_id_for(dest_segment, dest_offset));
    return 1;
  }

  void linked_object_file::pointer_link_split_word(u32 source_segment, u32 source_hi_offset, u32 source_lo_offset, u32 dest_segment, u32 dest_offset)
  {
    assert((source_hi_offset % 4) == 0);
    assert((source_lo_offset % 4) == 0);
    linked_word& hi_word = m_words_by_seg[source_segment][source_hi_offset / 4];
    linked_word& lo_word = m_words_by_seg[source_segment][source_lo_offset / 4];
    assert(hi_word.get_kind() == linked_word::PLAIN_DATA);
    assert(lo_word.get_kind() == linked_word::PLAIN_DATA);
    hi_word.set_to_pointer(linked_word::HI_PTR, get_label_id_for(dest_segment, dest_offset));
    lo_word.set_to_pointer(linked_word::LO_PTR, hi_word.label_id());
  }

  u32 linked_object_file::get_label_id_for(u32 segment, u32 offset)
  {
    auto it = m_label_per_seg_by_offset[segment].find(offset);
    if (it == m_label_per_seg_by_offset[segment].end())
    {
      // create new label
      u32 id = (u32)m_labels.size();
      decompiler_label label;
      label.target_segment = segment;
      label.offset = offset;
      label.name = "L" + std::to_string(id);
      m_label_per_seg_by_offset[segment][offset] = id;
      m_labels.emplace_back(label);
      return id;
    }
    else
    {
      // use existing label
      decompiler_label& label = m_labels[it->second];
      assert(label.target_segment == segment);
      assert(label.offset == offset);
      return it->second;
    }
  }

  void linked_object_file::symbol_link_word(u32 source_segment, u32 source_offset, const i8* name, linked_word::kind kind)
  {
    assert((source_offset % 4) == 0);
    linked_word& word = m_words_by_seg[source_segment][source_offset / 4];
    assert(word.get_kind() == linked_word::PLAIN_DATA);
    if (kind == linked_word::EMPTY_PTR)
    {
      word.set_to_empty_ptr();
    }
    else
    {
      word.set_to_symbol(kind, name);
    }
  }

  void linked_object_file::symbol_link_offset(u32 source_segment, u32 source_offset, const i8* name)
  {
    assert((source_offset % 4) == 0);
    linked_word& word = m_words_by_seg[source_segment][source_offset / 4];
    assert(word.get_kind() == linked_word::PLAIN_DATA);
    word.set_to_symbol(linked_word::SYM_OFFSET, name);
  }

  void linked_object_file::find_code()
  {
    if (m_segments == 1)
    {
      auto& segment = m_words_by_seg[0];
      for (linked_word& word : segment)
      {
        if (word.get_kind() == linked_word::TYPE_PTR)
        {
          assert(word.symbol_name() != "function");
        }
      }
      m_offset_of_data_zone_by_seg[0] = 0;
      m_stats.data_bytes = (u32)m_words_by_seg[0].size() * 4;
      m_stats.code_bytes = 0;
    }
    else if (m_segments == 3)
    {
      // V3 object files will have all the functions, then all the static data. So to find the
      // divider, we look for the last "function" tag, then find the last jr $ra instruction after
      // that (plus one for delay slot) and assume that after that is data. Additionally, we check to
      // make sure that there are no "function" type tags in the data section, although this is
      // redundant.
      for (u32 i = 0; i < m_segments; i++)
      {
        // try to find the last reference to "function"
        u8 found_function = 0;
        i64 function_loc = -1;
        for (i64 j = (i64)m_words_by_seg[i].size(); j-- > 0;)
        {
          linked_word& word = m_words_by_seg[i][j];
          if (word.get_kind() == linked_word::TYPE_PTR && word.symbol_name() == "function")
          {
            function_loc = j;
            found_function = 1;
            break;
          }
        }
        // look forward until we find "jr ra"
        if (found_function)
        {
          u32 jr_ra = 0x3E00008;
          u8 found_jr_ra = 0;
          i64 jr_ra_loc = -1;
          for (i64 j = function_loc; j < (i64)m_words_by_seg[i].size(); j++)
          {
            linked_word& word = m_words_by_seg[i][j];
            if (word.get_kind() == linked_word::PLAIN_DATA && word.m_data == jr_ra)
            {
              found_jr_ra = 1;
              jr_ra_loc = j;
              break;
            }
          }
          assert(found_jr_ra == 1);
          assert(jr_ra_loc + 1 < (i64)m_words_by_seg[i].size());
          m_offset_of_data_zone_by_seg[i] = (u32)(jr_ra_loc + 2);
        }
        else
        {
          m_offset_of_data_zone_by_seg[i] = 0;
        }
        // add label for debug purposes
        if (m_offset_of_data_zone_by_seg[i] < m_words_by_seg[i].size())
        {
          u32 data_label_id = get_label_id_for(i, 4 * (m_offset_of_data_zone_by_seg[i]));
          m_labels[data_label_id].name = "L-data-start";
        }
        // verify there are no functions after the data section starts
        for (u32 j = m_offset_of_data_zone_by_seg[i]; j < m_words_by_seg[i].size(); j++)
        {
          linked_word& word = m_words_by_seg[i][j];
          if (word.get_kind() == linked_word::TYPE_PTR && word.symbol_name() == "function")
          {
            assert(0);
          }
        }
        // sizes
        m_stats.data_bytes += 4 * ((u32)m_words_by_seg[i].size() - m_offset_of_data_zone_by_seg[i]) * 4;
        m_stats.code_bytes += 4 * m_offset_of_data_zone_by_seg[i];
      }
    }
    else
    {
      assert(0);
    }
  }

  void linked_object_file::find_functions()
  {
    if (m_segments == 1)
    {
      assert(m_offset_of_data_zone_by_seg[0] == 0);
    }
    else
    {
      // we assume functions don't have any data in between them, so we use the "function" type tag to
      // mark the end of the previous function and the start of the next. This means that some
      // functions will have a few 0x0 words after then for padding (GOAL functions are aligned), but
      // this is something that the disassembler should handle.
      for (u32 segments_id = 0; segments_id < m_segments; segments_id++)
      {
        u32 function_end = m_offset_of_data_zone_by_seg[segments_id];
        while (function_end > 0)
        {
          // back up until we find function type tag
          u32 function_tag_loc = function_end;
          u8 found_function_tag_loc = 0;
          while (function_tag_loc-- > 0)
          {
            linked_word& word = m_words_by_seg[segments_id][function_tag_loc];
            if (word.get_kind() == linked_word::TYPE_PTR && word.symbol_name() == "function")
            {
              found_function_tag_loc = 1;
              break;
            }
          }
          // mark this as a function, and try again from the current function start
          assert(found_function_tag_loc == 1);
          m_stats.function_count++;
          m_functions_by_seg[segments_id].emplace_back(function_tag_loc, function_end);
          function_end = function_tag_loc;
        }
        std::reverse(m_functions_by_seg[segments_id].begin(), m_functions_by_seg[segments_id].end());
      }
    }
  }

  void linked_object_file::disassemble_functions()
  {
    for (u32 segment_id = 0; segment_id < m_segments; segment_id++)
    {
      for (function& function : m_functions_by_seg[segment_id])
      {
        for (u32 word = function.m_start_word; word < function.m_end_word; word++)
        {
          decode_instruction(m_words_by_seg[segment_id][word], *this, segment_id, word);
          function.m_instructions.emplace_back();
          if (function.m_instructions.back().is_valid())
          {
            m_stats.decoded_ops++;
          }
          else
          {
            assert(0);
          }
        }
      }
    }
  }

  void linked_object_file::stats::add(const stats& other)
  {
    total_code_bytes += other.total_code_bytes;
    total_v2_code_bytes += other.total_v2_code_bytes;
    total_v2_pointers += other.total_v2_pointers;
    total_v2_pointer_seeks += other.total_v2_pointer_seeks;
    total_v2_link_bytes += other.total_v2_link_bytes;
    total_v2_symbol_links += other.total_v2_symbol_links;
    total_v2_symbol_count += other.total_v2_symbol_count;
    v3_code_bytes += other.v3_code_bytes;
    v3_pointers += other.v3_pointers;
    v3_pointer_seeks += other.v3_pointer_seeks;
    v3_link_bytes += other.v3_link_bytes;
    v3_word_pointers += other.v3_word_pointers;
    v3_split_pointers += other.v3_split_pointers;
    v3_symbol_count += other.v3_symbol_count;
    v3_symbol_link_offset += other.v3_symbol_link_offset;
    v3_symbol_link_word += other.v3_symbol_link_word;
    data_bytes += other.data_bytes;
    code_bytes += other.code_bytes;
    function_count += other.function_count;
    decoded_ops += other.decoded_ops;
    n_fp_reg_use += other.n_fp_reg_use;
    n_fp_reg_use_resolved += other.n_fp_reg_use_resolved;
  }
}