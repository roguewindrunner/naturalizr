#include <iostream>
#include <sstream>
#include <string>

int main() {
    std::ios::sync_with_stdio(false);

    std::ostringstream buf;
    buf << std::cin.rdbuf();
    const std::string input = buf.str();

    std::cout << input;
    return 0;
}