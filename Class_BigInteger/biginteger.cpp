#include "biginteger.h"

BigInteger operator""_bi(unsigned long long a) {
    BigInteger help(a);
    return help;
}

BigInteger operator""_bi(const char* st, std::size_t /*unused*/) {
    BigInteger help(st);
    return help;
}

BigInteger operator+(const BigInteger& dig1, const BigInteger& dig2) {
    BigInteger help = dig1;
    help += dig2;
    return help;
}

BigInteger operator-(const BigInteger& dig1, const BigInteger& dig2) {
    BigInteger help = dig1;
    help -= dig2;
    return help;
}

BigInteger operator*(const BigInteger& dig1, const BigInteger& dig2) {
    BigInteger help = dig1;
    help *= dig2;
    return help;
}

BigInteger operator/(const BigInteger& dig1, const BigInteger& dig2) {
    BigInteger help = dig1;
    help /= dig2;
    return help;
}

BigInteger operator%(const BigInteger& dig1, const BigInteger& dig2) {
    BigInteger help = dig1;
    help %= dig2;
    return help;
}

bool compare(const BigInteger& dig1, const BigInteger& dig2,
             bool isReverseCorner) {
    if (dig1.len() < dig2.len()) {
        return isReverseCorner;
    }
    if (dig1.len() > dig2.len()) {
        return !isReverseCorner;
    }
    for (int i = dig1.len() - 1; i >= 0; i--) {
        if (dig1[i] > dig2[i]) {
            return !isReverseCorner;
        }
        if (dig1[i] < dig2[i]) {
            return isReverseCorner;
        }
    }
    return false;
}

bool operator<(const BigInteger& dig1, const BigInteger& dig2) {
    if (&dig1 == &dig2) {
        return false;
    }
    if (dig1.isNegative && !dig2.isNegative) {
        return true;
    }
    if (!dig1.isNegative && dig2.isNegative) {
        return false;
    }
    if (dig1.isNegative && dig2.isNegative) {
        return compare(dig1, dig2, false);
    }
    return compare(dig1, dig2, true);
}

bool operator>(const BigInteger& dig1, const BigInteger& dig2) {
    return (dig2 < dig1);
}

bool operator==(const BigInteger& dig1, const BigInteger& dig2) {
    if (&dig1 == &dig2) {
        return true;
    }
    if (dig1.len() != dig2.len() ||
        dig1.is_number_negative() != dig2.is_number_negative()) {
        return false;
    }
    for (int i = dig1.len() - 1; i >= 0; i--) {
        if (dig1[i] != dig2[i]) {
            return false;
        }
    }
    return true;
}

bool operator!=(const BigInteger& dig1, const BigInteger& dig2) {
    return !(dig2 == dig1);
}

bool operator<=(const BigInteger& dig1, const BigInteger& dig2) {
    return !(dig1 > dig2);
}

bool operator>=(const BigInteger& dig1, const BigInteger& dig2) {
    return !(dig1 < dig2);
}

std::istream& operator>>(std::istream& in, BigInteger& dig) {
    string st;
    in >> st;
    dig = BigInteger(st);
    return in;
}

std::ostream& operator<<(std::ostream& out, const BigInteger& dig) {
    out << dig.toString();
    return out;
}

BigInteger::BigInteger() {
    digits.push_back(0);
    isNegative = false;
}

BigInteger::BigInteger(int numerator) {
    if (numerator == 0) {
        digits.push_back(0);
    }
    (numerator < 0 ? numerator = -numerator
                   : static_cast<int>(isNegative = false));
    while (numerator > 0) {
        digits.push_back(numerator % mod);
        numerator /= mod;
    }
}

BigInteger::BigInteger(const string& st) {
    long long id = st[0] == '-' ? 1 : 0;
    isNegative = (id == 1);
    for (int i = st.size() - 1; i >= id; i -= szm) {
        string hp;
        for (int j = i; j >= max(id, i - szm + 1); j--) {
            hp += st[j];
        }
        std::reverse(hp.begin(), hp.end());
        digits.push_back(std::stoi(hp));
    }
    deleteZero(false);
    checkZero();
}

BigInteger::BigInteger(const char* st) {
    long long id = st[0] == '-' ? 1 : 0;
    isNegative = (id == 1);
    for (int i = strlen(st) - 1; i >= id; i -= szm) {
        string hp;
        for (int j = i; j >= max(id, i - szm + 1); j--) {
            hp += st[j];
        }
        std::reverse(hp.begin(), hp.end());
        digits.push_back(std::stoi(hp));
    }
    deleteZero(false);
    checkZero();
}

string BigInteger::toString() const {
    string st = (isNegative && digits[len() - 1] != 0) ? "-" : "";
    int size = digits.size() - 1;
    int sizeThis = len() - 1;
    for (; size >= 0; size--) {
        if (size != sizeThis) {
            string hp = std::to_string(digits[size]);
            int sizeHelp = hp.size();
            for (int i = 0; i < szm - sizeHelp; i++) {
                st += '0';
            }
            st += hp;
        } else {
            st += std::to_string(digits[size]);
        }
    }
    return st;
}

BigInteger::operator bool() const {
    return digits[digits.size() - 1] != 0;
}

BigInteger& BigInteger::operator+=(const BigInteger& dig) {
    checkZero();
    if (isNegative == dig.isNegative) {
        long long change = 0;
        size_t lenDigits = dig.digits.size();
        size_t maxSize = std::max(digits.size(), lenDigits);
        for (size_t i = digits.size(); i < maxSize; i++) {
            digits.push_back(0);
        }
        for (size_t i = 0; i < digits.size(); i++) {
            if (i < lenDigits) {
                digits[i] += dig[i];
            }
            digits[i] += change;
            change = digits[i] / mod;
            digits[i] %= mod;
        }
        if (change != 0) {
            digits.push_back(change);
        }
    } else {
        BigInteger help1 = *this;
        BigInteger help2 = dig;
        help1.isNegative = false;
        help2.isNegative = false;
        bool f = false;
        if (help1 <= help2) {
            std::swap(help1, help2);
            f = true;
        }
        long long needSubtract = 0;
        for (size_t i = 0; (i < help2.len() || (needSubtract != 0)); i++) {
            if (i < help2.len()) {
                needSubtract += help2[i];
            }
            help1.digits[i] -= needSubtract;
            needSubtract = 0;
            if (help1.digits[i] < 0) {
                help1.digits[i] += mod;
                needSubtract = 1;
            }
        }
        digits = help1.digits;
        if (f) {
            isNegative = (!(isNegative || !dig.is_number_negative()));
        }
        checkZero();
        deleteZero(false);
    }
    return *this;
}

BigInteger& BigInteger::operator-=(const BigInteger& dig) {
    (*this) += -dig;
    return (*this);
}

BigInteger& BigInteger::operator*=(const BigInteger& dig) {
    vector<long long> hp(dig.len() + len() + 1, 0);
    for (size_t i = 0; i < dig.len(); i++) {
        long long add = 0;
        for (size_t j = 0; j < len(); j++) {
            hp[i + j] += (dig[i] * digits[j] + add);
            add = hp[i + j] / mod;
            hp[i + j] %= mod;
        }
        hp[i + len()] += add;
    }
    while (true) {
        int t = hp.back();
        hp[hp.size() - 1] %= mod;
        t = t / mod;
        if (t == 0) {
            break;
        }
        hp.push_back(t);
    }
    digits = hp;
    isNegative = (isNegative != dig.isNegative);
    deleteZero(false);
    checkZero();
    return *this;
}

BigInteger& BigInteger::operator/=(const BigInteger& dig) {
    if (len() < dig.len()) {
        *this = 0;
        return *this;
    }
    BigInteger ans;
    BigInteger help = 0;
    BigInteger dg = dig;
    dg.isNegative = false;
    for (int i = len() - 1; i >= 0; i--) {
        help *= mod;
        help += digits[i];
        int l = 0, r = mod;
        while (r - l > 1) {
            int downNumber = (r + l) / 2;
            if (dg * downNumber <= help) {
                l = downNumber;
            } else {
                r = downNumber;
            }
        }
        ans *= mod;
        ans += l;
        help -= l * dg;
    }
    *this = ans;
    if (isNegative != dig.isNegative) {
        isNegative = true;
    }
    checkZero();
    return *this;
}

BigInteger& BigInteger::operator%=(const BigInteger& dig) {
    *this -= (*this / dig) * dig;
    return *this;
}

BigInteger BigInteger::operator-() const {
    BigInteger help = *this;
    help.isNegative = (digits[len() - 1] == 0 ? false : !isNegative);
    return help;
}

BigInteger& BigInteger::operator++() {
    (*this) += 1;
    return (*this);
}

BigInteger BigInteger::operator++(int) {
    BigInteger help = *this;
    ++(*this);
    return help;
}

BigInteger& BigInteger::operator--() {
    (*this) += -1;
    return *this;
}

BigInteger BigInteger::operator--(int) {
    BigInteger help = (*this);
    --(*this);
    return help;
}

// help part of code

bool BigInteger::is_number_negative() const {
    return isNegative;
}

size_t BigInteger::len() const {
    return digits.size();
}

const long long& BigInteger::operator[](long long id) const {
    return digits[id];
}

void BigInteger::deleteZero(bool need_to_reverse) {
    if (need_to_reverse) {
        std::reverse(digits.begin(), digits.end());
    }
    int id = digits.size() - 1;
    while (id >= 0 && digits[id] == 0) {
        digits.pop_back();
        id--;
    }
    if (digits.empty()) {
        isNegative = false;
        digits.push_back(0);
    }
}

void BigInteger::checkZero() {
    if (digits[len() - 1] == 0) {
        isNegative = false;
    }
}

void BigInteger::clear() {
    isNegative = false;
    digits.clear();
}

Rational::Rational() : numerator(0), downNumber(1) {}

Rational::Rational(const BigInteger& x)
    : isNegative(x < 0), numerator(x < 0 ? -x : x), downNumber(1) {}

Rational::Rational(int x)
    : isNegative(x < 0), numerator(x < 0 ? -x : x), downNumber(1) {}

Rational::Rational(long long x, long long y)
    : isNegative(x * y < 0), numerator(std::abs(x)), downNumber(std::abs(y)) {
    gcd();
}

Rational::Rational(const BigInteger& dig1, const BigInteger& dig2)
    : isNegative(dig1.is_number_negative() != dig2.is_number_negative()),
      numerator((dig1.is_number_negative() ? -dig1 : dig1)),
      downNumber(dig2.is_number_negative() ? -dig2 : dig2) {
    gcd();
}

Rational& Rational::operator+=(const Rational& x) {
    if (isNegative == x.isNegative) {
        *this =
            createNumber(numerator * x.downNumber + downNumber * x.numerator,
                         x.downNumber * downNumber, isNegative);
    } else {
        Rational help(numerator * x.downNumber - downNumber * x.numerator,
                      x.downNumber * downNumber);
        if (isNegative && !help.isNegative) {
            help.isNegative = true;
        }
        *this = help;
    }
    return *this;
}

Rational& Rational::operator-=(const Rational& x) {
    *this += createNumber(x.numerator, x.downNumber, !x.isNegative);
    return *this;
}

Rational& Rational::operator*=(const Rational& x) {
    *this = createNumber(x.numerator * numerator, downNumber * x.downNumber,
                         (x.isNegative != isNegative));
    return *this;
}

Rational& Rational::operator/=(const Rational& x) {
    *this *= createNumber(x.downNumber, x.numerator,
                          (x.is_number_negative() && numerator != 0));
    return *this;
}

Rational Rational::operator-() {
    return createNumber(numerator, downNumber, (!isNegative && numerator != 0));
}

Rational::operator double() const {
    return std::stod(asDecimal(15));
}

string Rational::toString() const {
    string ans = (isNegative ? "-" : "");
    ans += numerator.toString();
    if (downNumber != 1) {
        ans += '/';
        ans += downNumber.toString();
    }
    return ans;
}

string Rational::asDecimal(int count) const {
    string ans = (isNegative ? "-" : "");
    ans += (numerator / downNumber).toString();
    if (numerator % downNumber == 0) {
        return ans;
    }
    ans += '.';
    BigInteger help(numerator % downNumber * mod);
    string st;
    int countIter = count / szm + (count % szm != 0 ? 1 : 0);
    for (int i = 0; i < countIter; i++) {
        string tp = (help / downNumber).toString();
        size_t addzero = szm - tp.size();
        for (size_t x = 0; x < addzero; x++) {
            st += '0';
        }
        st += tp;
        help %= downNumber;
        help *= mod;
    }
    for (int i = 0; i < count; i++) {
        ans += st[i];
    }
    return ans;
}

bool Rational::is_number_negative() const {
    return isNegative;
}

void Rational::gcd() {
    BigInteger d1 = numerator;
    BigInteger d2 = downNumber;
    while (d2) {
        BigInteger help = d2;
        d2 = (d1 %= d2);
        d1 = help;
    }
    numerator /= d1;
    downNumber /= d1;
    if (numerator == 0) {
        isNegative = false;
    }
}
Rational Rational::createNumber(const BigInteger& x, const BigInteger& y,
                                bool isNegative) {
    Rational newRational(x, y);
    newRational.isNegative = isNegative;
    return newRational;
}

Rational operator+(const Rational& x1, const Rational& x2) {
    Rational help = x1;
    help += x2;
    return help;
}

Rational operator-(const Rational& x1, const Rational& x2) {
    Rational help = x1;
    help -= x2;
    return help;
}
Rational operator*(const Rational& x1, const Rational& x2) {
    Rational help = x1;
    help *= x2;
    return help;
}
Rational operator/(const Rational& x1, const Rational& x2) {
    Rational help = x1;
    help /= x2;
    return help;
}

bool operator<(const Rational& x1, const Rational& x2) {
    if (x1.is_number_negative() != x2.is_number_negative()) {
        return x1.is_number_negative();
    }
    if (x1.numerator * x2.downNumber < x2.numerator * x1.downNumber) {
        return !x1.is_number_negative();
    }
    return x1.is_number_negative();
}

bool operator>(const Rational& x1, const Rational& x2) {
    return x2 < x1;
}

bool operator==(const Rational& x1, const Rational& x2) {
    if (x1.is_number_negative() != x2.is_number_negative()) {
        return false;
    }
    return x1.numerator * x2.downNumber == x1.downNumber * x2.numerator;
}

bool operator!=(const Rational& x1, const Rational& x2) {
    return !(x1 == x2);
}

bool operator<=(const Rational& x1, const Rational& x2) {
    return !(x1 > x2);
}

bool operator>=(const Rational& x1, const Rational& x2) {
    return !(x1 < x2);
}
