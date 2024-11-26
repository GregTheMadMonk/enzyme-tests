import math.core;
import utils.assert;
import utils.random;
import utils.selftest;

import std;

namespace assert = utils::assert;
using math::allclose;

static const utils::selftest::Test matvec_2x2{
    "/math/core/matvec_2x2", [] {
        const std::vector<double> A{ 1.0, 2.0, 4.0, 3.0 };
        const std::vector<double> x{ 1.0, 2.0 };
        const auto b = math::matvec(A, x);
        assert::always(math::allclose(b, std::vector{ 5.0, 10.0 }));
    }
}; // <-- matvec_2x2
