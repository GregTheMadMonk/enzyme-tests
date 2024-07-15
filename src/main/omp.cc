#ifdef USE_OMP

#include <iostream>

#include <benchmark/benchmark.h>

import aligned;
import utils;

/// v.T @ M @ v
double simple(const aligned::avx2_vector& v, const aligned::avx2_vector& M) {
    const auto N = v.size();

    double ret = 0;

    for (std::size_t i = 0; i < N; ++i) {
        for (std::size_t j = 0; j < N; ++j) {
            ret += v[i] * M[i * N + j] * v[j];
        }
    }

    return ret;
} // <-- quadratic

double omp_naive(const aligned::avx2_vector& v, const aligned::avx2_vector& M) {
    const auto N = v.size();

    double ret = 0;

    #pragma omp parallel for
    for (std::size_t i = 0; i < N; ++i) {
        #pragma omp parallel for
        for (std::size_t j = 0; j < N; ++j) {
            ret += v[i] * M[i * N + j] * v[j];
        }
    }

    return ret;
}

double omp(const aligned::avx2_vector& v, const aligned::avx2_vector& M) {
    const auto N = v.size();

    double ret = 0;

    aligned::avx2_vector interm(N);
    #pragma omp parallel for
    for (std::size_t i = 0; i < N; ++i) {
        for (std::size_t j = 0; j < N; ++j) {
            interm[i] += M[i * N + j] * v[j];
        }
    }
    for (std::size_t i = 0; i < N; ++i) ret += interm[i];

    return ret;
}

template <auto func>
void quadratic_bench(benchmark::State& state) {
    using utils::vectorSize;

    aligned::avx2_vector v(vectorSize);
    aligned::avx2_vector M(vectorSize * vectorSize);

    for (auto _ : state) {
        state.PauseTiming();
        utils::randomizeRange(v);
        utils::randomizeRange(M);
        state.ResumeTiming();

        benchmark::DoNotOptimize( func(v, M) );
    }
}

BENCHMARK(quadratic_bench<simple>);
BENCHMARK(quadratic_bench<omp>);
BENCHMARK(quadratic_bench<omp_naive>);

#endif
