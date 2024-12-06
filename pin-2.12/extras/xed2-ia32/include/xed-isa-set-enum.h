/*BEGIN_LEGAL 
Intel Open Source License 

Copyright (c) 2002-2012 Intel Corporation. All rights reserved.
 
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.  Redistributions
in binary form must reproduce the above copyright notice, this list of
conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.  Neither the name of
the Intel Corporation nor the names of its contributors may be used to
endorse or promote products derived from this software without
specific prior written permission.
 
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE INTEL OR
ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
END_LEGAL */
/// @file xed-isa-set-enum.h

// This file was automatically generated.
// Do not edit this file.

#if !defined(_XED_ISA_SET_ENUM_H_)
# define _XED_ISA_SET_ENUM_H_
#include "xed-common-hdrs.h"
typedef enum {
  XED_ISA_SET_INVALID,
  XED_ISA_SET_3DNOW,
  XED_ISA_SET_AES,
  XED_ISA_SET_AMD,
  XED_ISA_SET_AVX,
  XED_ISA_SET_AVX2,
  XED_ISA_SET_AVX2GATHER,
  XED_ISA_SET_BMI1,
  XED_ISA_SET_BMI2,
  XED_ISA_SET_CMPXCHG16B,
  XED_ISA_SET_F16C,
  XED_ISA_SET_FMA,
  XED_ISA_SET_I186,
  XED_ISA_SET_I286PROTECTED,
  XED_ISA_SET_I286REAL,
  XED_ISA_SET_I386,
  XED_ISA_SET_I386REAL,
  XED_ISA_SET_I486,
  XED_ISA_SET_I486REAL,
  XED_ISA_SET_I86,
  XED_ISA_SET_INVPCID,
  XED_ISA_SET_LAHF,
  XED_ISA_SET_LIN,
  XED_ISA_SET_LONGMODE,
  XED_ISA_SET_LZCNT,
  XED_ISA_SET_MMX,
  XED_ISA_SET_MOVBE,
  XED_ISA_SET_MXCSR,
  XED_ISA_SET_PCLMULQDQ,
  XED_ISA_SET_PENTIUM,
  XED_ISA_SET_PENTIUM2,
  XED_ISA_SET_PENTIUM4,
  XED_ISA_SET_PENTIUMMMX,
  XED_ISA_SET_PENTIUMREAL,
  XED_ISA_SET_POPCNT,
  XED_ISA_SET_PPRO,
  XED_ISA_SET_PREFETCH_NOP,
  XED_ISA_SET_RDPMC,
  XED_ISA_SET_RDRAND,
  XED_ISA_SET_RDTSCP,
  XED_ISA_SET_RDWRFSGS,
  XED_ISA_SET_RTM,
  XED_ISA_SET_SMX,
  XED_ISA_SET_SSE,
  XED_ISA_SET_SSE2,
  XED_ISA_SET_SSE3,
  XED_ISA_SET_SSE4,
  XED_ISA_SET_SSE42,
  XED_ISA_SET_SSE4A,
  XED_ISA_SET_SSE5,
  XED_ISA_SET_SSSE3,
  XED_ISA_SET_SVM,
  XED_ISA_SET_VMFUNC,
  XED_ISA_SET_VTX,
  XED_ISA_SET_X87,
  XED_ISA_SET_XSAVE,
  XED_ISA_SET_XSAVEOPT,
  XED_ISA_SET_LAST
} xed_isa_set_enum_t;

/// This converts strings to #xed_isa_set_enum_t types.
/// @param s A C-string.
/// @return #xed_isa_set_enum_t
/// @ingroup ENUM
XED_DLL_EXPORT xed_isa_set_enum_t str2xed_isa_set_enum_t(const char* s);
/// This converts strings to #xed_isa_set_enum_t types.
/// @param p An enumeration element of type xed_isa_set_enum_t.
/// @return string
/// @ingroup ENUM
XED_DLL_EXPORT const char* xed_isa_set_enum_t2str(const xed_isa_set_enum_t p);

/// Returns the last element of the enumeration
/// @return xed_isa_set_enum_t The last element of the enumeration.
/// @ingroup ENUM
XED_DLL_EXPORT xed_isa_set_enum_t xed_isa_set_enum_t_last(void);
#endif
