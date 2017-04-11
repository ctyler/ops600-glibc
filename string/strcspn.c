/* Copyright (C) 1991-2017 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */

#include <string.h>
#include <stdint.h>
#include <libc-pointer-arith.h>

#undef strcspn

#ifndef STRCSPN
# define STRCSPN strcspn
#endif

/* Return the length of the maximum initial segment of S
   which contains no characters from REJECT.  */
size_t STRCSPN (const char *str, const char *reject)
{
  if (__glibc_unlikely (reject[0] == '\0') ||
      __glibc_unlikely (reject[1] == '\0'))
    return __strchrnul (str, reject [0]) - str;

  /* Use multiple small memsets to enable inlining on most targets.  */
  unsigned char table[256];
  unsigned char *p = memset(table, 0, 64);
  memset(p + 64, 0, 64);
  memset(p + 128, 0, 64);
  memset(p + 192, 0, 64);

  unsigned char *s = (unsigned char*)reject;
  unsigned char tmp;
  do
    p[tmp = *s++] = 1;
  while (tmp);

  s = (unsigned char*)str;
  if (p[s[0]]) return 0;
  if (p[s[1]]) return 1;
  if (p[s[2]]) return 2;
  if (p[s[3]]) return 3;
  if (p[s[4]]) return 4;
  if (p[s[5]]) return 5;
  if (p[s[6]]) return 6;
  if (p[s[7]]) return 7;



  unsigned int c0, c1, c2, c3, c4, c5, c6, c7;
  do
  {
    s += 8;
    c0 = p[s[0]];
    c1 = p[s[1]];
    c2 = p[s[2]];
    c3 = p[s[3]];
    c4 = p[s[4]];
    c5 = p[s[5]];
    c6 = p[s[6]];
    c7 = p[s[7]];
  } while ((c0 | c1 | c2 | c3 | c4 | c5 | c6 | c7) == 0);

  size_t count = s - (unsigned char *)str;

  if (c0 | c1 != 0) {
    return count - c0 + 1;
  }
  else if ((c2 | c3) != 0) {
    return count - c2 + 3;
  }
  else if ((c4 | c5) != 0) {
    return count - c4 + 5;
  }
  else {
    return count - c6 + 5;
  }

}
libc_hidden_builtin_def (strcspn)
