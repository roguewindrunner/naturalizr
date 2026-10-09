#pragma once

#include <regex>
#include <string>
#include <string_view>
#include <vector>

namespace naturalizr {

struct Rule {
    std::string id;
    std::string lang;
    std::regex  pattern;
    std::string replace;
};

class RuleEngine {
public:
    RuleEngine() = default;

    // Parses TOML rule definitions.
    // Throws std::runtime_error on malformed TOML or a bad regex.
    static RuleEngine from_toml(std::string_view toml_text);

    std::string apply(const std::string& input) const;
    std::size_t size() const { return rules_.size(); }

private:
    std::vector<Rule> rules_;
};

} // namespace naturalizr