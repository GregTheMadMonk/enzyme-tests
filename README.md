# Experiments with using Enzyme

This repo is an eazy-to-setup place to experiment with using Enzyme from C++.

## [eigen-simple](https://github.com/GregTheMadMonk/enzyme-tests/tree/master/tests/eigen-simple)
A simple example that uses Eigen 3.4.0 with Enzyme.

## [eigensumsqdyn-notmp](https://github.com/GregTheMadMonk/enzyme-tests/tree/master/tests/eigensumsqdyn-notmp)
Another Eigen+Enzyme example, taken from directly from Enzyme tests. Doesn't
compile for an unknown reason. See [Enzyme#1800](https://github.com/EnzymeAD/Enzyme/issues/1800)
for status updates.

## [multi-tu](https://github.com/GregTheMadMonk/enzyme-tests/tree/master/tests/multi-tu)
Two translation units + Enzyme during linking (LLDEnzyme).

## [mhfem](https://github.com/GregTheMadMonk/enzyme-tests/tree/master/tests/mhfem)
MHFEM differentiation with Enzyme.

Function signature convention:
1. If a function needs to return a primitive, return it as-is
2. If a function needs to return a vector, return it via `std::unique_ptr`
3. Accept primitive arguments as-is (by value)
4. Accept vector arguments by-reference

If **2** is not followed, Enzyme will fail to differentiate the algorithm.
It could be replaced with an output parameter, but I don't like those.
It takes a very long time to differentiate the program this way, and emits a few
`freeing without malloc ptr` errors, but works :)

This tests uses C++20 modules and some C++23 features. Needs **Ninja**, at least
**CMake 3.28**, and an LLVM build with the following features:
1. C++20 modules support
2. A standard library implementation that supports `import std` (LLVM 19)

I personally used LLVM 19.0.0 fe2119a7b08b6e468b2a67768904ea85b1bf0a45 at the
time of writing.
