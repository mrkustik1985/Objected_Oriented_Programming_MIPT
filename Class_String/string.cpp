#include "string.h"
String::String(char x) : strsize{1}, maxCountElement{2}, string{new char[2]} {
    string[0] = x;
    string[1] = '\0';
}

String::String(const char* a)
    : strsize{strlen(a)},
      maxCountElement{strlen(a) + 1},
      string{new char[strlen(a) + 1]} {
    memcpy(string, a, strsize);
    string[strsize] = '\0';
}

String::String(size_t n, char c)
    : strsize{n}, maxCountElement{n + 1}, string{new char[n + 1]} {
    memset(string, c, n);
    string[strsize] = '\0';
}

String::String() : strsize{0}, maxCountElement{1}, string{new char[1]} {
    string[strsize] = '\0';
}

String::String(const String& str)
    : strsize{str.size()},
      maxCountElement{str.size() + 1},
      string{new char[str.size() + 1]} {
    memcpy(string, str.string, strsize + 1);
}

String& String::operator=(const String& str) {
    if (this == &str) {
        return *this;
    }
    if (str.size() + 1 > maxCountElement) {
        String help(str);
        std::swap(help.string, string);
        std::swap(help.maxCountElement, maxCountElement);
    } else {
        copyString(0, str, str.size());
    }
    return *this;
}

char& String::operator[](int id) {
    return string[id];
}

char String::operator[](int id) const {
    return string[id];
}

size_t String::length() const {
    return strsize;
}

size_t String::size() const {
    return strsize;
}

size_t String::capacity() const {
    return maxCountElement - 1;
}

void String::push_back(char x) {
    string[strsize] = x;
    if (strsize + 1 == maxCountElement) {
        changeCountElements(maxCountElement * 2);
    }
    string[++strsize] = '\0';
}

void String::pop_back() {
    string[--strsize] = '\0';
}

char& String::front() {
    return string[0];
}

const char& String::front() const {
    return string[0];
}

char& String::back() {
    return string[strsize - 1];
}

const char& String::back() const {
    return string[strsize - 1];
}

bool String::empty() {
    return strsize == 0;
}

void String::clear() {
    strsize = 0;
    string[0] = '\0';
}

void String::shrink_to_fit() {
    if (strsize + 1 == maxCountElement) {
        return;
    }
    changeCountElements(strsize + 1);
}

String& String::operator+=(const String& str) {
    if (strsize + str.size() + 1 > maxCountElement) {
        changeCountElements(strsize + str.size() + 1);
    }
    copyString(strsize, str, strsize + str.size());
    return *this;
}

size_t String::find(const String& str) const {
    return findString(0, strsize - str.size(), 1, 1, str);
}

size_t String::rfind(const String& str) const {
    return findString(strsize - str.size(), 1, -1, -1, str);
}

String String::substr(size_t start, size_t count) const {
    size_t new_sz = std::min(count, strsize - start - 1);
    String help(new_sz, '\0');
    memcpy(help.string, string + start, new_sz);
    return help;
}

char* String::data() {
    return string;
}

const char* String::data() const {
    return string;
}

String::~String() {
    delete[] string;
}

void String::copyString(int start, const String& str, size_t delta) {
    memcpy(string + start, str.data(), str.size());
    strsize = delta;
    string[strsize] = '\0';
}

void String::changeCountElements(size_t newcapacity) {
    maxCountElement = newcapacity;
    String help(maxCountElement, '\0');
    memcpy(help.string, string, strsize + 1);
    std::swap(string, help.string);
}

int String::findString(int start, int finish, int step, int sign,
                       const String& str) const {
    if (str.size() > strsize) {
        return strsize;
    }
    for (int i = start; i * sign <= finish; i += step) {
        if (memcmp(string + i, str.data(), str.size()) == 0) {
            return i;
        }
    }
    return strsize;
}

bool operator<(const String& str1, const String& str2) {
    return strcmp(str1.data(), str2.data()) < 0;
}

bool operator>(const String& str1, const String& str2) {
    return str2 < str1;
}

bool operator<=(const String& str1, const String& str2) {
    return !(str1 > str2);
}

bool operator>=(const String& str1, const String& str2) {
    return !(str1 < str2);
}

bool operator==(const String& str1, const String& str2) {
    if (str1.size() != str2.size()) {
        return false;
    }
    return strcmp(str1.data(), str2.data()) == 0;
}

bool operator!=(const String& str1, const String& str2) {
    return !(str1 == str2);
}

String operator+(const String& str1, const String& str2) {
    String help(str1);
    help += str2;
    return help;
}

std::ostream& operator<<(std::ostream& out, const String& str) {
    for (size_t i = 0; i < str.size(); i++) {
        out.put(str[i]);
    }
    return out;
}

std::istream& operator>>(std::istream& in, String& str) {
    str.clear();
    char x;
    in.get(x);
    while (((iscntrl(x)) == 0) && ((isspace(x)) == 0) && !(in.eof())) {
        str.push_back(x);
        in.get(x);
    }
    return in;
}
