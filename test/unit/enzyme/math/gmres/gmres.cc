import math.core;
import math.gmres;
import utils.assert;
import utils.selftest;
import enzyme;

import std;

namespace assert = utils::assert;

/*
 * Test gmres for
 *      /  a   b  \
 * A =  |         |
 *      \  c   d  /
 *
 * and b = ( e f )
 *
 * Accepts [ a, b, c, d, e, f ] as input
 */
inline void test_gmres_2x2_general(std::array<double, 6> statement) {
    const auto [ a, b, c, d, e, f ] = statement;
    using vec = std::vector<double>;
    static constexpr auto mvs = [] (const vec& A, const vec& b) {
        vec x(b.size());
        math::gmres::solve(A, b, x, 100);
        float ret = 0.0f;
        for (auto xe : x) ret += xe;
        return ret;
    }; // <-- mvs

    const vec A{ a, b,
                 c, d };
    const vec B{ e, f };

    vec dA(A.size());
    vec db(B.size());
    std::println("{} {} {} {}", A, dA, B, db);
    __enzyme_autodiff<float>(
        +mvs,
        enzyme_dupnoneed, &A, &dA,
        enzyme_const, &B
    );
    std::println("{} {} {} {}", A, dA, B, db);
    __enzyme_autodiff<float>(
        +mvs,
        enzyme_const, &A,
        enzyme_dupnoneed, &B, &db
    );
    std::println("{} {} {} {}", A, dA, B, db);

    const auto det  = a * d - b * c;
    const auto det2 = det * det;
    const vec Ainv{
        d / det, -b / det,
        -c / det, a / det
    }; // <-- Ainv

    const vec db0{ Ainv[0] + Ainv[2], Ainv[1] + Ainv[3] };
    const vec dA0{
        e * (c - d) * d / det2 + f * ((b - a) * d / det2 + 1.0 / det),
        e * (d - c) * c / det2 + f * ((a - b) * c / det2 - 1.0 / det),
        e * ((d - c) * b / det2 - 1.0 / det) + f * (a - b) * b / det2,
        e * ((c - d) * a / det2 + 1.0 / det) + f * (b - a) * a / det2,
    }; // <-- dA0

    std::println("{} == {}", dA, dA0);
    std::println("{} == {}", db, db0);

    assert::always(math::allclose(dA, dA0));
    assert::always(math::allclose(db, db0));
} // <-- test_gmres_2x2_general

static const utils::selftest::Test gmres_2x2_diag{
    "/enzyme/math/gmres/gmres_2x2_diag", [] {
        test_gmres_2x2_general({ 1.0, 0.0, 0.0, 2.0, 1.0, 2.0 });
    }
}; // <-- gmres_2x2_diag

static const utils::selftest::Test gmres_2x2{
    "/enzyme/math/gmres/gmres_2x2", [] {
        test_gmres_2x2_general({ 1.0, 3.0, -1.0, 2.0, 1.0, 4.0 });
    }
}; // <-- gmres_2x2

static const utils::selftest::Test gmres_2x2_pt2{
    "/enzyme/math/gmres/gmres_2x2_pt2", [] {
        test_gmres_2x2_general({ 1.0, 3.0, -1.0, 2.0, 1.0, 0.0 });
    }
}; // <-- gmres_2x2_pt2
