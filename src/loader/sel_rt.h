/*
 * Copyright 2009 The Native Client Authors.  All rights reserved.
 * Use of this source code is governed by a BSD-style license that can
 * be found in the LICENSE file.
 */
/*
 * Copyright (c) 2012, LiteStack, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * NaCl Secure Runtime
 */
#ifndef SEL_RT_H__
#define SEL_RT_H__ 1

#include <inttypes.h>
#include "src/main/tools.h"

typedef uint64_t  nacl_reg_t;  /* general purpose register type */

struct ThreadContext {
  nacl_reg_t  rax,  rbx,  rcx,  rdx,  rbp,  rsi,  rdi,  rsp;
  /*          0x0,  0x8, 0x10, 0x18, 0x20, 0x28, 0x30, 0x38 */
  nacl_reg_t  r8,     r9,  r10,  r11,  r12,  r13,  r14,  r15;
  /*          0x40, 0x48, 0x50, 0x58, 0x60, 0x68, 0x70, 0x78 */
  nacl_reg_t  prog_ctr;  /* rip */
  /*          0x80 */
  nacl_reg_t  sysret;
  /*          0x88 */
};

uintptr_t GetThreadCtxSp(struct ThreadContext *th_ctx);

void SetThreadCtxSp(struct ThreadContext *th_ctx, uintptr_t sp);

/*
 * Argument passing convention in AMD64, from
 *
 *   http://www.x86-64.org/documentation/abi.pdf
 *
 * for system call parameters, are as follows.  All syscall arguments
 * are of INTEGER type (section 3.2.3).  They are assigned, from
 * left-to-right, to the registers
 *
 *   rdi, rsi, rdx, rcx, r8, r9
 *
 * and any additional arguments are passed on the stack, pushed onto
 * the stack in right-to-left order.  Note that this means that the
 * syscall with the maximum number of arguments, mmap, passes all its
 * arguments in registers.
 *
 * Argument passing convention for Microsoft, from wikipedia, is
 * different.  The first four arguments go in
 *
 *   rcx, rdx, r8, r9
 *
 * respectively, with the caller responsible for allocating 32 bytes
 * of "shadow space" for the first four arguments, an additional
 * arguments are on the stack.  Presumably this is to make stdargs
 * easier to implement: the callee can always write those four
 * registers to 8(%rsp), ..., 24(%rsp) resp (%rsp value assumed to be
 * at fn entry/start of prolog, before push %rbp), and then use the
 * effective address of 8(%rsp) as a pointer to an in-memory argument
 * list.  However, because this is always done, presumably called code
 * might treat this space as if it's part of the red zone, and it
 * would be an error to not allocate this stack space, even if the
 * called function is declared to take fewer than 4 arguments.
 *
 * Caller/callee saved
 *
 * - AMD64:
 *   - caller saved: rax, rcx, rdx, rdi, rsi, r8, r9, r10, r11
 *   - callee saved: rbx, rbp, r12, r13, r14, r15
 *
 * - Microsoft:
 *   - caller saved: rax, rcx, rdx, r8, r9, r10, r11
 *   - callee saved: rbx, rbp, rdi, rsi, r12, r13, r14, r15
 *
 * A conservative approach might be to follow microsoft and save more
 * registers, but the presence of shadow space will make assembly code
 * incompatible anyway, assembly code that calls must allocate shadow
 * space, but then in-memory arguments will be in the wrong location
 * wrt %rsp.
 */

nacl_reg_t GetStackPtr(void);

#endif /* SEL_RT_H__ */
