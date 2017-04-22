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

/* Assumptions:
 *
 * ARMv8-a, AArch64
 */

#include <sysdep.h>

#define REP8_01 0x0101010101010101
#define REP8_7f 0x7f7f7f7f7f7f7f7f
#define REP8_80 0x8080808080808080

/*
Comment Added by shiv
r0 through r30 - to refer generally to the registers
x0 through x30 - for 64-bit-wide access (same registers)
w0 through w30 - for 32-bit-wide access (same registers - upper 32 bits are either cleared on load or sign-extended (set to the value of the most significant bit of the loaded value)).
Total number of register used for this function

11 General Purpose 64bit register
2 General Purpose 32bit register w2,w3.
*/
/* Parameters and result.  */
#define src1            x0                              //paramter
#define src2            x1
#define result          x0


/* Internal variables.  */
#define data1           x2
#define data1w          w2
#define data2           x3
#define data2w          w3
#define data1v          v0
#define data2v          v1

#define has_nul         x4
#define diff            x5
#define syndrome        x6
#define tmp1            x7
#define tmp2            x8
#define counter         x9
#define selection       x10
#define tmp_v           v3
#define tmp1_v          v4
#define tmp2_v          v5
#define tmp3_v          v6

        /* Start of performance-critical section  -- one 64B cache line.  */
ENTRY_ALIGN(strcmp, 6)

                                DELOUSE (0)
                                DELOUSE (1)
                                mov     selection,#0
                                eor     tmp1, src1, src2        /*Appling Or operator to src1 and src2 and saving output in tmp1(x7 register)*/
                                tst     tmp1, #15
                                b.ne    L(misaligned15) /*      if two strings are misaligned then go byte by byte method*/
                                /*ands    tmp1, src1, #15*/
								/*b.ne    L(mutual_align)*/

L(loop_aligned):
                                LD1 {v0.16B}, [src1] ,#16
                                LD1 {v1.16B}, [src2] ,#16
                                /*Lets try initialiliziung registers*/
L(start_realigned):
                                cmeq    v4.16B,v0.16B,v1.16B    /*if any of these are*/
                                uminv   b3, v0.16B                      /*finds the lowet unsinged interger*/
                                uminv   b5,v4.16B               /*      if minimum is 0 then found difference else both vectors are same*/
                                umov    w4,v3.16B[0]            /*Loading LSB 64bit of Vector to general */
                                umov    w5,v5.16B[0]
                                cbz     has_nul,L(16_byte_backwords)         /* Lowet Unsigned Integer is zero=nul*/
                                cbnz    diff,L(loop_aligned)    /*  Branch to Lable on non-zero*/
                                b       L(16_byte_backwords)



L(mutual_align):
        /* Sources are mutually aligned, but are not currently at an
           alignment boundary.  Round down the addresses and then mask off
           the bytes that preceed the start point.  */
		ins 	v3.D[0],tmp1
        bic     src1, src1, #15
        bic     src2, src2, #15
        lsl     tmp1, tmp1, #4          /* 16 Bytes beyond alignment -> bits.  */
		ld1 	{v0.16B}, [src1] ,#16
        neg  	v3.16B, V3.16B             /* Bits to alignment.  */
		ld1 	{v1.16B}, [src2] ,#16
        mov     tmp2, #~0
		dup 	V4.2D,tmp2
#ifdef __AARCH64EB__
        /* Big-endian.  Early bytes are at MSB.  */
		ushl 	D4,D4,D3
#else
        /* Little-endian.  Early bytes are at LSB.  */
		rev64 	v4.16B,v4.16B
        ushl 	D4,D4,D3
		rev64	v4.16B,v4.16B
#endif
		orr  	V1.16B, V1.16B,v4.16B
        orr		v2.16B,v2.16B,v4.16B
        b       L(start_realigned)

L(16_byte_backwords):
		sub     src1 , src1 ,#16
		sub     src2 ,  src2,#16

L(misaligned15):
/*need to find from where it is alligned and have it jump to vector methode*/
        ldrb    data1w, [src1], #1 //8 bits
        ldrb    data2w, [src2], #1      //8 bits
        cmp     data1w, #1
        ccmp    data1w, data2w, #0, cs  /* NZCV = 0b0000.  */
        b.eq    L(misaligned15) /*branch to label if equal*/
        sub     result, data1, data2// simply subtract , result = data1 -data2
        ret                                             //retun of  function
END(strcmp)
libc_hidden_builtin_def (strcmp)
