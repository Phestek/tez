#include <iostream>
#include <fstream>
#include <vector>

#include "lexer.h"
#include "parser.h"
#include "c_code_gen.h"

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
    if(argc < 2) {
        print_usage();
    } else {
        std::vector<std::string> input_files;
        std::string output_file = "output.c";
        std::vector<std::string> command_line_arguments{argv + 1, argv + argc};
        try {
            for(std::size_t i = 0; i < command_line_arguments.size(); ++i) {
                if(command_line_arguments.at(i) == "-o") {
                    output_file = command_line_arguments.at(++i);
                } else {
                    input_files.push_back(command_line_arguments.at(i));
                }
            }
        } catch(const std::out_of_range& e) {
            std::cerr << "Unexpected end of input parameters. Exiting.\n";
            return 1;
        }

        std::cout << "Input files: ";
        for(auto& f : input_files) {
            std::cout << f << ' ';
        }
        std::cout << '\n';
        std::cout << "Output file: " << output_file << '\n';
        
        compile(input_files, output_file);
    }
    return 0;
}

