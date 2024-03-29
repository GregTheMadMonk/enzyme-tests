#include <iostream>
#include <iomanip>

#include <c-enzyme.hh>

double f(double x); // External!

int main() {
    for (double x = -4.0; x <= 4.0; x += 0.5) {
        std::cout
            << "x = " << std::setw(4) << x
            << "\tf(x) = " << std::setw(7) << f(x)
            << "\tf'(x) = " << std::setw(7) << __enzyme_autodiff(f, x)
            << "\texpect f'(x) = " << std::setw(7) << 3 * x * x
            << '\n';
    }
    return 0;
}
