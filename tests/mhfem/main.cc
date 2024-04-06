import selftest;
import math;
import std;

int main() {
    selftest::run_all();

    math::gmres solver{ std::vector<float>{ 1, 2, 3, 4 } };
    return 0;
}
