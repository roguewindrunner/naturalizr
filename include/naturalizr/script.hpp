#pragma once

#include <string_view>

namespace naturalizr {

enum class Script { Latin, Japanese, Mixed };

// Classifies text by the share of Japanese characters it contains.
Script detect_script(std::string_view text);

const char* script_name(Script s);

} // namespace naturalizr