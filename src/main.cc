#include <iostream>

#include "parser.h"

void print_usage() {
    std::cout << "Usage:\n"
            "waywardc <input_file>\n";
}

bool compile(const std::string& working_path, const std::string& filename) {
    wayward::lexer lexer{working_path, filename};
    auto tokens = lexer.tokenize();
    if(lexer.errors_reported()) {
        return false;
    }

    wayward::parser parser{tokens};
    auto ast = parser.parse();
    if(parser.errors_reported()) {
        return false;
    }

    return true;
}

int main(int argc, char* argv[]) {
    if(argc < 2) {
        print_usage();
    } else {
        std::vector<std::string> input_files;
        std::string output_file = "a.exe";
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
            compile("./", argv[1]);
        } else {
            std::string filename = working_path.substr(pos);
            working_path = working_path.substr(0, pos + 1);
            compile(working_path, filename);
        }
    }
    return 0;
}

