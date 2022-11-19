#include "dimensional.h"
#include <stdio.h>
#include <iostream>

int main() {
    MPS(double) speed(5.0);
    S(double) delta_time(0.01);

    M(double) x = speed * delta_time;

    std::cout << x.value << std::endl;

    return 0;
}