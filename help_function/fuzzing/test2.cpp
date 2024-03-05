#include <iostream>
using namespace std;

int main() {
    const uint8_t D[] = {0x12, 0x34, 0x56, 0x78};
    int i1 = (int)D[0] << 24 | (int)D[1] << 16 | (int)D[2] << 8 | (int)D[3];
    cout << "i1 = " << i1 << endl;
    return 0;
}

