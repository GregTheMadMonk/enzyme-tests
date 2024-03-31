#include "test_math_common.hh"

static const st::test test_sqlen{
    "Simple sqlen() lambda", [] {
        static constexpr auto sqlen = [] (const std::vector<float>& x) {
            return stdm::dot(x, x);
        };

        std::println("{} length squared is {}", v, sqlen(v));
        stdm::assert(sqlen(v) == 14);

        vec dv(v.size());
        __enzyme_autodiff(+sqlen, enzyme_dup, &v, &dv);
        std::println("d(sqlen)/dv = {}", dv);
        stdm::assert(stdr::equal(dv, std::vector{ 2, 4, 6 }));
    }
};

static const st::test test_sqlen_after{
    "sqlen_after() lambda: length after matrix transformation", [] {
        static constexpr auto len_after =
            [] (const std::vector<float>& m, const std::vector<float>& x) {
                const auto mx = stdm::matvec(m, x);
                return stdm::dot(*mx, *mx);
            };


        std::println(
            "Sq. length of {} after applying {} is {} (old sq. length: {})",
            v, M, len_after(M, v), stdm::dot(v, v)
        );
        stdm::assert(len_after(M, v) == stdm::dot(v, v)); // Unchanged

        vec dM(M.size());
        vec dv(v.size());
        __enzyme_autodiff(
            +len_after,
            enzyme_dup, &M, &dM,
            enzyme_dup, &v, &dv
        );
        std::println("d/dM = {}", dM);
        std::println("d/dv = {}", dv);
        stdm::assert(stdr::equal(dv, std::vector{ 2, 4, 6 }));
        stdm::assert(
            stdr::equal(
                dM, std::vector{ 4, 8, 12, 6, 12, 18, 2, 4, 6 }
            )
        );
    }
};
