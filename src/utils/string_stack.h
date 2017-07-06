#ifndef TEZ_STRING_STACK_H
#define TEZ_STRING_STACK_H

#include <string>
#include <vector>

namespace tez {

template <typename T = std::string>
class String_Stack {
public:
    // Append new element to the top.
    void append(std::string&& s);
    // Remove top element.
    void pop();
    // Remove all elements.
    void clear();
    // Get all elements converted to string.
    std::string get();
    // Set new delimiter.
    void set_delimiter(char delim);
    // .
    void use_trailing_delimiter(bool value);

private:
    void generate_string();

    char           _delimiter = '_';
    bool           _use_trailing_delim = false;
    std::vector<T> _parts;
    std::string    _generated;
};

template <typename T>
void String_Stack<T>::append(std::string&& s) {
    _parts.push_back(s);
    generate_string();
}

template <typename T>
void String_Stack<T>::pop() {
    _parts.pop_back();
    generate_string();
}

template <typename T>
void String_Stack<T>::clear() {
    _parts.clear();
    generate_string();
}

template <typename T>
std::string String_Stack<T>::get() {
    return _generated;
}

template <typename T>
void String_Stack<T>::set_delimiter(char delim) {
    _delimiter = delim;
}

template <typename T>
void String_Stack<T>::use_trailing_delimiter(bool value) {
    _use_trailing_delim = value;
}

template <typename T>
void String_Stack<T>::generate_string() {
    _generated.clear();
    if(_parts.size() == 0) {
        return;
    }
    for(const auto& part : _parts) {
        _generated += part + _delimiter;
    }
    if(!_use_trailing_delim) {
        _generated.erase(_generated.size() - 1);
    }
}

}

#endif //TEZ_STRING_STACK_H
