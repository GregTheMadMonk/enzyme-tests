This example produces two separate binaries by linking together the same
`main.cc` main source file with two different "provider" source files that
provide a different math function to differentiate for the binary. They are
later lined with `LLDEnzymeFlags`.

_Note: `include_directories` call is necessary in CMake for now since
`LLDEnzymeFlags` target does not provide source files with the Enzyme headers._

**Required LLVM:** tried LLVM 17, but practically anything should work

**Required CMake:** idk any?
