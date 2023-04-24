// ====------ memcpy3D_2.cpp---------- -*- C++ -* ----===////
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//
// ===----------------------------------------------------------------------===//
#include <sycl/sycl.hpp>
#include <dpct/dpct.hpp>
#include <malloc.h>
#include <stdio.h>

void check(float *h_data, float *h_ref, size_t width, size_t height,
           size_t depth) {
  for (int i = 0; i < width * height * depth; i++) {
    float diff = fabs(h_data[i] - h_ref[i]);
    if (diff > 1.e-6) {
      printf("Verification failed!");
      printf("h_data[%d]=%f, h_ref[%d]=%f, diff=%f\n", i, h_data[i], i,
             h_ref[i], diff);
      exit(-1);
    }
  }
}

void test_1(void) {
  size_t width = 6;
  size_t height = 8;
  size_t depth = 10;
  float *h_data;
  float *h_ref;
  //test_feature:byte_t
  dpct::byte_t a = 'a';
  printf("%c\n", a);
  if(sizeof(dpct::byte_t) != 1) {
    printf("Verification failed!");
    exit(-1);
  }

  dpct::pitched_data d_data;
  sycl::range<3> extent = sycl::range<3>(sizeof(float) * 1, 1, 1);
  dpct::pitched_data cpyParm_from_data_ct1, cpyParm_to_data_ct1;
  sycl::id<3> cpyParm_from_pos_ct1(0, 0, 0), cpyParm_to_pos_ct1(0, 0, 0);
  sycl::range<3> cpyParm_size_ct1(0, 0, 0);
  dpct::memcpy_direction cpyParm_direction_ct1;

  h_data = (float *)malloc(sizeof(float) * width * height * depth);
  for (int i = 0; i < width * height * depth; i++)
    h_data[i] = (float)i;

  h_ref = (float *)malloc(sizeof(float) * width * height * depth);
  for (int i = 0; i < width * height * depth; i++)
    h_ref[i] = (float)i;

  // alloc memory.
  extent = sycl::range<3>(sizeof(float) * width, height, depth);
  //test_feature:dpct_malloc
  d_data = (dpct::pitched_data)dpct::dpct_malloc(extent);

  // copy to Device.
  cpyParm_from_data_ct1 =
      dpct::pitched_data((void *)h_data, sizeof(float) * width, width, height);

  //test_feature:get_data_ptr
  //test_feature:get_pitch
  //test_feature:get_x
  //test_feature:get_y
  if(cpyParm_from_data_ct1.get_data_ptr() != h_data || 
  cpyParm_from_data_ct1.get_pitch() != sizeof(float) * width ||
  cpyParm_from_data_ct1.get_x() != width ||
  cpyParm_from_data_ct1.get_y() != height) {
    printf("Verification failed!");
    exit(-1);
  }
  //test_feature:set_data_ptr
  //test_feature:set_pitch
  //test_feature:set_x
  //test_feature:set_y
  cpyParm_from_data_ct1.set_data_ptr((void *)h_data);
  cpyParm_from_data_ct1.set_pitch(sizeof(float) * width);
  cpyParm_from_data_ct1.set_x(width);
  cpyParm_from_data_ct1.set_y(height);

  //test_feature:get_data_ptr
  //test_feature:get_pitch
  //test_feature:get_x
  //test_feature:get_y
  if(cpyParm_from_data_ct1.get_data_ptr() != h_data || 
  cpyParm_from_data_ct1.get_pitch() != sizeof(float) * width ||
  cpyParm_from_data_ct1.get_x() != width ||
  cpyParm_from_data_ct1.get_y() != height) {
    printf("Verification failed!");
    exit(-1);
  }

  cpyParm_to_data_ct1 = d_data;
  cpyParm_size_ct1 = extent;
  cpyParm_direction_ct1 = dpct::host_to_device;
  //test_feature:async_dpct_memcpy 
  dpct::async_dpct_memcpy(cpyParm_to_data_ct1, cpyParm_to_pos_ct1,
                    cpyParm_from_data_ct1, cpyParm_from_pos_ct1,
                    cpyParm_size_ct1, cpyParm_direction_ct1);
  dpct::get_default_queue().wait_and_throw();
  // copy back to host.
  cpyParm_from_data_ct1 = d_data;
  cpyParm_to_data_ct1 =
      dpct::pitched_data((void *)h_data, sizeof(float) * width, width, height);
  cpyParm_size_ct1 = extent;
  cpyParm_direction_ct1 = dpct::device_to_host;
  //test_feature:async_dpct_memcpy 
  dpct::async_dpct_memcpy(cpyParm_to_data_ct1, cpyParm_to_pos_ct1,
                    cpyParm_from_data_ct1, cpyParm_from_pos_ct1,
                    cpyParm_size_ct1, cpyParm_direction_ct1);
  dpct::get_default_queue().wait_and_throw();
  check(h_data, h_ref, width, height, depth);
  // memset device data.
  //test_feature:async_dpct_memset 
  dpct::async_dpct_memset(d_data, 0x1, extent);
  dpct::get_default_queue().wait_and_throw();
  // copy back to host
  cpyParm_from_data_ct1 = d_data;
  cpyParm_to_data_ct1 =
      dpct::pitched_data((void *)h_data, sizeof(float) * width, width, height);
  cpyParm_size_ct1 = extent;
  cpyParm_direction_ct1 = dpct::device_to_host;

  //test_feature:async_dpct_memcpy 
  dpct::async_dpct_memcpy(cpyParm_to_data_ct1, cpyParm_to_pos_ct1,
                    cpyParm_from_data_ct1, cpyParm_from_pos_ct1,
                    cpyParm_size_ct1, cpyParm_direction_ct1);
  dpct::get_default_queue().wait_and_throw();
  // memset reference data.
  memset(h_ref, 0x1, width * height * depth * sizeof(float));
  check(h_data, h_ref, width, height, depth);

  free(h_data);
  free(h_ref);
  sycl::free(d_data.get_data_ptr(), dpct::get_default_context());

  printf("Test1 passed!\n");
}

void test_2(void) {
  size_t width = 4;
  size_t height = 4;
  size_t depth = 5;
  float *h_data;

  dpct::pitched_data d_data;
  sycl::range<3> extent = sycl::range<3>(sizeof(float) * 1, 1, 1);
  dpct::pitched_data cpyParm_from_data_ct1, cpyParm_to_data_ct1;
  sycl::id<3> cpyParm_from_pos_ct1(0, 0, 0), cpyParm_to_pos_ct1(0, 0, 0);
  sycl::range<3> cpyParm_size_ct1(0, 0, 0);
  dpct::memcpy_direction cpyParm_direction_ct1;

  h_data = (float *)malloc(sizeof(float) * width * height * depth);
  /*
    0.000000        1.000000        2.000000        3.000000
    4.000000        5.000000        6.000000        7.000000
    8.000000        9.000000        10.000000       11.000000
    12.000000       13.000000       14.000000       15.000000

    16.000000       17.000000       18.000000       19.000000
    20.000000       21.000000       22.000000       23.000000
    24.000000       25.000000       26.000000       27.000000
    28.000000       29.000000       30.000000       31.000000

    32.000000       33.000000       34.000000       35.000000
    36.000000       37.000000       38.000000       39.000000
    40.000000       41.000000       42.000000       43.000000
    44.000000       45.000000       46.000000       47.000000

    48.000000       49.000000       50.000000       51.000000
    52.000000       53.000000       54.000000       55.000000
    56.000000       57.000000       58.000000       59.000000
    60.000000       61.000000       62.000000       63.000000

    64.000000       65.000000       66.000000       67.000000
    68.000000       69.000000       70.000000       71.000000
    72.000000       73.000000       74.000000       75.000000
    76.000000       77.000000       78.000000       79.000000
  */
  for (int i = 0; i < width * height * depth; i++)
    h_data[i] = (float)i;

  /*
    5.000000        6.000000
    9.000000        10.000000

    21.000000       22.000000
    25.000000       26.000000

    37.000000       38.000000
    41.000000       42.000000
  */
  float Ref[12] = {5, 6, 9, 10, 21, 22, 25, 26, 37, 38, 41, 42};

  size_t out_width = 2;
  size_t out_height = 2;
  size_t out_depth = 3;

  // alloc memory.
  extent = sycl::range<3>(sizeof(float) * width, height, depth);
  //test_feature:dpct_malloc
  d_data = (dpct::pitched_data)dpct::dpct_malloc(extent);

  // copy to Device.
  cpyParm_from_data_ct1 =
      dpct::pitched_data((void *)h_data, sizeof(float) * width, width, height);
  cpyParm_to_data_ct1 = d_data;
  cpyParm_size_ct1 = extent;
  cpyParm_direction_ct1 = dpct::host_to_device;
  //test_feature:async_dpct_memcpy 
  dpct::async_dpct_memcpy(cpyParm_to_data_ct1, cpyParm_to_pos_ct1,
                    cpyParm_from_data_ct1, cpyParm_from_pos_ct1,
                    cpyParm_size_ct1, cpyParm_direction_ct1);
  dpct::get_default_queue().wait_and_throw();
  cpyParm_from_pos_ct1 = {1*sizeof(float), 1, 0}; // set offset on x/y/z.
  cpyParm_size_ct1 =     {out_width *sizeof(float), out_height , out_depth};

  for (int i = 0; i < out_width * out_height * out_depth; i++)
    h_data[i] = -1;
  // copy back to host.
  cpyParm_from_data_ct1 = d_data;
  cpyParm_to_data_ct1 =
  dpct::pitched_data((void *)h_data, sizeof(float) * out_width, out_width, out_height);
  cpyParm_direction_ct1 = dpct::device_to_host;
  //test_feature:async_dpct_memcpy 
  dpct::async_dpct_memcpy(cpyParm_to_data_ct1, cpyParm_to_pos_ct1,
                    cpyParm_from_data_ct1, cpyParm_from_pos_ct1,
                    cpyParm_size_ct1, cpyParm_direction_ct1);
  dpct::get_default_queue().wait_and_throw();
  // Copy back to host data.
  check(h_data, Ref, out_width, out_height, out_depth);
  free(h_data);
  sycl::free(d_data.get_data_ptr(), dpct::get_default_context());
  printf("Test2 passed!\n");
}

void test_3(void) {
  size_t width = 4;
  size_t height = 4;
  size_t depth = 5;
  float *h_data;

  dpct::pitched_data d_data;
  sycl::range<3> extent = sycl::range<3>(sizeof(float) * 1, 1, 1);
  dpct::pitched_data cpyParm_from_data_ct1, cpyParm_to_data_ct1;
  sycl::id<3> cpyParm_from_pos_ct1(0, 0, 0), cpyParm_to_pos_ct1(0, 0, 0);
  sycl::range<3> cpyParm_size_ct1(0, 0, 0);
  dpct::memcpy_direction cpyParm_direction_ct1;

  h_data = (float *)malloc(sizeof(float) * width * height * depth);
  /*
    0.000000        1.000000        2.000000        3.000000
    4.000000        5.000000        6.000000        7.000000
    8.000000        9.000000        10.000000       11.000000
    12.000000       13.000000       14.000000       15.000000

    16.000000       17.000000       18.000000       19.000000
    20.000000       21.000000       22.000000       23.000000
    24.000000       25.000000       26.000000       27.000000
    28.000000       29.000000       30.000000       31.000000

    32.000000       33.000000       34.000000       35.000000
    36.000000       37.000000       38.000000       39.000000
    40.000000       41.000000       42.000000       43.000000
    44.000000       45.000000       46.000000       47.000000

    48.000000       49.000000       50.000000       51.000000
    52.000000       53.000000       54.000000       55.000000
    56.000000       57.000000       58.000000       59.000000
    60.000000       61.000000       62.000000       63.000000

    64.000000       65.000000       66.000000       67.000000
    68.000000       69.000000       70.000000       71.000000
    72.000000       73.000000       74.000000       75.000000
    76.000000       77.000000       78.000000       79.000000
  */
  for (int i = 0; i < width * height * depth; i++)
    h_data[i] = (float)i;

  /*
    21.000000       22.000000
    25.000000       26.000000

    37.000000       38.000000
    41.000000       42.000000

    53.000000       54.000000
    57.000000       58.000000
  */
  float Ref[12] = {21, 22, 25, 26, 37, 38, 41, 42, 53, 54, 57, 58};

  size_t out_width = 2;
  size_t out_height = 2;
  size_t out_depth = 3;

  // alloc memory.
  extent = sycl::range<3>(sizeof(float) * width, height, depth);
  //test_feature:dpct_malloc
  d_data = (dpct::pitched_data)dpct::dpct_malloc(extent);

  // copy to Device.
  cpyParm_from_data_ct1 =
      dpct::pitched_data((void *)h_data, sizeof(float) * width, width, height);
  cpyParm_to_data_ct1 = d_data;
  cpyParm_size_ct1 = extent;
  cpyParm_direction_ct1 = dpct::host_to_device;
  //test_feature:async_dpct_memcpy 
  dpct::async_dpct_memcpy(cpyParm_to_data_ct1, cpyParm_to_pos_ct1,
                    cpyParm_from_data_ct1, cpyParm_from_pos_ct1,
                    cpyParm_size_ct1, cpyParm_direction_ct1);
  dpct::get_default_queue().wait_and_throw();
  cpyParm_from_pos_ct1 = {1*sizeof(float), 1, 1}; // set offset on x/y/z.
  cpyParm_size_ct1 =     {out_width *sizeof(float), out_height , out_depth};

  for (int i = 0; i < out_width * out_height * out_depth; i++)
    h_data[i] = -1;
  // copy back to host.
  cpyParm_from_data_ct1 = d_data;
  cpyParm_to_data_ct1 =
  dpct::pitched_data((void *)h_data, sizeof(float) * out_width, out_width, out_height);
  cpyParm_direction_ct1 = dpct::device_to_host;
  //test_feature:async_dpct_memcpy 
  dpct::async_dpct_memcpy(cpyParm_to_data_ct1, cpyParm_to_pos_ct1,
                    cpyParm_from_data_ct1, cpyParm_from_pos_ct1,
                    cpyParm_size_ct1, cpyParm_direction_ct1);
  dpct::get_default_queue().wait_and_throw();
  // Copy back to host data.
  check(h_data, Ref, out_width, out_height, out_depth);
  free(h_data);
  sycl::free(d_data.get_data_ptr(), dpct::get_default_context());
  printf("Test3 passed!\n");
}

void test_1(sycl::queue &q) {
  size_t width = 6;
  size_t height = 8;
  size_t depth = 10;
  float *h_data;
  float *h_ref;

  dpct::pitched_data d_data;
  sycl::range<3> extent = sycl::range<3>(sizeof(float) * 1, 1, 1);
  dpct::pitched_data cpyParm_from_data_ct1, cpyParm_to_data_ct1;
  sycl::id<3> cpyParm_from_pos_ct1(0, 0, 0), cpyParm_to_pos_ct1(0, 0, 0);
  sycl::range<3> cpyParm_size_ct1(0, 0, 0);
  dpct::memcpy_direction cpyParm_direction_ct1;

  h_data = (float *)malloc(sizeof(float) * width * height * depth);
  for (int i = 0; i < width * height * depth; i++)
    h_data[i] = (float)i;

  h_ref = (float *)malloc(sizeof(float) * width * height * depth);
  for (int i = 0; i < width * height * depth; i++)
    h_ref[i] = (float)i;

  // alloc memory.
  extent = sycl::range<3>(sizeof(float) * width, height, depth);
  //test_feature:dpct_malloc
  d_data = (dpct::pitched_data)dpct::dpct_malloc(extent, q);

  // copy to Device.
  cpyParm_from_data_ct1 =
      dpct::pitched_data((void *)h_data, sizeof(float) * width, width, height);
  cpyParm_to_data_ct1 = d_data;
  cpyParm_size_ct1 = extent;
  cpyParm_direction_ct1 = dpct::host_to_device;
  //test_feature:async_dpct_memcpy 
  dpct::async_dpct_memcpy(cpyParm_to_data_ct1, cpyParm_to_pos_ct1,
                    cpyParm_from_data_ct1, cpyParm_from_pos_ct1,
                    cpyParm_size_ct1, cpyParm_direction_ct1, q);
  q.wait_and_throw();
  // copy back to host.
  cpyParm_from_data_ct1 = d_data;
  cpyParm_to_data_ct1 =
      dpct::pitched_data((void *)h_data, sizeof(float) * width, width, height);
  cpyParm_size_ct1 = extent;
  cpyParm_direction_ct1 = dpct::device_to_host;
  //test_feature:async_dpct_memcpy 
  dpct::async_dpct_memcpy(cpyParm_to_data_ct1, cpyParm_to_pos_ct1,
                    cpyParm_from_data_ct1, cpyParm_from_pos_ct1,
                    cpyParm_size_ct1, cpyParm_direction_ct1, q);

  q.wait_and_throw();

  check(h_data, h_ref, width, height, depth);
  // memset device data.
  //test_feature:async_dpct_memset 
  dpct::async_dpct_memset(d_data, 0x1, extent, q);
  q.wait_and_throw();
  // copy back to host
  cpyParm_from_data_ct1 = d_data;
  cpyParm_to_data_ct1 =
      dpct::pitched_data((void *)h_data, sizeof(float) * width, width, height);
  cpyParm_size_ct1 = extent;
  cpyParm_direction_ct1 = dpct::device_to_host;
  //test_feature:async_dpct_memcpy 
  dpct::async_dpct_memcpy(cpyParm_to_data_ct1, cpyParm_to_pos_ct1,
                    cpyParm_from_data_ct1, cpyParm_from_pos_ct1,
                    cpyParm_size_ct1, cpyParm_direction_ct1, q);

  q.wait_and_throw();
  // memset reference data.
  memset(h_ref, 0x1, width * height * depth * sizeof(float));
  check(h_data, h_ref, width, height, depth);

  free(h_data);
  free(h_ref);
  dpct::dpct_free(d_data.get_data_ptr(), q);

  printf("Test1 passed!\n");
}

void test_2(sycl::queue &q) {
  size_t width = 4;
  size_t height = 4;
  size_t depth = 5;
  float *h_data;

  dpct::pitched_data d_data;
  sycl::range<3> extent = sycl::range<3>(sizeof(float) * 1, 1, 1);
  dpct::pitched_data cpyParm_from_data_ct1, cpyParm_to_data_ct1;
  sycl::id<3> cpyParm_from_pos_ct1(0, 0, 0), cpyParm_to_pos_ct1(0, 0, 0);
  sycl::range<3> cpyParm_size_ct1(0, 0, 0);
  dpct::memcpy_direction cpyParm_direction_ct1;

  h_data = (float *)malloc(sizeof(float) * width * height * depth);
  /*
    0.000000        1.000000        2.000000        3.000000
    4.000000        5.000000        6.000000        7.000000
    8.000000        9.000000        10.000000       11.000000
    12.000000       13.000000       14.000000       15.000000

    16.000000       17.000000       18.000000       19.000000
    20.000000       21.000000       22.000000       23.000000
    24.000000       25.000000       26.000000       27.000000
    28.000000       29.000000       30.000000       31.000000

    32.000000       33.000000       34.000000       35.000000
    36.000000       37.000000       38.000000       39.000000
    40.000000       41.000000       42.000000       43.000000
    44.000000       45.000000       46.000000       47.000000

    48.000000       49.000000       50.000000       51.000000
    52.000000       53.000000       54.000000       55.000000
    56.000000       57.000000       58.000000       59.000000
    60.000000       61.000000       62.000000       63.000000

    64.000000       65.000000       66.000000       67.000000
    68.000000       69.000000       70.000000       71.000000
    72.000000       73.000000       74.000000       75.000000
    76.000000       77.000000       78.000000       79.000000
  */
  for (int i = 0; i < width * height * depth; i++)
    h_data[i] = (float)i;

  /*
    5.000000        6.000000
    9.000000        10.000000

    21.000000       22.000000
    25.000000       26.000000

    37.000000       38.000000
    41.000000       42.000000
  */
  float Ref[12] = {5, 6, 9, 10, 21, 22, 25, 26, 37, 38, 41, 42};

  size_t out_width = 2;
  size_t out_height = 2;
  size_t out_depth = 3;

  // alloc memory.
  extent = sycl::range<3>(sizeof(float) * width, height, depth);
  //test_feature:dpct_malloc
  d_data = (dpct::pitched_data)dpct::dpct_malloc(extent, q);

  // copy to Device.
  cpyParm_from_data_ct1 =
      dpct::pitched_data((void *)h_data, sizeof(float) * width, width, height);
  cpyParm_to_data_ct1 = d_data;
  cpyParm_size_ct1 = extent;
  cpyParm_direction_ct1 = dpct::host_to_device;
  //test_feature:async_dpct_memcpy 
  dpct::async_dpct_memcpy(cpyParm_to_data_ct1, cpyParm_to_pos_ct1,
                    cpyParm_from_data_ct1, cpyParm_from_pos_ct1,
                    cpyParm_size_ct1, cpyParm_direction_ct1, q);
  q.wait_and_throw();
  cpyParm_from_pos_ct1 = {1*sizeof(float), 1, 0}; // set offset on x/y/z.
  cpyParm_size_ct1 =     {out_width *sizeof(float), out_height , out_depth};

  for (int i = 0; i < out_width * out_height * out_depth; i++)
    h_data[i] = -1;
  // copy back to host.
  cpyParm_from_data_ct1 = d_data;
  cpyParm_to_data_ct1 =
  dpct::pitched_data((void *)h_data, sizeof(float) * out_width, out_width, out_height);
  cpyParm_direction_ct1 = dpct::device_to_host;
  //test_feature:async_dpct_memcpy 
  dpct::async_dpct_memcpy(cpyParm_to_data_ct1, cpyParm_to_pos_ct1,
                    cpyParm_from_data_ct1, cpyParm_from_pos_ct1,
                    cpyParm_size_ct1, cpyParm_direction_ct1, q);
  q.wait_and_throw();
  // Copy back to host data.
  check(h_data, Ref, out_width, out_height, out_depth);
  free(h_data);
  dpct::dpct_free(d_data.get_data_ptr(), q);
  printf("Test2 passed!\n");
}

void test_3(sycl::queue &q) {
  size_t width = 4;
  size_t height = 4;
  size_t depth = 5;
  float *h_data;

  dpct::pitched_data d_data;
  sycl::range<3> extent = sycl::range<3>(sizeof(float) * 1, 1, 1);
  dpct::pitched_data cpyParm_from_data_ct1, cpyParm_to_data_ct1;
  sycl::id<3> cpyParm_from_pos_ct1(0, 0, 0), cpyParm_to_pos_ct1(0, 0, 0);
  sycl::range<3> cpyParm_size_ct1(0, 0, 0);
  dpct::memcpy_direction cpyParm_direction_ct1;

  h_data = (float *)malloc(sizeof(float) * width * height * depth);
  /*
    0.000000        1.000000        2.000000        3.000000
    4.000000        5.000000        6.000000        7.000000
    8.000000        9.000000        10.000000       11.000000
    12.000000       13.000000       14.000000       15.000000

    16.000000       17.000000       18.000000       19.000000
    20.000000       21.000000       22.000000       23.000000
    24.000000       25.000000       26.000000       27.000000
    28.000000       29.000000       30.000000       31.000000

    32.000000       33.000000       34.000000       35.000000
    36.000000       37.000000       38.000000       39.000000
    40.000000       41.000000       42.000000       43.000000
    44.000000       45.000000       46.000000       47.000000

    48.000000       49.000000       50.000000       51.000000
    52.000000       53.000000       54.000000       55.000000
    56.000000       57.000000       58.000000       59.000000
    60.000000       61.000000       62.000000       63.000000

    64.000000       65.000000       66.000000       67.000000
    68.000000       69.000000       70.000000       71.000000
    72.000000       73.000000       74.000000       75.000000
    76.000000       77.000000       78.000000       79.000000
  */
  for (int i = 0; i < width * height * depth; i++)
    h_data[i] = (float)i;

  /*
    21.000000       22.000000
    25.000000       26.000000

    37.000000       38.000000
    41.000000       42.000000

    53.000000       54.000000
    57.000000       58.000000
  */
  float Ref[12] = {21, 22, 25, 26, 37, 38, 41, 42, 53, 54, 57, 58};

  size_t out_width = 2;
  size_t out_height = 2;
  size_t out_depth = 3;

  // alloc memory.
  extent = sycl::range<3>(sizeof(float) * width, height, depth);
  //test_feature:dpct_malloc
  d_data = (dpct::pitched_data)dpct::dpct_malloc(extent, q);

  // copy to Device.
  cpyParm_from_data_ct1 =
      dpct::pitched_data((void *)h_data, sizeof(float) * width, width, height);
  cpyParm_to_data_ct1 = d_data;
  cpyParm_size_ct1 = extent;
  cpyParm_direction_ct1 = dpct::host_to_device;
  //test_feature:async_dpct_memcpy 
  dpct::async_dpct_memcpy(cpyParm_to_data_ct1, cpyParm_to_pos_ct1,
                    cpyParm_from_data_ct1, cpyParm_from_pos_ct1,
                    cpyParm_size_ct1, cpyParm_direction_ct1, q);
  q.wait_and_throw();
  cpyParm_from_pos_ct1 = {1*sizeof(float), 1, 1}; // set offset on x/y/z.
  cpyParm_size_ct1 =     {out_width *sizeof(float), out_height , out_depth};

  for (int i = 0; i < out_width * out_height * out_depth; i++)
    h_data[i] = -1;
  // copy back to host.
  cpyParm_from_data_ct1 = d_data;
  cpyParm_to_data_ct1 =
  dpct::pitched_data((void *)h_data, sizeof(float) * out_width, out_width, out_height);
  cpyParm_direction_ct1 = dpct::device_to_host;
  //test_feature:async_dpct_memcpy 
  dpct::async_dpct_memcpy(cpyParm_to_data_ct1, cpyParm_to_pos_ct1,
                    cpyParm_from_data_ct1, cpyParm_from_pos_ct1,
                    cpyParm_size_ct1, cpyParm_direction_ct1, q);
  q.wait_and_throw();
  // Copy back to host data.
  check(h_data, Ref, out_width, out_height, out_depth);
  free(h_data);
  dpct::dpct_free(d_data.get_data_ptr(), q);
  printf("Test3 passed!\n");
}

int main() {
  test_1();
  test_2();
  test_3();

  sycl::queue q;
  test_1(q);
  test_2(q);
  test_3(q);
  
  return 0;
}