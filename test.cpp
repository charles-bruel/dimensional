#include "dimensional.h"
#include <stdio.h>
#include <iostream>

int main() {
    MPS(double) speed(5.0);
    S(double) delta_time(0.01);
    S(double) onesec(1);

    M(double) x = speed * delta_time;

    auto y = x * x *x * speed * speed;

    std::cout << speed << std::endl;

    std::wcout << speed << std::endl;

    std::wcout << y << std::endl;

    std::wcout << (x / onesec) << std::endl;

    return 0;
}