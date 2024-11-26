import math.core;
import math.gmres;
import utils.assert;
import utils.random;
import utils.selftest;

import std;

namespace assert = utils::assert;
using math::allclose;

static const utils::selftest::Test gmres_2x2_diag{
    "/math/gmres/gmres_2x2_diag", [] {
        const std::vector<double> A{ 1.0, 0.0, 0.0, 2.0 };
        const std::vector<double> b{ 1.0, 2.0 };
        const auto x = math::gmres::solve(A, b);
        assert::always(math::allclose(x, std::vector{ 1.0, 1.0 }));
    }
}; // <-- gmres_2x2_diag

static const utils::selftest::Test gmres_2x2{
    "/math/gmres/gmres_2x2", [] {
        const std::vector<double> A{ 1.0, 8.0, 4.0, 2.0 };
        const std::vector<double> b{ 13.0, 7.0 };
        const auto x = math::gmres::solve(A, b);
        assert::always(math::allclose(x, std::vector{ 1.0, 1.5 }));
    }
}; // <-- gmres_2x2

static const utils::selftest::Test gmres20x20{
    "/math/gmres/gmres_20x20", [] {
        namespace rng = utils::random::generators;

        static constexpr std::size_t n = 20;
        const auto A = std::views::take(rng::normal<double>(20.0, 20.0), n * n)
                     | std::ranges::to<std::vector<double>>();
        const auto x0 = std::views::take(rng::normal<double>(), n)
                      | std::ranges::to<std::vector<double>>();
        const auto b = math::matvec(A, x0);

        const auto x = math::gmres::solve(A, b, 100);
        assert::always(math::allclose(x, x0));
    }
}; // <-- gmres_20x20
