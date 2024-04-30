#include <iostream>
#include <stdexcept>

#include <enzyme/enzyme>

#include "provider.hh"

int main() {
    const float x = 2.0f;
    float df_enzyme = 0.0f; // Always set derivative vars to zero!
    __enzyme_autodiff<void>(&f, enzyme_dup, &x, &df_enzyme);
    const auto df_actual = df(x);
    std::cout << "Compare " << df_enzyme << " to " << df_actual << '\n';
    if (df_enzyme != df_actual) throw std::logic_error{"Wrong answer!!!"};
    return 0;
}
