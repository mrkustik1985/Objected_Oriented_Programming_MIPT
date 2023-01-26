#include <algorithm>
#include <cstring>
#include <iostream>

class String {
  public:
    String(char x);
    String(const char* a);
    String(size_t n, char c);
    String();
    String(const String& str);
    String& operator=(const String& str);
    char& operator[](int id);
    char operator[](int id) const;
    size_t length() const;
    size_t size() const;
    size_t capacity() const;
    void push_back(char x);
    void pop_back();
    char& front();
    const char& front() const;
    char& back();
    const char& back() const;
    bool empty();
    void clear();
    void shrink_to_fit();
    String& operator+=(const String& str);
    size_t find(const String& str) const;
    size_t rfind(const String& str) const;
    String substr(size_t start, size_t count) const;
    char* data();
    const char* data() const;
    ~String();

  private:
    size_t strsize;
    size_t maxCountElement;
    char* string;
    void copyString(int start, const String& str, size_t delta);
    void changeCountElements(size_t newcapacity);
    int findString(int start, int finish, int step, int sign,
                   const String& str) const;
};

bool operator<(const String& str1, const String& str2);

bool operator>(const String& str1, const String& str2);

bool operator<=(const String& str1, const String& str2);

bool operator>=(const String& str1, const String& str2);

bool operator==(const String& str1, const String& str2);

bool operator!=(const String& str1, const String& str2);

String operator+(const String& str1, const String& str2);

std::ostream& operator<<(std::ostream& out, const String& str);

std::istream& operator>>(std::istream& in, String& str);
