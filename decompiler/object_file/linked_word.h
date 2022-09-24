#ifndef LINKED_WORD_H
#define LINKED_WORD_H

#include <common/types.h>

#include <string>

namespace decompiler
{
  struct linked_word
  {
    enum kind : u8
    {
      PLAIN_DATA, // just plain data
      PTR,        // pointer to a location
      HI_PTR,     // lower 16-bits of this data are the upper 16 bits of a pointer
      LO_PTR,     // lower 16-bits of this data are the lower 16 bits of a pointer
      SYM_PTR,    // this is a pointer to a symbol
      EMPTY_PTR,  // this is a pointer to the empty list
      SYM_OFFSET, // this is an offset of a symbol in the symbol table
      TYPE_PTR,   // this is a pointer to a type
    };

    linked_word(u32 data);

    kind get_kind() const;
    u8 holds_string() const;
    void set_to_pointer(kind kind, u32 label_id);
    u32 label_id() const;
    void set_to_empty_ptr();
    void set_to_symbol(kind kind, const std::string& name);
    std::string symbol_name() const;

    u32 m_data = 0;
    u64 m_data_ptr = 0;
    kind m_kind = PLAIN_DATA;
  };
}

#endif