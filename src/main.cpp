#include <iostream>
#include <sstream>
#include <string>
#include <naturalizr/rules.hpp>


int main() {
    std::ios::sync_with_stdio(false);

    std::ostringstream buf;
    buf << std::cin.rdbuf();

    std::cout << naturalizr::naturalize(buf.str()) << std::endl;
    return 0;
}