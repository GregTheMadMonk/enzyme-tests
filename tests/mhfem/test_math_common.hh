#pragma once

import std;
import stdm;

import selftest;

#include <c-enzyme.hh>

namespace stdr = std::ranges;
namespace st   = selftest;

using vec = std::vector<float>;

inline const std::vector<float>  v{ 1, 2, 3 };
inline const std::vector<double> u{ 3, 2, 1 };
inline const vec M{
    0, 1, 0,
    0, 0, 1,
    1, 0, 0
};
