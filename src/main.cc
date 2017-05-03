#include <iostream>
#include <fstream>

#include "parser.h"

void print_usage() {
    std::cout << "Usage:\n"
            "waywardc <input_file>\n";
}

// Get entire file content, including newlines.
// NOTE: leaves empty line on the end.
std::string read_file_content(const std::string& filename) {
    std::ifstream file{filename};
    if(!file.good()) {
        // TODO: Handle it.
        std::cerr << "Failed to open file \"" + filename + "\".\n";
        throw;
    }
    std::string content;
    while(!file.eof()) {
        std::string line;
        std::getline(file, line);
        if(line != "\n") {
            content += line + '\n';
        }
    }
    return content;
}

// Returns true if compilation was successful and false if there were any
// errors. TODO: What about warnings?
void compile(const std::string& input_file) {
    auto wayward_source = read_file_content(input_file);
    wayward::lexer lexer{wayward_source};
    auto tokens = lexer.tokenize();
    wayward::parser parser{tokens};
    auto ast = parser.parse();
}

int main(int argc, char* argv[]) {
    if(argc < 2) {
        print_usage();
    } else if(argc == 2) {
        // TODO: Validate file extension (*.wwd?).
        try {
            compile(argv[1]);
        } catch(const std::invalid_argument& e) {
            std::cout << e.what() << "\n";
            return 1;
        }
    } else {
        std::cout << "More than one file compilation and switches are not "
                "supported yet.\n";
    }
    return 0;
}

