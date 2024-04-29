This example loads a simple 2D VTU mesh and performs a straight line trace through it.
Mesh cell data combined with the trace result is then converted to a scalar.
The derivatives of this scalar function are calculated with Enzyme.

**Required LLVM:** tried LLVM 17, but any with C++20 support should work

**Required CMake:** idk any?
