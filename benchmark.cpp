#include "dimensional.h"
#include <random>
#include <time.h>

#define TEST_SIZE 1000000000

int main() {
    std::random_device rd;
    std::mt19937 e2(rd());
    std::uniform_real_distribution<> dist(0, 25);

    std::vector<DIM_FORCE(float)> dinp1(TEST_SIZE);
    std::vector<DIM_MASS(float)> dinp2(TEST_SIZE);
    std::vector<DIM_ACC(float)> dresult(TEST_SIZE);

    std::vector<float> rinp1(TEST_SIZE);
    std::vector<float> rinp2(TEST_SIZE);
    std::vector<float> rresult(TEST_SIZE);

    for (int i = 0; i < TEST_SIZE; ++i) {
        float v = dist(e2);
        rinp1[i] = v;
        dinp1[i].value = v;
        v = dist(e2);
        rinp2[i] = v;
        dinp2[i].value = v;
    }

    clock_t tStart = clock();
    for (int i = 0; i < TEST_SIZE; ++i) {
        dresult[i] = dinp1[i] / dinp2[i];
    }
    auto dim_div = (double)(clock() - tStart)/CLOCKS_PER_SEC;

    tStart = clock();
    for (int i = 0; i < TEST_SIZE; ++i) {
        rresult[i] = rinp1[i] / rinp2[i];
    }
    auto raw_div = (double)(clock() - tStart)/CLOCKS_PER_SEC;

    tStart = clock();
    for (int i = 0; i < TEST_SIZE/10000; ++i) {
        std::cout << dresult[i] << std::endl;
    }
    auto dim_cout = (double)(clock() - tStart)/CLOCKS_PER_SEC;

    tStart = clock();
    for (int i = 0; i < TEST_SIZE/10000; ++i) {
        std::cout << rresult[i] << "m/ss" << std::endl;
    }
    auto raw_cout = (double)(clock() - tStart)/CLOCKS_PER_SEC;

    printf("dimensional divide: %.2fs\n", dim_div);
    printf("raw divide: %.2fs\n", raw_div);
    printf("dimensional print: %.2fs\n", dim_cout);
    printf("raw print: %.2fs\n", raw_cout);

    return 0;
}