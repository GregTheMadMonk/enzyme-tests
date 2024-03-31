#include "test_math_common.hh"

static const st::test test_dot{
    "stdm::dot()", [] {
        std::println("v @ u = {} @ {} = {}", v, u, stdm::dot(v, u));
        stdm::assert(stdm::dot(v, u) == 10);

        decltype(v) dv(v.size());
        decltype(u) du(u.size());

        __enzyme_autodiff(
            stdm::dot<std::vector<float>, std::vector<double>>,
            enzyme_dup, &v, &dv,
            enzyme_dup, &u, &du
        );

        std::println("d/dv = {}", dv);
        stdm::assert(stdr::equal(dv, u));
        std::println("d/du = {}", du);
        stdm::assert(stdr::equal(du, v));
    }
};

static const st::test test_matvec{
    "stdm::matvec", [] {
        std::println("M @ v = {} @ {} = {}", M, v, *stdm::matvec(M, v));
        stdm::assert(stdr::equal(*stdm::matvec(M, v), std::vector{ 2, 3, 1 }));

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
                stdm::matvec_element<decltype(M), decltype(v)>,
                enzyme_dup, &M, &dM,
                enzyme_dup, &v, &dv,
                enzyme_const, i
            );
            const auto& etalon = etalons[i];
            std::println("d/dM = {}", dM);
            stdm::assert(stdr::equal(std::get<1>(etalon), dM));
            std::println("d/dv = {}", dv);
            stdm::assert(stdr::equal(std::get<0>(etalon), dv));
        }
    }
};

static const st::test test_vecmat{
    "stdm::vecmat", [] {
        std::println("v.T @ M = {}.T @ {} = {}", v, M, *stdm::vecmat(v, M));
        stdm::assert(stdr::equal(*stdm::vecmat(v, M), std::vector{ 3, 1, 2 }));

        vec dM(M.size());
        vec dv(v.size());
        const std::vector<std::pair<vec, vec>> etalons{
            { { 0, 0, 1 }, { 1, 0, 0, 2, 0, 0, 3, 0, 0 } },
            { { 1, 0, 0 }, { 0, 1, 0, 0, 2, 0, 0, 3, 0 } },
            { { 0, 1, 0 }, { 0, 0, 1, 0, 0, 2, 0, 0, 3 } },
        };
        for (auto i = 0uz; i < stdr::size(v); ++i) {
            std::println("Component ${}", i);
            stdr::fill(dM, 0);
            stdr::fill(dv, 0);

            __enzyme_autodiff(
                stdm::vecmat_element<decltype(M), decltype(v)>,
                enzyme_dup, &v, &dv,
                enzyme_dup, &M, &dM,
                enzyme_const, i
            );
            const auto& etalon = etalons[i];
            std::println("d/dM = {}", dM);
            stdm::assert(stdr::equal(std::get<1>(etalon), dM));
            std::println("d/dv = {}", dv);
            stdm::assert(stdr::equal(std::get<0>(etalon), dv));
        }
    }
};
