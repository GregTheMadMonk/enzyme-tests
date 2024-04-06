#include "test_math_common.hh"

static const st::test test_sqlen{
    "Simple sqlen() lambda", [] {
        static constexpr auto sqlen = [] (const std::vector<float>& x) {
            return math::dot(x, x);
        };

        std::println("{} length squared is {}", v, sqlen(v));
        math::assert(sqlen(v) == 14);

        vec dv(v.size());
        __enzyme_autodiff(+sqlen, enzyme_dup, &v, &dv);
        std::println("d(sqlen)/dv = {}", dv);
        math::assert(stdr::equal(dv, std::vector{ 2, 4, 6 }));
    }
};

static const st::test test_sqlen_after{
    "sqlen_after() lambda: length after matrix transformation", [] {
        static constexpr auto len_after =
            [] (const std::vector<float>& m, const std::vector<float>& x) {
                const auto mx = math::matvec(m, x);
                return math::dot(*mx, *mx);
            };


        std::println(
            "Sq. length of {} after applying {} is {} (old sq. length: {})",
            v, M, len_after(M, v), math::dot(v, v)
        );
        math::assert(len_after(M, v) == math::dot(v, v)); // Unchanged

        vec dM(M.size());
        vec dv(v.size());
        __enzyme_autodiff(
            +len_after,
            enzyme_dup, &M, &dM,
            enzyme_dup, &v, &dv
        );
        std::println("d/dM = {}", dM);
        std::println("d/dv = {}", dv);
        math::assert(stdr::equal(dv, std::vector{ 2, 4, 6 }));
        math::assert(
            stdr::equal(
                dM, std::vector{ 4, 8, 12, 6, 12, 18, 2, 4, 6 }
            )
        );
    }
};
