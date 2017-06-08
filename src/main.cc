#include <iostream>
#include <fstream>

#include "codegen/c_code_generator.h"
#include "syntax/lexer.h"
#include "syntax/parser.h"

struct Compilation_Settings {
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
        Compilation_Settings& settings) {
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

int compile(const Compilation_Settings& settings) {
    std::vector<tez::Token> tokens;
    for(const auto& f : settings.input_files) {
        tez::Lexer lexer{f};
        auto t = lexer.tokenize();
        if(lexer.errors_reported()) {
            return 1;
        }
        tokens.insert(tokens.end(), t.begin(), t.end());
    }

    tez::Parser parser{tokens};
    auto ast = parser.parse();
    if(parser.errors_reported()) {
        return 2;
    }

    tez::C_Code_Generator c_code_gen{ast};
    auto c_source = c_code_gen.generate();
    std::ofstream of{settings.output_file};
    of << c_source;

    return 0;
}

int main(int argc, char* argv[]) {
    if(argc < 2) {
        print_help();
        return 0;
    }

    std::vector<std::string> args(argv + 1, argv + argc);
    Compilation_Settings     settings;
    if(!parse_command_line_arguments(args, settings)) {
        return 1;
    }

    return compile(settings);
}

