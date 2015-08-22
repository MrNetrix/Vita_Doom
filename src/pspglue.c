
#include <stdio.h>
#include <stdlib.h>


typedef unsigned char      uint8_t;
typedef signed   char      sint8_t;
typedef unsigned short     uint16_t;
typedef signed   short     sint16_t;
//typedef unsigned int       uint32_t;
typedef signed   int       sint32_t;

 
/* 
   Converts a string to a signed int. 
 */
sint32_t _atoi(uint8_t *s)
{
#define ISNUM(c) ((c) >= '0' && (c) <= '9')

  register uint32_t i = 0;
  register uint32_t sign = 0;
  register uint8_t *p = s;
  
  /* Conversion starts at the first numeric character or sign. */
  while(*p && !ISNUM(*p) && *p != '-') p++;
  
  /* 
     If we got a sign, set a flag.
     This will negate the value before return.
   */
  if(*p == '-')
    {
      sign++;
      p++;
    }
  
  /* Don't care when 'u' overflows (Bug?) */
  while(ISNUM(*p))
    {
      i *= 10;
      i += *p++ - '0';
    }
  
  /* Return according to sign */
  if(sign)
    return - i;
  else
    return i;
  
#undef ISNUM  
}

int strcpy_s(char* dest, size_t size, const char* src)
{
    return strncpy(dest, src, size);
}