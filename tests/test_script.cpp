#include <doctest/doctest.h>
#include <naturalizr/script.hpp>

using naturalizr::Script;
using naturalizr::detect_script;

TEST_CASE("pure latin") {
    CHECK(detect_script("Hello world.") == Script::Latin);
    CHECK(detect_script("") == Script::Latin);
    CHECK(detect_script("12345 !@#$%") == Script::Latin);
}

TEST_CASE("pure japanese") {
    CHECK(detect_script("これはテストです。") == Script::Japanese);
    CHECK(detect_script("日本語") == Script::Japanese);
}

TEST_CASE("loanwords do not flip classification") {
    // 3 latin chars against ~13 japanese: still essentially japanese text,
    // but we accept Mixed since running both rulesets is harmless.
    const auto s = detect_script("このAPIを使用することができます。");
    CHECK((s == Script::Japanese || s == Script::Mixed));
}

TEST_CASE("genuinely bilingual text is mixed") {
    CHECK(detect_script("We delve into データを分析することができます。") == Script::Mixed);
}

TEST_CASE("malformed utf-8 does not crash or hang") {
    CHECK_NOTHROW(detect_script("\xE3\x81"));          // truncated 3-byte char
    CHECK_NOTHROW(detect_script("\x80\x80\x80"));      // stray continuations
    CHECK_NOTHROW(detect_script("abc\xFF""def"));        // invalid byte
}