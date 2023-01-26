

#include <algorithm>
#include <array>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

using std::array;
using std::max;
using std::string;
using std::vector;

class BigInteger;

bool operator<=(const BigInteger& /*dig1*/, const BigInteger& /*dig2*/);
BigInteger operator*(const BigInteger& /*dig1*/, const BigInteger& /*dig2*/);
BigInteger operator/(const BigInteger& /*dig1*/, const BigInteger& /*dig2*/);

const long long mod = 1e9;
const long long szm = 9;

class BigInteger {
  public:
    BigInteger();
    BigInteger(int n);
    BigInteger(const string& st);
    BigInteger(const char* st);

    string toString() const;

    BigInteger& operator+=(const BigInteger& dig);
    BigInteger& operator-=(const BigInteger& dig);
    BigInteger& operator*=(const BigInteger& dig);
    BigInteger& operator/=(const BigInteger& dig);
    BigInteger& operator%=(const BigInteger& dig);

    explicit operator bool() const;
    BigInteger operator-() const;
    BigInteger& operator++();
    BigInteger operator++(int);
    BigInteger& operator--();
    BigInteger operator--(int);

    // help part of code
    bool is_number_negative() const;
    size_t len() const;
    const long long& operator[](long long id) const;

  private:
    friend bool operator<(const BigInteger& /*dig1*/,
                          const BigInteger& /*dig2*/);
    vector<long long> digits;
    bool isNegative = true;
    void deleteZero(bool need_to_reverse);
    void checkZero();
    void clear();
};

class Rational {
  public:
    Rational();
    Rational(const BigInteger& x);
    Rational(int x);
    Rational(long long x, long long y);
    Rational(const BigInteger& dig1, const BigInteger& dig2);

    Rational& operator+=(const Rational& x);
    Rational& operator-=(const Rational& x);
    Rational& operator*=(const Rational& x);
    Rational& operator/=(const Rational& x);
    Rational operator-();
    explicit operator double() const;

    string toString() const;
    string asDecimal(int count) const;

    bool is_number_negative() const;

  private:
    friend bool operator<(const Rational& /*x1*/, const Rational& /*x2*/);
    friend bool operator==(const Rational& /*x1*/, const Rational& /*x2*/);
    bool isNegative = false;

    BigInteger numerator;
    BigInteger downNumber;

    void gcd();
    static Rational createNumber(const BigInteger& x, const BigInteger& y,
                                 bool isNegative);
};

BigInteger operator""_bi(unsigned long long a);

BigInteger operator""_bi(const char* /*st*/, std::size_t /*unused*/);

BigInteger operator+(const BigInteger& dig1, const BigInteger& dig2);

BigInteger operator-(const BigInteger& dig1, const BigInteger& dig2);

BigInteger operator%(const BigInteger& dig1, const BigInteger& dig2);

bool compare(const BigInteger& dig1, const BigInteger& dig2,
             bool isReverseCorner);

bool operator<(const BigInteger& dig1, const BigInteger& dig2);

bool operator>(const BigInteger& dig1, const BigInteger& dig2);

bool operator==(const BigInteger& dig1, const BigInteger& dig2);

bool operator!=(const BigInteger& dig1, const BigInteger& dig2);

bool operator>=(const BigInteger& dig1, const BigInteger& dig2);

std::istream& operator>>(std::istream& in, BigInteger& dig);

std::ostream& operator<<(std::ostream& out, const BigInteger& dig);

Rational operator+(const Rational& x1, const Rational& x2);

Rational operator-(const Rational& x1, const Rational& x2);

Rational operator*(const Rational& x1, const Rational& x2);

Rational operator/(const Rational& x1, const Rational& x2);

bool operator<(const Rational& x1, const Rational& x2);

bool operator>(const Rational& x1, const Rational& x2);

bool operator==(const Rational& x1, const Rational& x2);

bool operator!=(const Rational& x1, const Rational& x2);

bool operator<=(const Rational& x1, const Rational& x2);

bool operator>=(const Rational& x1, const Rational& x2);