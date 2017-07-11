#include <iostream>
#include <fstream>

#if TEZ_DEBUG
#   include <chrono>
#endif

#include <llvm/Support/raw_ostream.h>

//#include "semantic/semantic_analyzer.h"
#include "syntax/lexer.h"
#include "syntax/parser.h"

struct Compilation_Settings {
    std::vector<std::string> input_files;
    std::string              output_file = "output.c";
    bool                     use_llvm = false;
};

void print_help() {
    std::cout << "Usage: waywardc [options] [input_files]\n"
            "Options: \n"
            "  -h            - Display this information.\n"
            "  -o <file>     - Place the output into <file>.\n"
            "  -llvm         - Use LLVM backend instead C backend.\n";
}

bool parse_command_line_arguments(const std::vector<std::string> args,
        Compilation_Settings& settings) {
    bool errors = false;
    for(std::size_t i = 0; i < args.size(); ++i) {
        if(args[i][0] != '-') {
            settings.input_files.push_back(args[i]);
            continue;
        }
        if(args[i] == "-h") {
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
        if(args[i] == "-llvm") {
            settings.use_llvm = true;
            continue;
        }
        std::cerr << "Error: Unknown option '" + args[i] + "'.\n";
        errors = true;
    }
    return !errors;
}

int compile(const Compilation_Settings& settings) {
    std::vector<tez::File_Tokens> tokenized_files;
    for(const auto& f : settings.input_files) {
        tez::Lexer lexer{f};
        auto tokens = lexer.tokenize();
        if(lexer.errors_reported()) {
            return 1;
        }
        tokenized_files.push_back(tokens);
    }
   
    tez::Ast ast;
    for(const auto& file : tokenized_files) {
        tez::Parser parser{file};
        ast.push_back(parser.parse());
        if(parser.errors_reported()) {
            return 2;
        }
    }

    //tez::Semantic_Analyzer analyzer{};
    //analyser.analyse(ast);
    //if(analyser.errors_reported()) {
    //    return 3;
    //}

    if(settings.use_llvm) {
        tez::LLVM_Codegen_Data codegen_data{};
        for([[maybe_unused]]const auto& file : ast) {
        }
        codegen_data.module->print(llvm::errs(), nullptr);
    } else {
        tez::C_Codegen_Data codegen_data;
        std::stringstream c_source;
        c_source <<
                "#include<stdio.h>\n"
                "#include<stdlib.h>\n"
                "#include<stdint.h>\n"
                "typedef char bool;\n"
                "typedef unsigned int uint;\n"
                "typedef int8_t int8;\n"
                "typedef int16_t int16;\n"
                "typedef int32_t int32;\n"
                "typedef int64_t int64;\n"
                "typedef uint8_t uint8;\n"
                "typedef uint16_t uint16;\n"
                "typedef uint32_t uint32;\n"
                "typedef uint64_t uint64;\n"
                "typedef float float32;\n"
                "typedef double float64;\n";
        for(const auto& f : ast) {
            c_source << f.generate_c(codegen_data) << '\n';
        }
        std::ofstream of{settings.output_file};
        auto tmp = c_source.str();
        of << tmp;
    }
    return 0;
}

int main(int argc, char* argv[]) {
#if TEZ_DEBUG
    auto start = std::chrono::steady_clock::now();
#endif

    if(argc < 2) {
        print_help();
        return 0;
    }

    std::vector<std::string> args(argv + 1, argv + argc);
    Compilation_Settings     settings;
    if(!parse_command_line_arguments(args, settings)) {
        return 1;
    }

    auto return_code = compile(settings);
    if(return_code) {
        // TODO: Count errors :)
        std::cout << "<> errors reported.\n";
    } else {
        std::cout << "No errors reported.\n";
    }

#if TEZ_DEBUG
    auto end = std::chrono::steady_clock::now();
    auto elapsed
            = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    auto seconds = elapsed.count() / 1000;
    auto milliseconds = elapsed.count() % 1000;
    std::cout << "Compilation time: " << seconds << '.' << milliseconds
            << "s.\n";
#endif

    return return_code;
}

