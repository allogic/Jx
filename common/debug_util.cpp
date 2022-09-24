#include <common/debug_util.h>

#include <cstdlib>

void debug_util::dump_hex(const std::vector<u8>& in, u32 offset, u32 amount, u8 mod)
{
  for (u32 i = offset; i < (offset + amount); i += mod)
  {
    std::printf("0x%08X | ", i);
    for (u8 j = 0; j < mod; j++)
    {
      std::printf("%02X ", in[i + j]);
    }
    std::printf("| ");
    for (u8 j = 0; j < mod; j++)
    {
      if (in[i + j] >= 32 && in[i + j] < 127)
      {
        std::printf("%c", in[i + j]);
      }
      else
      {
        std::printf(".");
      }
    }
    std::printf("\n");
  }
}