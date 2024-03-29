# Experiments with using Enzyme

This repo is an eazy-to-setup place to experiment with using Enzyme from C++.

Tests, their status and requirements:
1. [eigen-simple](https://github.com/GregTheMadMonk/enzyme-tests/tree/master/tests/eigen-simple)
   a simple example that uses Eigen 3.4.0 with Enzyme.
2. [eigensumsqdyn-notmp](https://github.com/GregTheMadMonk/enzyme-tests/tree/master/tests/eigensumsqdyn-notmp)
   another Eigen+Enzyme example, taken from directly from Enzyme tests. Doesn't
   compile for an unknown reason. See [Enzyme#1800](https://github.com/EnzymeAD/Enzyme/issues/1800)
   for status updates.
3. [multi-tu](https://github.com/GregTheMadMonk/enzyme-tests/tree/master/tests/multi-tu)
   two translation units + Enzyme during linking (LLDEnzyme).
