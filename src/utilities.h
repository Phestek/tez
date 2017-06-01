#ifndef WAYWARD_UTIL_H
#define WAYWARD_UTIL_H

#include <iostream>

template<class... Args>
[[noreturn]] inline void print_error_and_exit(Args&&... args) {
    std::cout << "error: ";
    
    (void)(int[]){0, (void(std::cout << std::forward<Args>(args)), 0)...};

    std::cout << std::endl;

    exit(EXIT_FAILURE);
}

template<class... Args>
inline void error(Args&&... args) {
    std::cout << "error: ";

    (void)(int[]){0, (void(std::cout << std::forward<Args>(args)), 0)...};

    std::cout << std::endl;
}

#endif // WAYWARD_UTIL_H