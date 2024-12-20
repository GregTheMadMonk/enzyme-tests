#include <fenv.h>

import std;
import utils.selftest;
import utils.cstd;

import cfd;
import math;

int main(int argc, char** argv) {
    if (argc > 2) {
        std::println(stderr, "Usage: {} [filter]", argv[0]);
        return EXIT_FAILURE;
    }

    // feenableexcept(FE_INVALID | FE_OVERFLOW);

    utils::selftest::run_tests(argc == 2 ? argv[1] : "");

    return EXIT_SUCCESS;
}
