#include "kalyadb/version.hpp"

#include <cstdlib>
#include <cstring>

int main() {
    return std::strcmp(kalyadb::version(), "0.1.0") == 0
        ? EXIT_SUCCESS
        : EXIT_FAILURE;
}
