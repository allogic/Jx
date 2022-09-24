#ifndef BINARY_READER_H
#define BINARY_READER_H

#include <common/types.h>

#include <cassert>

struct binary_reader
{
  u8* m_buffer = nullptr;
  u32 m_size = 0;
  u32 m_seek = 0;

  binary_reader() = default;
  binary_reader(u8* buffer, u32 size)
    : m_buffer{ buffer }
    , m_size{ size }
    , m_seek{ 0 }
  {

  }

  template<typename T>
  T read()
  {
    assert(m_seek + sizeof(T) <= m_size);
    T& v = *(T*)(m_buffer + m_seek);
    m_seek += sizeof(T);
    return v;
  }

  void fwd(u32 amount)
  {
    m_seek += amount;
    assert(m_seek <= m_size);
  }

  u32 bytes_left()
  {
    return m_size - m_seek;
  }

  u8* here()
  {
    return m_buffer + m_seek;
  }

  u32 get_seek()
  {
    return m_seek;
  }
};

#endif