#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

  int unparse_base10_64(char * s, int64_t n, int len);
  int unparse_double(double num, int whole_pad, int fract_pad, char * dst);

#ifdef __cplusplus
}
#endif
