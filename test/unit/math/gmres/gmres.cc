import math.gmres;
import utils.selftest;

import std;

static const utils::selftest::Test gmres{
    "/math/gmres/gmres", [] {
        const std::vector<double> A{ 1.0f, 0.0f, 0.0f, 2.0f };
        const std::vector<double> b{ 1.0f, 2.0f };
        std::println("{} @ x = {}", A, b);
        std::vector<double> x(2);
        math::gmres::solve(2, A.data(), b.data(), x.data());
        std::println("x = {}", x);
    }
};
