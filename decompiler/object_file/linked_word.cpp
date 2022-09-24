#include <cassert>

#include <object_file/linked_word.h>

namespace decompiler
{
  linked_word::linked_word(u32 data)
    : m_data{ data }
  {

  }

  linked_word::kind linked_word::get_kind() const
  {
    return m_kind;
  }

  u8 linked_word::holds_string() const
  {
    return m_kind == kind::SYM_PTR || m_kind == kind::SYM_OFFSET || m_kind == kind::TYPE_PTR;
  }

  void linked_word::set_to_pointer(kind kind, u32 label_id)
  {
    if (holds_string() == 1)
    {
      delete[]((i8*)m_data_ptr);
    }
    m_data_ptr = label_id;
    m_kind = kind;
  }

  u32 linked_word::label_id() const
  {
    assert(m_kind == kind::PTR || m_kind == kind::LO_PTR || m_kind == kind::HI_PTR);
    return (u32)m_data_ptr;
  }

  void linked_word::set_to_empty_ptr()
  {
    if (holds_string() == 1)
    {
      delete[]((i8*)m_data_ptr);
    }
    m_kind = EMPTY_PTR;
  }

  void linked_word::set_to_symbol(kind kind, const std::string& name)
  {
    if (holds_string() == 1)
    {
      delete[]((i8*)m_data_ptr);
    }
    m_kind = kind;
    i8* str = new char[name.size() + 1];
    strcpy(str, name.c_str());
    m_data_ptr = (u64)str;
  }

  std::string linked_word::symbol_name() const
  {
    assert(holds_string() == 1);
    return (const i8*)m_data_ptr;
  }
}