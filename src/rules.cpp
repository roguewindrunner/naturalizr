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

namespace {

bool rule_applies(const Rule& r, Script script) {
    if (r.lang == "any") return true;
    switch (script) {
        case Script::Latin:    return r.lang == "en";
        case Script::Japanese: return r.lang == "ja";
        case Script::Mixed:    return true;   // run everything
    }
    return false;
}

} // namespace

Result RuleEngine::apply(const std::string& input) const {
    return apply(input, detect_script(input));
}

Result RuleEngine::apply(const std::string& input, Script script) const {
    Result res;
    res.text = input;

    for (const auto& r : rules_) {
        if (!rule_applies(r, script)) continue;

        const auto n = std::distance(
            std::sregex_iterator(res.text.begin(), res.text.end(), r.pattern),
            std::sregex_iterator());
        if (n == 0) continue;

        res.text = std::regex_replace(res.text, r.pattern, r.replace);
        res.hits.push_back({r.id, static_cast<int>(n)});
    }

    return res;
}

} // namespace naturalizr