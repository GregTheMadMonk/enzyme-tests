## TL;DR

Enzyme doesn't do AVX2 intrinsics at the moment:
```
cannot handle (reverse) unknown intrinsic
llvm.x86.avx.hadd.pd.256
  %30 = tail call noundef <4 x double> @llvm.x86.avx.hadd.pd.256(<4 x double> %9, <4 x double> %9) #17
```

**But** auto-vectorization seem to work for derivatives too (some AVX assembly
is in the binary, not sure about AVX2 though).
