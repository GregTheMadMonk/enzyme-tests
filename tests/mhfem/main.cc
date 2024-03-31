import std;
import stdm;

#include <c-enzyme.hh>

namespace stdr = std::ranges;

int main() {
    std::println("Test stdm::dot()...");

    std::vector<float>  v{ 1, 2, 3 };
    std::vector<double> u{ 3, 2, 1 };
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


    std::println("Test stdm::matvec()...");

    std::vector<float> M{
        0, 1, 0,
        0, 0, 1,
        1, 0, 0
    };
    std::println("M @ v = {} @ {} = {}", M, v, *stdm::matvec(M, v));
    stdm::assert(stdr::equal(*stdm::matvec(M, v), std::vector{ 2, 3, 1 }));

    decltype(M) dM(M.size());
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
        std::println("d/dM = {}", dM);
        std::println("d/dv = {}", dv);
    }


    std::println("Test stdm::vecmat()...");
    std::println("v.T @ M = {}.T @ {} = {}", v, M, *stdm::vecmat(v, M));
    stdm::assert(stdr::equal(*stdm::vecmat(v, M), std::vector{ 3, 1, 2 }));

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
        std::println("d/dM = {}", dM);
        std::println("d/dv = {}", dv);
    }


    std::println("Test sqlen() lambda...");
    static constexpr auto sqlen = [] (const std::vector<float>& x) {
        return stdm::dot(x, x);
    };


    std::println("{} length squared is {}", v, sqlen(v));

    stdr::fill(dv, 0);
    __enzyme_autodiff(+sqlen, enzyme_dup, &v, &dv);
    std::println("d(sqlen)/dv = {}", dv);
    stdm::assert(stdr::equal(dv, std::vector{ 2, 4, 6 }));


    std::println("Test vector length converter...");
    static constexpr auto len_after =
        [] (const std::vector<float>& m, const std::vector<float>& x) {
            const auto mx = stdm::matvec(m, x);
            return stdm::dot(*mx, *mx);
        };


    std::println(
        "Length squared of {} after applying {} is {} (old length squared {})",
        v, M, len_after(M, v), stdm::dot(v, v)
    );
    stdr::fill(dM, 0);
    stdr::fill(dv, 0);

    __enzyme_autodiff(
        +len_after,
        enzyme_dup, &M, &dM,
        enzyme_dup, &v, &dv
    );
    std::println("d/dM = {}", dM);
    std::println("d/dv = {}", dv);

    return 0;
}
