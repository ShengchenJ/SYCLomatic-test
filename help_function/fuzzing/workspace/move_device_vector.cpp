#include <oneapi/dpl/execution>
#include <oneapi/dpl/algorithm>
#include <sycl.hpp>
#include <dpct/dpct.hpp>
#include <dpct/dpl_utils.hpp>

bool verify(dpct::device_vector<int> &D, int N, int V) {
  if (D.size() != N)
    return false;
  for (int i = 0; i < N; ++i)
    if (D[i] != V)
      return false;
  return true;
}

int getIntValue(int index, const uint8_t *D) {
  return (int)D[index * 4] << 24 | (int)D[index * 4 + 1] << 16 | (int)D[index * 4 + 2] << 8 | (int)D[index * 4 + 3];
}
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {

  int N = getIntValue(0, Data);
  int V = getIntValue(1, Data);
  // Construct D1 from move constructor.
  dpct::device_vector<int> D1(std::move(dpct::device_vector<int>(N, V)));
  if (!verify(D1, N, V)) {
    return 1;
  }
  // Move assign to D2.
  dpct::device_vector<int> D2;
  D2 = std::move(dpct::device_vector<int>(N, V));
  if (!verify(D2, N, V)) {
    return 1;
  }
  return 0;
}
