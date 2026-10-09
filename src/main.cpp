#include <iostream>
#include <sstream>
#include <string>
#include <regex>


static std::string naturalize(const std::string& in) {
    // English: word boundaries work, since spaces delimit words.
    static const std::regex kDelve(R"(\bdelve into\b)", std::regex::icase);

    // Japanese: no \b, no character classes. Literal byte sequences only.
    static const std::regex kSuruKoto(R"(することができます)");

    std::string out = in;
    out = std::regex_replace(out, kDelve,   "dig into");
    out = std::regex_replace(out, kSuruKoto, "できます");
    return out;
}

int main() {
    std::ios::sync_with_stdio(false);

    std::ostringstream buf;
    buf << std::cin.rdbuf();
    const std::string input = buf.str();

    std::cout << naturalize(input);
    return 0;
}