## HOWTO

1. Use CMake as usually would when building with Enzyme
1. Use `ninja` 'cuz modules
1. Use `-DUSE_AVX2` to build AVX2 stuff (don't do this on ARM, naturally :) )
1. Use `-DUSE_OMP` to build OpenMP stuff
1. Use `VECTOR_SIZE` env variable when launching `./main` to set the vector
   size during benchmarking


## TL;DR so far

### One

Enzyme doesn't do AVX2 intrinsics at the moment:
```
cannot handle (reverse) unknown intrinsic
llvm.x86.avx.hadd.pd.256
  %30 = tail call noundef <4 x double> @llvm.x86.avx.hadd.pd.256(<4 x double> %9, <4 x double> %9) #17
```

**But** auto-vectorization seem to work for derivatives too (some AVX assembly
is in the binary, not sure about AVX2 though).

### Two

Auto-vectorization is great on simple cases:
```
benchmark_dot<dot>                887 ns          854 ns       628333
benchmark_dot<dot_avx2>           874 ns          842 ns       616290
```

More complex cases require further investigation
