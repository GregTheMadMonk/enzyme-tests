#include <utility>

#include <benchmark/benchmark.h>

import aligned;
import enzyme;
import main;

double dot_vec(const aligned::avx2_vector& u, const aligned::avx2_vector& v) {
    const auto sz = u.size();
    double ret = 0;
    for (std::size_t i = 0; i < sz; ++i) ret += u[i] * v[i];
    return ret;
}


auto d_dot_vec(const aligned::avx2_vector& u, const aligned::avx2_vector& v) {
    aligned::avx2_vector du(u.size(), 0);
    aligned::avx2_vector dv(v.size(), 0);
    __enzyme_autodiff(
        (void*)&dot_vec, enzyme_dup, &u, &du, enzyme_dup, &v, &dv
    );
    return std::pair{ du, dv };
}

BENCHMARK_MAIN();
