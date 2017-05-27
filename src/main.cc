#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>

#include "lexer.h"
#include "parser.h"
#include "c_code_gen.h"

bool check_flag(const std::string& flag, std::vector<std::string>& args) {
    const auto it = std::find(std::begin(args), std::end(args), flag);
    const bool contains = it != std::end(args);
    if(contains) args.erase(it);
    return contains;
}

bool check_string_option(std::string& out_value, const std::string& flag_prefix, std::vector<std::string>& args) {
    const auto it = std::find_if(std::begin(args), std::end(args), [&](const std::string& arg){
        return !arg.compare(0, flag_prefix.size(), flag_prefix);
    });

    const bool contains = it != std::end(args);

    out_value = it->substr(flag_prefix.size());

    if(contains) args.erase(it);
    return contains;
}

void print_usage() {
    std::cout << "Usage:\n"
            "waywardc <input_file>\n";
}

bool compile(const std::vector<std::string>& input_files,
        const std::string& output_file) {
    std::vector<wayward::token> tokens;
    for(const auto& f : input_files) {
        wayward::lexer lexer{f};
        auto t = lexer.tokenize();
        if(lexer.errors_reported()) {
            return false;
        }
        tokens.insert(tokens.end(), t.begin(), t.end());
    }

    wayward::parser parser{tokens};
    auto ast = parser.parse();
    if(parser.errors_reported()) {
        return false;
    }
    
    auto c_code = generate_c_code(ast);
    std::ofstream of{output_file};
    of << c_code;

    return true;
}

int main(int argc, char* argv[]) {

    std::vector<std::string> args(argv + 1, argv + argc);

    std::string output_file = "output.c";
    check_string_option(output_file, "-o=", args);

    for(auto& arg : args) {
        if(arg.at(0) == '-') {
            std::cerr << "Unsupported argument: " << arg << "\n";
            return 1;
        }
    }

    if(args.empty()) {
        std::cerr << "No input files\n";
        return 1;
    }

    std::cout << "Input files: ";
    for(auto& f : args) {
        std::cout << f << ' ';
    }
    std::cout << "\n";

    std::cout << "Ouput file: " << output_file << "\n";

    compile(args, output_file);

    return 0;
}

