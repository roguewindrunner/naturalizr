#include <naturalizr/rules.hpp>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace  {

    constexpr const char* kVersion = "0.1.0";

    void usage(std::ostream& os) {
    os << "naturalizr " << kVersion << " - offline text style cleaner (EN/JA)\n\n"
       << "usage: naturalizr [options] [file]\n"
       << "       cat file | naturalizr [options]\n\n"
       << "options:\n"
       << "  -r, --rules PATH    rules file (default: data/rules.toml)\n"
       << "  -l, --lang LANG     force en|ja|mixed instead of detecting\n"
       << "  -o, --output PATH   write result to PATH instead of stdout\n"
       << "  -s, --stats         print which rules fired (stderr)\n"
       << "  -q, --quiet         suppress informational output\n"
       << "  -V, --version       print version\n"
       << "  -h, --help          this help\n";
    }

    bool guzzle(std::istream& in, std::string& out) {
        std::ostringstream ss;
        ss << in.rdbuf();
        out = ss.str();
        return static_cast<bool>(in) || in.eof();
    }

    bool read_file(const std::string& path, std::string& out) {
        std::ifstream f(path, std::ios::binary);
        if (!f) return false;
        return guzzle(f, out);
    }
}
int main(int argc, char** argv) {
        std::ios::sync_with_stdio(false);

    std::string rules_path = "data/rules.toml";
    std::string lang, out_path, in_path;
    bool stats = false, quiet = false;

    for (int i = 1; i < argc; ++i) {
        const std::string a = argv[i];
        auto next = [&](const char* what) -> std::string {
            if (i + 1 >= argc) {
                std::cerr << "naturalizr: " << what << " needs a value\n";
                std::exit(2);
            }
            return argv[++i];
        };

        if      (a == "-h" || a == "--help")    { usage(std::cout); return 0; }
        else if (a == "-V" || a == "--version") { std::cout << kVersion << '\n'; return 0; }
        else if (a == "-s" || a == "--stats")   { stats = true; }
        else if (a == "-q" || a == "--quiet")   { quiet = true; }
        else if (a == "-r" || a == "--rules")   { rules_path = next("--rules"); }
        else if (a == "-l" || a == "--lang")    { lang = next("--lang"); }
        else if (a == "-o" || a == "--output")  { out_path = next("--output"); }
        else if (!a.empty() && a[0] == '-' && a != "-") {
            std::cerr << "naturalizr: unknown option " << a << "\n\n";
            usage(std::cerr);
            return 2;
        }
        else { in_path = a; }
    }

        std::string rules_src;
    if (!read_file(rules_path, rules_src)) {
        std::cerr << "naturalizr: cannot read rules file: " << rules_path << '\n';
        return 1;
    }

    naturalizr::RuleEngine engine;
    try {
        engine = naturalizr::RuleEngine::from_toml(rules_src);
    } catch (const std::exception& e) {
        std::cerr << "naturalizr: " << e.what() << '\n';
        return 1;
    }

    std::string input;
    if (in_path.empty() || in_path == "-") {
        guzzle(std::cin, input);
    } else if (!read_file(in_path, input)) {
        std::cerr << "naturalizr: cannot read input file: " << in_path << '\n';
        return 1;
    }

    naturalizr::Script script;
    if      (lang.empty())    script = naturalizr::detect_script(input);
    else if (lang == "en")    script = naturalizr::Script::Latin;
    else if (lang == "ja")    script = naturalizr::Script::Japanese;
    else if (lang == "mixed") script = naturalizr::Script::Mixed;
    else {
        std::cerr << "naturalizr: --lang must be en, ja, or mixed\n";
        return 2;
    }

    const auto result = engine.apply(input, script);

    if (!out_path.empty()) {
        std::ofstream of(out_path, std::ios::binary);
        if (!of) {
            std::cerr << "naturalizr: cannot write: " << out_path << '\n';
            return 1;
        }
        of << result.text;
    } else {
        std::cout << result.text;
    }

    if (stats) {
        int total = 0;
        for (const auto& h : result.hits) total += h.count;
        std::cerr << "--- " << engine.size() << " rules loaded, script: "
                  << naturalizr::script_name(script) << ", " << total << " replacements\n";
        for (const auto& h : result.hits) {
            std::cerr << "  " << h.count << "x  " << h.id << '\n';
        }
    } else if (!quiet) {
        std::cerr << "script: " << naturalizr::script_name(script) << '\n';
    }

    return 0;
}
