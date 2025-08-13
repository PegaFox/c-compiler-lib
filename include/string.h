#include <stddef.h>

void *memset(void *dest, int ch, size_t count)
{
  for (char* pos = (char*)dest; count > 0; --count)
  {
    *pos = ch;

    ++pos;
  }

  return dest;
}

