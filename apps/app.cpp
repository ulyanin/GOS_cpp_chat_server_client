#include <chat_server/lib.h>

#include <iostream>
#include <vector>
#include <tuple>

int main() {
    std::vector<double> input = {1.2, 2.3, 3.4, 4.5};

    auto [mean, moment] = accumulate_vector(input);

    std::cout << "Mean: " << mean << " moment: " << moment << std::endl;

    return 0;
}
