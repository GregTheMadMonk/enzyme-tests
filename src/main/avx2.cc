#ifdef USE_AVX2
#include <utility>

#include <immintrin.h>

#include <benchmark/benchmark.h>

import aligned;
import enzyme;
import utils;

double dot(const aligned::avx2_vector& u, const aligned::avx2_vector& v) {
    const auto sz = u.size();
    double ret = 0;
    for (std::size_t i = 0; i < sz; ++i) ret += u[i] * v[i];
    return ret;
}


auto d_dot(const aligned::avx2_vector& u, const aligned::avx2_vector& v) {
    aligned::avx2_vector du(u.size(), 0);
    aligned::avx2_vector dv(v.size(), 0);
    __enzyme_autodiff(
        (void*)&dot, enzyme_dup, &u, &du, enzyme_dup, &v, &dv
    );
    return std::pair{ du, dv };
}

/* Horizontal add works within 128-bit lanes. Use scalar ops to add
 * across the boundary. */
double reduce_vector1(__m256d input) {
  __m256d temp = _mm256_hadd_pd(input, input);
  return ((double*)&temp)[0] + ((double*)&temp)[2];
}

/* Another way to get around the 128-bit boundary: grab the first 128
 * bits, grab the lower 128 bits and then add them together with a 128
 * bit add instruction. */
double reduce_vector2(__m256d input) {
  __m256d temp = _mm256_hadd_pd(input, input);
  __m128d sum_high = _mm256_extractf128_pd(temp, 1);
  __m128d result = _mm_add_pd(sum_high, _mm256_castpd256_pd128(temp));
  return ((double*)&result)[0];
}

double dot_avx2_internal(const double *a, const double *b, std::size_t N) {
  __m256d sum_vec = _mm256_set_pd(0.0, 0.0, 0.0, 0.0);

  /* Add up partial dot-products in blocks of 256 bits */
  for(int ii = 0; ii < N/4; ++ii) {
    __m256d x = _mm256_load_pd(a+4*ii);
    __m256d y = _mm256_load_pd(b+4*ii);
    __m256d z = _mm256_mul_pd(x,y);
    sum_vec = _mm256_add_pd(sum_vec, z);
  }

  /* Find the partial dot-product for the remaining elements after
   * dealing with all 256-bit blocks. */
  double final = 0.0;
  for(int ii = N-N%4; ii < N; ++ii)
    final += a[ii] * b[ii];

  return reduce_vector2(sum_vec) + final;
} // <-- dot_avx2_internal()

double dot_avx2(const aligned::avx2_vector& u, const aligned::avx2_vector& v) {
    return dot_avx2_internal(u.data(), v.data(), u.size());
}

#if 0 // This doesn't work at the time!
auto d_dot_avx2(const aligned::avx2_vector& u, const aligned::avx2_vector& v) {
    aligned::avx2_vector du(u.size(), 0);
    aligned::avx2_vector dv(v.size(), 0);

    __enzyme_autodiff(
        (void*)&avx2::dot,
        enzyme_dup, u.data(), du.data(),
        enzyme_dup, v.data(), dv.data(),
        enzyme_const, u.size()
    );

    return std::pair{ du, dv };
} // <-- d_dot_avx2()
#endif

template <auto func>
void benchmark_dot(benchmark::State& state) {
    using utils::vectorSize;

    aligned::avx2_vector v(vectorSize);
    aligned::avx2_vector u(vectorSize);

    for (auto _ : state) {
        state.PauseTiming();
        utils::randomizeRange(v);
        utils::randomizeRange(u);
        state.ResumeTiming();

        benchmark::DoNotOptimize( func(u, v) );
    }
}

BENCHMARK(dot_bench<dot>);
BENCHMARK(dot_bench<dot_avx2>);

#endif
