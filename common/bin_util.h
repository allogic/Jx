#ifndef BIN_UTILS_H
#define BIN_UTILS_H

template<typename T>
T align4(T in)
{
  return (in + 3) & (~T(3));
}

template<typename T>
T align8(T in)
{
  return (in + 7) & (~T(7));
}

template<typename T>
T align16(T in)
{
  return (in + 15) & (~T(15));
}

template<typename T>
T align64(T in)
{
  return (in + 63) & (~T(63));
}

#endif