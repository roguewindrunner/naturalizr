#include <naturalizr/rules.hpp>

#include <toml++/toml.hpp>

#include <stdexcept>

namespace naturalizr {

RuleEngine RuleEngine::from_toml(std::string_view toml_text) {
    RuleEngine eng;

    toml::table tbl;
    try {
        tbl = toml::parse(toml_text);
    } catch (const toml::parse_error& e) {
        throw std::runtime_error(std::string("bad rules file: ") + e.description().data());
    }

    const auto* arr = tbl["rule"].as_array();
    if (!arr) throw std::runtime_error("rules file has no [[rule]] entries");

    for (const auto& node : *arr) {
        const auto* t = node.as_table();
        if (!t) continue;

        Rule r;
        r.id      = (*t)["id"].value_or("unnamed");
        r.lang    = (*t)["lang"].value_or("any");
        r.replace = (*t)["replace"].value_or("");

        auto pat = (*t)["pattern"].value<std::string>();
        if (!pat) throw std::runtime_error("rule '" + r.id + "' has no pattern");

        auto flags = std::regex::ECMAScript | std::regex::optimize;
        if (r.lang != "ja") flags |= std::regex::icase;

        try {
            r.pattern = std::regex(*pat, flags);
        } catch (const std::regex_error& e) {
            throw std::runtime_error("rule '" + r.id + "' bad regex: " + e.what());
        }

        eng.rules_.push_back(std::move(r));
    }

    return eng;
}

std::string RuleEngine::apply(const std::string& input) const {
    std::string out = input;
    for (const auto& r : rules_) {
        out = std::regex_replace(out, r.pattern, r.replace);
    }
    return out;
}

} // namespace naturalizr