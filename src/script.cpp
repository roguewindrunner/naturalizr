#include <naturalizr/script.hpp>

#include <cstddef>
#include <cstdint>

namespace naturalizr {
namespace {

// Decodes the UTF-8 character starting at text[i].
// Returns the codepoint and advances i past it.
std::uint32_t next_codepoint(std::string_view text, std::size_t& i) {
    const auto b0 = static_cast<unsigned char>(text[i]);

    if (b0 < 0x80) { i += 1; return b0; }

    std::size_t len = 0;
    std::uint32_t cp = 0;
    if      ((b0 & 0xE0) == 0xC0) { len = 2; cp = b0 & 0x1F; }
    else if ((b0 & 0xF0) == 0xE0) { len = 3; cp = b0 & 0x0F; }
    else if ((b0 & 0xF8) == 0xF0) { len = 4; cp = b0 & 0x07; }
    else { i += 1; return 0xFFFD; }   // stray continuation byte

    if (i + len > text.size()) { i = text.size(); return 0xFFFD; }

    for (std::size_t k = 1; k < len; ++k) {
        const auto bk = static_cast<unsigned char>(text[i + k]);
        if ((bk & 0xC0) != 0x80) { i += 1; return 0xFFFD; }  // truncated
        cp = (cp << 6) | (bk & 0x3F);
    }

    i += len;
    return cp;
}

bool is_japanese(std::uint32_t cp) {
    return (cp >= 0x3040 && cp <= 0x309F)    // hiragana
        || (cp >= 0x30A0 && cp <= 0x30FF)    // katakana
        || (cp >= 0x4E00 && cp <= 0x9FFF)    // CJK unified ideographs
        || (cp >= 0x3400 && cp <= 0x4DBF)    // CJK extension A
        || (cp >= 0xFF66 && cp <= 0xFF9D);   // halfwidth katakana
}

bool is_latin_letter(std::uint32_t cp) {
    return (cp >= 'A' && cp <= 'Z') || (cp >= 'a' && cp <= 'z');
}

} // namespace

Script detect_script(std::string_view text) {
    std::size_t ja = 0, latin = 0;

    for (std::size_t i = 0; i < text.size(); ) {
        const std::uint32_t cp = next_codepoint(text, i);
        if (is_japanese(cp))         ++ja;
        else if (is_latin_letter(cp)) ++latin;
    }

    if (ja == 0 && latin == 0) return Script::Latin;   // digits/punctuation only
    if (ja == 0)    return Script::Latin;
    if (latin == 0) return Script::Japanese;

    // Both present. Mixed unless one side is negligible.
    const double total    = static_cast<double>(ja + latin);
    const double ja_share = ja / total;

    if (ja_share < 0.05) return Script::Latin;      // stray 漢字 in English text
    if (ja_share > 0.95) return Script::Japanese;   // stray "API" in Japanese text
    return Script::Mixed;
}

const char* script_name(Script s) {
    switch (s) {
        case Script::Latin:    return "latin";
        case Script::Japanese: return "japanese";
        case Script::Mixed:    return "mixed";
    }
    return "unknown";
}

} // namespace naturalizr