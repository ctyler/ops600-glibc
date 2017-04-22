/* Copyright (C) 2012-2017 Free Software Foundation, Inc.

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
   License along with the GNU C Library.  If not, see
   <http://www.gnu.org/licenses/>.  */


#include <wchar.h>
#include <sysdep.h>

int 
__wcscmp(const wchar_t *src1, const wchar_t *src2)
{

    asm goto(
        "loadCharacters:\n"
        "ld1 {v0.4s}, [%0], #16\n" //Load 4 characters into vector register from src1
        "ld1 {v1.4s}, [%1], #16\n" //Load 4 characters into vector register from src2
        "cmeq v2.4s, v0.4s, v1.4s\n" //compare the two registers and place results in a third register
        "uminv s2, v2.4s\n" //find lowest value in the result register, 0 if they were unequal otherwise 1
        "umov w20, v2.4s[0]\n" //move the lowest result to a general purpose register
        "cbnz w20, equalCheckNull\n" //branch if they are equal (not 0)

        "compareCharacters:\n" //They are not equal so find the difference
        "umov w21, v0.4s[0]\n" //move first character to general purpose register
        "umov w22, v1.4s[0]\n" 
        "cmp w21, w22\n" //Compare them
        "blt %l3\n" //If greater than or less than branch else continue
        "bgt %l4\n" 
        "umov w21, v0.4s[1]\n" //Repeat for 3 other characters 
        "umov w22, v1.4s[1]\n"
        "cmp w21, w22\n"
        "blt %l3\n"
        "bgt %l4\n"
        "umov w21, v0.4s[2]\n"
        "umov w22, v1.4s[2]\n"
        "cmp w21, w22\n"
        "blt %l3\n"
        "bgt %l4\n"
        "umov w21, v0.4s[3]\n"
        "umov w22, v1.4s[3]\n"
        "cmp w21, w22\n"
        "blt %l3\n"
        "bgt %l4\n" //Difference must be found by this point 

        "equalCheckNull:\n" //registers were equal
        "cmgt v3.4s, v1.4s, #0\n" //check if any are not greater than 0 (null)
        "uminv s3, v3.4s\n" //find lowest value which would be 0 if any were null
        "umov w20, v3.4s[0]\n" //move value to general purpose register
        "cbz w20, %l2\n" //branch if zero to return 0 (equal)
        "b loadCharacters" //If not zero load next 4 characters
        :
        : "r"(src1), "r"(src2)
        : "w20", "w21", "w22", "v0",
          "v1", "v2", "v3", "cc"
        : returnEqual, returnLessThan, returnGreaterThan);

returnEqual:
    return 0;

returnLessThan:
    return -1;

returnGreaterThan:
    return 1;
}
libc_hidden_builtin_def(__wcscmp)
weak_alias(__wcscmp,wcscmp)
