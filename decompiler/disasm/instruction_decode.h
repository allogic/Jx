#ifndef INSTRUCTION_DECODE_H
#define INSTRUCTION_DECODE_H

#include <common/types.h>

#include <object_file/linked_word.h>
#include <object_file/linked_object_file.h>

namespace decompiler
{
  instruction decode_instruction(linked_word& word, linked_object_file& lof, u32 segment_id, u32 word_id);
}

#endif