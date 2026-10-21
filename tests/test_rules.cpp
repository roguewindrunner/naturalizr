#include <doctest/doctest.h>
#include <naturalizr/rules.hpp>

#include <string>

using naturalizr::RuleEngine;
using naturalizr::Script;

namespace {

// Tests define their own rules inline — they must not depend on
// data/rules.toml, which changes constantly.
RuleEngine make(const std::string& toml) {
    return RuleEngine::from_toml(toml);
}

const char* kBasic = R"(
[[rule]]
id = "en.test.delve"
lang = "en"
pattern = '\bdelve into\b'
replace = "dig into"

[[rule]]
id = "ja.test.dekiru"
lang = "ja"
pattern = 'することができます'
replace = "できます"
)";

} // namespace

TEST_CASE("english rule fires on latin text") {
    auto eng = make(kBasic);
    auto r = eng.apply("Let us delve into it.", Script::Latin);
    CHECK(r.text == "Let us dig into it.");
    REQUIRE(r.hits.size() == 1);
    CHECK(r.hits[0].id == "en.test.delve");
    CHECK(r.hits[0].count == 1);
}

TEST_CASE("japanese rule fires on japanese text") {
    auto eng = make(kBasic);
    auto r = eng.apply("使用することができます。", Script::Japanese);
    CHECK(r.text == "使用できます。");
}

TEST_CASE("rules are filtered by script") {
    auto eng = make(kBasic);
    // Forcing Japanese means the English rule must not run.
    auto r = eng.apply("Let us delve into it.", Script::Japanese);
    CHECK(r.text == "Let us delve into it.");
    CHECK(r.hits.empty());
}

TEST_CASE("mixed runs both rulesets") {
    auto eng = make(kBasic);
    auto r = eng.apply("We delve into データを分析することができます。", Script::Mixed);
    CHECK(r.text == "We dig into データを分析できます。");
    CHECK(r.hits.size() == 2);
}

TEST_CASE("repeated matches are counted") {
    auto eng = make(kBasic);
    auto r = eng.apply("delve into A, delve into B.", Script::Latin);
    REQUIRE(r.hits.size() == 1);
    CHECK(r.hits[0].count == 2);
}

TEST_CASE("capture groups work") {
    auto eng = make(R"(
[[rule]]
id = "en.test.utilize"
lang = "en"
pattern = '\butiliz(e|es|ed|ing)\b'
replace = "us$1"
)");
    CHECK(eng.apply("utilizing", Script::Latin).text == "using");
    CHECK(eng.apply("utilized",  Script::Latin).text == "used");
}

TEST_CASE("input with no matches is returned unchanged") {
    auto eng = make(kBasic);
    const std::string in = "Nothing here matches anything at all.";
    auto r = eng.apply(in, Script::Latin);
    CHECK(r.text == in);
    CHECK(r.hits.empty());
}

TEST_CASE("malformed toml throws") {
    CHECK_THROWS_AS(make("this is not toml ["), std::runtime_error);
    CHECK_THROWS_AS(make("[[rule]]\nid = \"x\"\n"), std::runtime_error);  // no pattern
    CHECK_THROWS_AS(make("title = \"nope\""), std::runtime_error);        // no [[rule]]
}

TEST_CASE("bad regex names the offending rule") {
    try {
        make("[[rule]]\nid = \"en.broken\"\npattern = '([unclosed'\n");
        FAIL("expected throw");
    } catch (const std::runtime_error& e) {
        CHECK(std::string(e.what()).find("en.broken") != std::string::npos);
    }
}