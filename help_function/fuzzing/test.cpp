#include<sycl/sycl.hpp>


template<typename T>
T getPart(const uint8_t *Data, size_t begin, size_t end) {
    T result = 0;
    size_t size  = end - begin + 1;
    for (int i = begin; i < begin + size; i++) {
        result |= (int)Data[i] << (8 * i);
    }
    return result;
}

template <typename T> 
sycl::range<T> getSYCLRange(const uint8_t *Data, size_t Size) {
   
    switch (T)
    {
    case 1:
        auto value = getPart<int>(Data, 0, Size-1);
        return sycl::range<1>((int)value%VAL_MAX);
        break;
    case 2:
        auto value1 = getPart<int>(Data, 0, Size/2);
        auto value2 = getPart<int>(Data, Size/2 + 1, Size-1);
        return sycl::range<2>((int)value1%VAL_MAX, (int)value2%VAL_MAX);
    case 3:
        auto value1 = getPart<int>(Data, 0, Size/3);
        auto value2 = getPart<int>(Data, Size/3 + 1, Size/3 *2);
        auto value2 = getPart<int>(Data, Size/3 *2 + 1, Size-1);
        return sycl::range<3>((int)value1%VAL_MAX, (int)value2%VAL_MAX);
    default:
        auto value = getPart<int>(Data, 0, Size-1);
        return sycl::range<1>((int)value%VAL_MAX);
        break;
    }
}

void test() {
    getSYCLRange<3>(nullptr, 100);
}