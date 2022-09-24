#ifndef LINK_TYPES_H
#define LINK_TYPES_H

#include <common/types.h>

struct dgo_header
{
  u32 size;
  i8 name[60];
};

struct object_header
{
  u32 size;
  i8 name[60];
};

#endif