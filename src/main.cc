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
    } else if(argc == 2) {
        std::string working_path = argv[1];
        auto pos = working_path.find_last_of("/\\");
        if(pos >= working_path.length()) {
            compile("./", argv[1]);
        } else {
            std::string filename = working_path.substr(pos);
            working_path = working_path.substr(0, pos + 1);
            compile(working_path, filename);
        }
    } else {
        std::cout << "More than one file compilation and switches are not "
                "supported yet.\n";
    }
    return 0;
}

