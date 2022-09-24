#include <common/file_util.h>
#include <common/binary_reader.h>
#include <common/debug_util.h>

#include <vendor/lzokay/lzokay.hpp>

#include <string>
#include <fstream>
#include <filesystem>

u8 file_util::collect_file_paths_by_extension(const std::string& root_path, const std::string& extension, std::vector<std::string>& out)
{
  std::filesystem::directory_iterator it = std::filesystem::directory_iterator{ root_path };
  std::filesystem::directory_iterator end;
  for (; it != end; it++)
  {
    if (std::filesystem::is_directory(it->path()) == 0)
    {
      if (it->path().extension().string() == extension)
      {
        out.emplace_back(it->path().string());
      }
    }
  }
  return out.size() > 0;
}

u8 file_util::read_binary_file(const std::string& file_name, std::vector<u8>& out)
{
  std::ifstream stream;
  stream.open(file_name, std::ios::binary);
  if (stream.is_open())
  {
    stream.seekg(0, std::ios::end);
    u64 size = stream.tellg();
    stream.seekg(0, std::ios::beg);
    out.resize(size);
    stream.read((i8*)out.data(), size);
    u64 bytes_read = stream.gcount();
    stream.close();
    return size > 0;
  }
  return 0;
}

u8 file_util::decompress_dgo(const std::vector<u8>& in, std::vector<u8>& out)
{
  binary_reader reader = binary_reader{ (u8*)in.data(), (u32)in.size() };
  u32 jak_header = reader.read<u32>();
  if (jak_header != 0x426C5A6F)
  {
    return 0;
  }
  u32 decomp_size = reader.read<u32>();
  out.resize(decomp_size);
  u32 output_offset = 0;
  while (1)
  {
    u32 chunk_size = 0;
    while (chunk_size == 0)
    {
      chunk_size = reader.read<u32>();
    }
    if (chunk_size < 0x8000)
    {
      u64 bytes_written = 0;
      lzokay::EResult ok = lzokay::decompress(reader.here(), chunk_size, out.data() + output_offset, out.size() - output_offset, bytes_written);
      assert(ok == lzokay::EResult::Success);
      reader.fwd(chunk_size);
      output_offset += (u32)bytes_written;
    }
    else
    {
      std::memcpy(out.data() + output_offset, reader.here(), 0x8000);
      reader.fwd(0x8000);
      output_offset += 0x8000;
    }
    if (output_offset >= decomp_size)
    {
      break;
    }
    while (reader.get_seek() % 4 != 0)
    {
      reader.fwd(1);
    }
  }
  return out.size() > 0;
}