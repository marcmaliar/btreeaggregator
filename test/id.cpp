#include <iostream>
#include "source.h"
#include <bitset>

void testID() {
    for (int i = 0; i < 10; ++i) {
        std::cout << "ID " << std::bitset<32>(nextID()) << std::endl;
    }
}

int main() {
    testID();
    return 0;
}