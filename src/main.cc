#include <iostream>
#include <fstream>

#include "lexer.h"
#include "parser.h"
#include "c_code_gen.h"

void print_usage() {
    std::cout << "Usage:\n"
            "waywardc <input_file>\n";
}

bool compile(const std::string& working_path, const std::string& input_file,
        const std::string& output_file) {
    wayward::lexer lexer{working_path, input_file};
    auto tokens = lexer.tokenize();
    if(lexer.errors_reported()) {
        return false;
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

        std::string working_path = argv[1];
        auto pos = working_path.find_last_of("/\\");
        if(pos >= working_path.length()) {
            compile("./", argv[1], output_file);
        } else {
            std::string filename = working_path.substr(pos);
            working_path = working_path.substr(0, pos + 1);
            compile(working_path, filename, output_file);
        }
    }
    return 0;
}

