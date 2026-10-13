#pragma once

#include <regex>
#include <string>
#include <string_view>
#include <vector>
#include <naturalizr/script.hpp>

namespace naturalizr {

struct Rule {
    std::string id;
    std::string lang;
    std::regex  pattern;
    std::string replace;
};

struct Hit {
    std::string id;
    int count;
};

struct Result {
    std::string      text;
    std::vector<Hit> hits;
};

class RuleEngine {
public:
    RuleEngine() = default;

    // Parses TOML rule definitions.
    // Throws std::runtime_error on malformed TOML or a bad regex.
    static RuleEngine from_toml(std::string_view toml_text);

    Result apply(const std::string& input) const;
    Result apply(const std::string& input, Script script) const;
    std::size_t size() const { return rules_.size(); }

private:
    std::vector<Rule> rules_;
};

} // namespace naturalizr