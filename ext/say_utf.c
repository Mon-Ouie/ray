#include "say.h"

/*
  Based on source code found on unicode.org:
    http://www.unicode.org/Public/PROGRAMS/CVTUTF/ConvertUTF.c
    http://www.unicode.org/Public/PROGRAMS/CVTUTF/ConvertUTF.h
*/

#define UNI_REPLACEMENT_CHAR (uint32_t)0x0000FFFD
#define UNI_MAX_BMP (uint32_t)0x0000FFFF
#define UNI_MAX_UTF16 (uint32_t)0x0010FFFF
#define UNI_MAX_UTF32 (uint32_t)0x7FFFFFFF
#define UNI_MAX_LEGAL_UTF32 (uint32_t)0x0010FFFF

#define UNI_SUR_HIGH_START  (uint32_t)0xD800
#define UNI_SUR_HIGH_END    (uint32_t)0xDBFF
#define UNI_SUR_LOW_START   (uint32_t)0xDC00
#define UNI_SUR_LOW_END     (uint32_t)0xDFFF

static const char say_utf8_trailing_bytes[256] = {
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 3,3,3,3,3,3,3,3,4,4,4,4,5,5,5,5
};

static const uint32_t say_utf8_offsets[6] = {
  0x00000000UL, 0x00003080UL, 0x000E2080UL,
  0x03C82080UL, 0xFA082080UL, 0x82082080UL
};

uint32_t say_utf8_to_utf32(const uint8_t *string) {
  uint32_t ret = 0;

  char trailing_bytes = say_utf8_trailing_bytes[(int)(*string)];

  switch (trailing_bytes) { /* notice the fall through */
    case 5: ret += *string++; ret <<= 6;
    case 4: ret += *string++; ret <<= 6;
    case 3: ret += *string++; ret <<= 6;
    case 2: ret += *string++; ret <<= 6;
    case 1: ret += *string++; ret <<= 6;
    case 0: ret += *string++;
  }

  ret -= say_utf8_offsets[(int)trailing_bytes];

  if (ret <= UNI_MAX_LEGAL_UTF32) {
    if (ret >= UNI_SUR_HIGH_START && ret <= UNI_SUR_LOW_END) {
      ret = UNI_REPLACEMENT_CHAR;
    }
  }
  else {
    ret = UNI_REPLACEMENT_CHAR;
  }

  return ret;
}
