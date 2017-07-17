#ifndef TEZ_STRING_STACK_H
#define TEZ_STRING_STACK_H

#include <string>
#include <vector>

namespace tez {

template <typename T = std::string, typename Container = std::vector<T>>
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
    // Get number of elements on stack.
    std::size_t size() const;
    // Set new delimiter.
    void set_delimiter(char delim);
    // .
    void use_trailing_delimiter(bool value);

private:
    void generate_string();

    char           _delimiter = '_';
    bool           _use_trailing_delim = false;
    Container      _container;
    std::string    _generated;
};

template <typename T, typename Container>
void String_Stack<T, Container>::append(std::string&& s) {
    _container.push_back(std::move(s));
    generate_string();
}

template <typename T, typename Container>
void String_Stack<T, Container>::pop() {
    if(_container.size() == 0) {
        return;
    }
    _container.pop_back();
    generate_string();
}

template <typename T, typename Container>
void String_Stack<T, Container>::clear() {
    _container.clear();
    generate_string();
}

template <typename T, typename Container>
std::string String_Stack<T, Container>::get() {
    return _generated;
}

template <typename T, typename Container>
std::size_t String_Stack<T, Container>::size() const {
    return _container.size();
}

template <typename T, typename Container>
void String_Stack<T, Container>::set_delimiter(char delim) {
    _delimiter = delim;
}

template <typename T, typename Container>
void String_Stack<T, Container>::use_trailing_delimiter(bool value) {
    _use_trailing_delim = value;
}

template <typename T, typename Container>
void String_Stack<T, Container>::generate_string() {
    _generated.clear();
    if(_container.size() == 0) {
        return;
    }
    for(const auto& part : _container) {
        _generated += part + _delimiter;
    }
    if(!_use_trailing_delim) {
        _generated.erase(_generated.size() - 1);
    }
}

}

#endif //TEZ_STRING_STACK_H

