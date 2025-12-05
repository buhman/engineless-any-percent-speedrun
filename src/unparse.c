#include <math.h>

#include "unparse.h"

static inline int digits_base10_64(uint64_t n)
{
  if (n >= 10000000000000000000ull) return 20;
  if (n >= 1000000000000000000ull) return 19;
  if (n >= 100000000000000000ull) return 18;
  if (n >= 10000000000000000ull) return 17;
  if (n >= 1000000000000000ull) return 16;
  if (n >= 100000000000000ull) return 15;
  if (n >= 10000000000000ull) return 14;
  if (n >= 1000000000000ull) return 13;
  if (n >= 100000000000ull) return 12;
  if (n >= 10000000000ull) return 11;
  if (n >= 1000000000ull) return 10;
  if (n >= 100000000ull) return 9;
  if (n >= 10000000ull) return 8;
  if (n >= 1000000ull) return 7;
  if (n >= 100000ull) return 6;
  if (n >= 10000ull) return 5;
  if (n >= 1000ull) return 4;
  if (n >= 100ull) return 3;
  if (n >= 10ull) return 2;
  return 1;
}

static inline int min(int a, int b)
{
  if (a < b)
    return a;
  else
    return b;
}

int unparse_base10_64(char * s, int64_t n, int len)
{
  int digits = 0;
  if (n < 0) {
    digits += 1;
    n = -n;
  }

  digits += digits_base10_64(n);
  len = min(digits, len);
  int ret = len;

  while (len > 0) {
    const uint32_t digit = n % 10;
    n = n / 10;
    s[--len] = digit + 48;
  }

  return ret;
}

int unparse_double(double num, int whole_pad, int fract_pad, char * dst)
{
  bool negative = num < 0;
  double n = negative ? -num : num;
  double whole = floor(n);
  double fract = n - whole;
  int64_t wholei = whole;
  int64_t fracti = fract * pow(10.0, fract_pad);

  char wbuf[20];
  int wret = unparse_base10_64(wbuf, wholei, 20);
  char fbuf[20];
  int fret = unparse_base10_64(fbuf, fracti, 20);
  int ix = 0;
  for (int i = 0; i < whole_pad - wret; i++) {
    dst[ix++] = ' ';
  }
  dst[ix++] = negative ? '-' : ' ';
  for (int i = 0; i < wret; i++) {
    dst[ix++] = wbuf[i];
  }
  dst[ix++] = '.';
  for (int i = 0; i < fract_pad - fret; i++) {
    dst[ix++] = '0';
  }
  for (int i = 0; i < fret; i++) {
    dst[ix++] = fbuf[i];
  }

  return ix;
}
