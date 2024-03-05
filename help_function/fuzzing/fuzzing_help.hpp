#include <cstdint>
#include <cstring>
#include <iostream>
#include <sycl/sycl.hpp>


// Param 1: the begin address, Param 2: the size of the malloc.
// std::map<uint8_t, size_t> address_size;
struct address_size {
    uint8_t *begin_adress;
    size_t size;
};

std::vector<address_size> used_address;

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

// Init value,
// Template T is type
// Param 1 Fuzzer input data,
// Param 2 size of the input
template <typename T>
T *malloc_init_dev(const uint8_t *data, size_t input_size, sycl::queue &q_ct1) {
    address_size current_struct;
    uint8_t *cur_address = const_cast<uint8_t *>(data);
    if (used_address.size() != 0) {
        auto val = used_address.back();
        cur_address = val.begin_adress + (int)val.size;
    }
    current_struct.begin_adress = cur_address;
    current_struct.size = input_size;
    used_address.push_back(current_struct);

    T *dev_data = (T *)sycl::malloc_device<T>(input_size, q_ct1);

    q_ct1.memcpy(dev_data, data, input_size).wait();
    return dev_data;
}

