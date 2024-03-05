#include <cstdint>
#include <cstring>
#include <iostream>
#include <sycl/sycl.hpp>
template <typename T>
void ConvertData(const uint8_t* data, size_t size, T* value) {
  std::memcpy(value, data, sizeof(T));
}

template <typename T>
T ConvertData(const uint8_t* data) {
  T value;
  ConvertData(data, sizeof(T), &value);
  return value;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  int32_t int_value = ConvertData<int32_t>(data);
  double double_value = ConvertData<double>(data + sizeof(int32_t));
  sycl::float2 float2_value = ConvertData<sycl::float2>(data + sizeof(int32_t) + sizeof(double));
  std::cout << "int_value: " << int_value << ", double_value: " << double_value << std::endl;
  std::cout << "Fuzzer value is " << float2_value.x() << "   " << float2_value.y() << std::endl;
  return 0;
}

