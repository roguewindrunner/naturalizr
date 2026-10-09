#include <naturalizr/rules.hpp>

#include <regex>

namespace naturalizr {
namespace {

const std::regex kDelve(R"(\bdelve into\b)", std::regex::icase);
const std::regex kSuruKoto(R"(することができます)");

} // namespace

std::string naturalize(const std::string& input) {
    std::string out = input;
    out = std::regex_replace(out, kDelve,    "dig into");
    out = std::regex_replace(out, kSuruKoto, "できます");
    return out;
}

} // namespace naturalizr