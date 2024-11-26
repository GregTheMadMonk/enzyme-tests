import math.gmres;
import utils.selftest;
import enzyme;

import std;

static const utils::selftest::Test gmres{
    "/enzyme/math/gmres/gmres", [] {
        using vec = std::vector<float>;
        static constexpr auto mvs = [] (const vec& A, const vec& b) {
            vec x(b.size());
            math::gmres::solve(A, b, x);
            float ret = 0.0f;
            for (auto xe : x) ret += xe;
            return ret;
        };

        const vec A{ 1.0f, 0.0f, 0.0f, 2.0f };
        const vec b{ 1.0f, 2.0f };

        const auto x = math::gmres::solve(A, b);
        std::println("{} @ {} = {}", A, x, b);

        vec dA(A.size());
        vec db(b.size());
        __enzyme_autodiff<float>(
            +mvs,
            enzyme_dupnoneed, &A, &dA,
            enzyme_dupnoneed, &b, &db
        );

        std::println("dA = {}", dA);
        std::println("db = {}", db);
    }
};
