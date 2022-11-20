#include "dimensional.h"
#include <stdio.h>
#include <iostream>

int main() {
    Value<double, -6, 1, {METER}, 1, {SECOND}> speed(5.0);
    DIM_S(double) delta_time(0.01);
    DIM_S(double) onesec(1);
    DIM_N(double) force(9.81 * 2);

    auto x = speed * delta_time;

    auto y = x * x *x * speed * speed;

    std::cout << speed << std::endl;

    std::wcout << force << std::endl;

    std::wcout << speed << std::endl;

    std::wcout << y << std::endl;

    std::wcout << (x / onesec) << std::endl;

    return 0;
}