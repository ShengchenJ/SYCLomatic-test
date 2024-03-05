// ====------ util_reverse_bits_test.cpp---------- -*- C++ -* ----===////
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//
// ===----------------------------------------------------------------------===//

#include <sycl/sycl.hpp>
#include <dpct/dpct.hpp>
#include "fuzzing_help.hpp"

//  reverse_bits_test() {
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  unsigned int a = ConvertData<unsigned int>(Data);
  unsigned int b = dpct::reverse_bits(a);
  if (b != 0x80000000)
    exit(-1);

  a = 0x12345678;
  b = dpct::reverse_bits(a);
  if (b != 0x1e6a2c48)
    exit(-1);

  printf("util_reverse_bits_test passed!\n");
}

