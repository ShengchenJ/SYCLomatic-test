#include<iostream>
#include<random>

##define VAL_MIN       -1000
#define VAL_MAX        1000
#define VAL_RANGE      (VAL_MAX - VAL_MIN + 1)

void init(int *input, int* fuzzing, size_t size) {
    for (int i = 0; i < size; i++) {
        input[i] = fuzzing[i];
    }
}

std::mt19937 RAND;


// Get the random range size
template <typename T>
class GenRandom {
public:
    GenRandom(const T* choices, const int num):choices(choices), num(num) {}
    const T* getElt() {
        return choices + (RAND()%num);
    }
private:
    T *choices;
    const int num;
};
template<typename T>
T getPart(const uint8_t *Data, size_t begin, size_t end) {
    T result = 0;
    size_t size  = end - begin + 1;
    for (int i = begin; i < begin + size; i++) {
        result |= (int)ptr[i] << (8 * i);
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
    case 2:
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
void initRAND() {
    int seedList[] = {1, 3, 5, 7};
    int rnd= rand()%4;
    std::cout << " Random is " << rnd << std::endl;
    RAND.seed(seedList[rnd]);
}
