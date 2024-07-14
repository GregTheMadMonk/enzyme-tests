#include <new>
#include <limits>
#include <vector>

#include <immintrin.h>

template <typename T, std::size_t alignBytes = 64>
class AlignedAllocator {
private:
    static_assert(
        alignBytes >= alignof(T),
        "Minimum alignment requirements not met"
    );

public:
    using value_type = T;
    static constexpr std::align_val_t alignment{ alignBytes };

    template <typename OtherT>
    struct rebind {
        using other = AlignedAllocator<OtherT, alignBytes>;
    };

    constexpr AlignedAllocator() noexcept = default;
    constexpr AlignedAllocator(const AlignedAllocator&) noexcept = default;

    template <typename U>
    constexpr AlignedAllocator(const AlignedAllocator<U, alignBytes>&) noexcept {}

    [[nodiscard]]
    T* allocate(std::size_t num) {
        if (num > std::numeric_limits<std::size_t>::max() / sizeof(T)) {
            throw std::bad_array_new_length{};
        }

        const auto bytes = num * sizeof(T);
        return reinterpret_cast<T*>(
            ::operator new[]( bytes, alignment )
        );
    }

    void deallocate(T* ptr, [[maybe_unused]] std::size_t num) {
        ::operator delete[]( ptr, alignment );
    }
};

using aligned_vector = std::vector<double, AlignedAllocator<double, 64>>;

extern double __enzyme_autodiff(void*, ...);
extern int enzyme_dup, enzyme_const;

double dot_vec(const aligned_vector& u, const aligned_vector& v) {
    const auto sz = u.size();
    double ret = 0;
    for (std::size_t i = 0; i < sz; ++i) ret += u[i] * v[i];
    return ret;
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

#ifndef NO_INTRINSICS
double dot_avx(const double *a, const double *b, std::size_t N) {
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
}
#endif

auto d_dot_vec(const aligned_vector& u, const aligned_vector& v) {
    aligned_vector du(u.size(), 0);
    aligned_vector dv(v.size(), 0);
    __enzyme_autodiff(
        (void*)&dot_vec, enzyme_dup, &u, &du, enzyme_dup, &v, &dv
    );
    return std::pair{ du, dv };
}

#ifndef NO_INTRINSICS
auto d_dot_avx(const aligned_vector& u, const aligned_vector& v) {
    aligned_vector du(u.size(), 0);
    aligned_vector dv(v.size(), 0);

    __enzyme_autodiff(
        (void*)&dot_avx,
        enzyme_dup, u.data(), du.data(),
        enzyme_dup, v.data(), dv.data(),
        enzyme_const, u.size()
    );
}
#endif

int main() {
}

