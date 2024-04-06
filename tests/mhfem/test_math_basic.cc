#include "test_math_common.hh"

static const st::test test_dot{
    "math::dot()", [] {
        std::println("v @ u = {} @ {} = {}", v, u, math::dot(v, u));
        math::assert(math::dot(v, u) == 10);

        decltype(v) dv(v.size());
        decltype(u) du(u.size());

        __enzyme_autodiff(
            math::dot<std::vector<float>, std::vector<double>>,
            enzyme_dup, &v, &dv,
            enzyme_dup, &u, &du
        );

        std::println("d/dv = {}", dv);
        math::assert(stdr::equal(dv, u));
        std::println("d/du = {}", du);
        math::assert(stdr::equal(du, v));
    }
};

static const st::test test_matvec{
    "math::matvec", [] {
        std::println("M @ v = {} @ {} = {}", M, v, *math::matvec(M, v));
        math::assert(stdr::equal(*math::matvec(M, v), std::vector{ 2, 3, 1 }));

        vec dM(M.size());
        vec dv(v.size());
        const std::vector<std::pair<vec, vec>> etalons{
            { { 0, 1, 0 }, { 1, 2, 3, 0, 0, 0, 0, 0, 0 } },
            { { 0, 0, 1 }, { 0, 0, 0, 1, 2, 3, 0, 0, 0 } },
            { { 1, 0, 0 }, { 0, 0, 0, 0, 0, 0, 1, 2, 3 } },
        };
        for (auto i = 0uz; i < stdr::size(v); ++i) {
            std::println("Component #{}", i);
            stdr::fill(dM, 0);
            stdr::fill(dv, 0);

            __enzyme_autodiff(
                math::matvec_element<decltype(M), decltype(v)>,
                enzyme_dup, &M, &dM,
                enzyme_dup, &v, &dv,
                enzyme_const, i
            );
            const auto& etalon = etalons[i];
            std::println("d/dM = {}", dM);
            math::assert(stdr::equal(std::get<1>(etalon), dM));
            std::println("d/dv = {}", dv);
            math::assert(stdr::equal(std::get<0>(etalon), dv));
        }
    }
};

static const st::test test_vecmat{
    "math::vecmat", [] {
        std::println("v.T @ M = {}.T @ {} = {}.T", v, M, *math::vecmat(v, M));
        math::assert(stdr::equal(*math::vecmat(v, M), std::vector{ 3, 1, 2 }));

        vec dM(M.size());
        vec dv(v.size());
        const std::vector<std::pair<vec, vec>> etalons{
            { { 0, 0, 1 }, { 1, 0, 0, 2, 0, 0, 3, 0, 0 } },
            { { 1, 0, 0 }, { 0, 1, 0, 0, 2, 0, 0, 3, 0 } },
            { { 0, 1, 0 }, { 0, 0, 1, 0, 0, 2, 0, 0, 3 } },
        };
        for (auto i = 0uz; i < stdr::size(v); ++i) {
            std::println("Component #{}", i);
            stdr::fill(dM, 0);
            stdr::fill(dv, 0);

            __enzyme_autodiff(
                math::vecmat_element<decltype(M), decltype(v)>,
                enzyme_dup, &v, &dv,
                enzyme_dup, &M, &dM,
                enzyme_const, i
            );
            const auto& etalon = etalons[i];
            std::println("d/dM = {}", dM);
            math::assert(stdr::equal(std::get<1>(etalon), dM));
            std::println("d/dv = {}", dv);
            math::assert(stdr::equal(std::get<0>(etalon), dv));
        }
    }
};
