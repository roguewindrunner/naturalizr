#include <naturalizr/rules.hpp>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

int main() {
    std::ios::sync_with_stdio(false);

    const std::string rules_path = "data/rules.toml";
    std::ifstream f(rules_path);
    if (!f) {
        std::cerr << "cannot open " << rules_path << '\n';
        return 1;
    }
    std::ostringstream rb;
    rb << f.rdbuf();

    naturalizr::RuleEngine engine;
    try {
        engine = naturalizr::RuleEngine::from_toml(rb.str());
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        return 1;
    }
    std::cerr << "loaded " << engine.size() << " rules\n";

    std::ostringstream buf;
    buf << std::cin.rdbuf();
    const std::string input = buf.str();
    const auto script = naturalizr::detect_script(input);
    std::cerr << "script: " << naturalizr::script_name(script) << '\n';
    std::cout << engine.apply(input, script);
    return 0;
}