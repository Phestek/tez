#include <iostream>
#include <fstream>

#include "lexer.h"
#include "parser.h"
#include "c_code_gen.h"

struct compilation_settings {
    std::vector<std::string> input_files;
    std::string              output_file = "output.c";
};

void print_help() {
    std::cout << "Usage: waywardc [options] [input_files]\n"
            << "Options: \n"
            << "  -h, --help      - Display this information.\n"
            << "  -o <file>       - Place the output into <file>.\n";
}

bool parse_command_line_arguments(const std::vector<std::string> args,
        compilation_settings& settings) {
    bool errors = false;
    for(std::size_t i = 0; i < args.size(); ++i) {
        if(args[i][0] != '-') {
            settings.input_files.push_back(args[i]);
            continue;
        }
        if(args[i] == "-h" || args[i] == "--help") {
            print_help();
            continue;
        }
        try {
            if(args[i] == "-o") {
                settings.output_file = args.at(++i);
                continue;
            }
        } catch(const std::out_of_range& e) {
            std::cerr << "Error: Expected file name after '-o'.\n";
            errors = true;
            continue;
        }
        std::cerr << "Error: Unknown option '" + args[i] + "'.\n";
        errors = true;
    }
    return !errors;
}

int compile(const compilation_settings& settings) {
    std::vector<wayward::token> tokens;
    for(const auto& f : settings.input_files) {
        wayward::lexer lexer{f};
        auto t = lexer.tokenize();
        if(lexer.errors_reported()) {
            return 1;
        }
        tokens.insert(tokens.end(), t.begin(), t.end());
    }

    wayward::parser parser{tokens};
    auto ast = parser.parse();
    if(parser.errors_reported()) {
        return 2;
    }

    auto c_code = generate_c_code(ast);
    std::ofstream of{settings.output_file};
    of << c_code;

    return 0;
}

int main(int argc, char* argv[]) {
    if(argc < 2) {
        print_help();
        return 0;
    }

    std::vector<std::string> args(argv + 1, argv + argc);
    compilation_settings     settings;
    if(!parse_command_line_arguments(args, settings)) {
        return 1;
    }

    std::cout << "Input files: ";
    for(auto& f : settings.input_files) {
        std::cout << f << ' ';
    }
    std::cout << "\n";
    std::cout << "Ouput file: " << settings.output_file << "\n";

    return compile(settings);
}

