#include "fibo.h"
#include <iostream>
#include <vector>
#include <cassert>
#include <cstring>
#include <ostream>

using std::vector;

// KONSTRUKTORY.

Fibo::Fibo() {
    this->value = vector<bool>(1, false);
}

Fibo::Fibo(const std::string &str) {
    this->value = vector<bool>(str.size(), false);
    for (size_t i = 0; i < str.size(); i++) {
        assert(str[str.size() - 1 - i] == '1' ||
               str[str.size() - 1 - i] == '0');
        this->value[i] = (str[str.size() - 1 - i] == '1');
    }
    this->normalize();
}

Fibo::Fibo(const Fibo &second) {
    value = second.value;
}

Fibo::Fibo(const char *cstr) : Fibo(std::string(cstr)) {};

Fibo::Fibo(const long long x) : Fibo(create_string(x)) {};

Fibo::Fibo(const long x) : Fibo(create_string(x)) {};

Fibo::Fibo(const unsigned x) : Fibo(create_string(x)) {};

Fibo::Fibo(const unsigned long x) : Fibo(create_string(x)) {};

Fibo::Fibo(const int x) : Fibo(create_string(x)) {};

Fibo::Fibo(const unsigned long long x) : Fibo(create_string(x)) {};

// OPERATORY i reszta.

[[nodiscard]] size_t Fibo::length() const {
    return value.size();
}

Fibo &Fibo::operator+=(const Fibo &second) {
    //Tworzymy bufor długości 10 zerowych fibitów.
    value.resize(std::max(length(), second.length()) + 10, false);
    //Liczby dodajemy od najmniej znaczącego fibitu.
    for (size_t pos = 0; pos < length(); pos++) {
        //Jeżeli w drugiej liczbie nie ma już fibitów nie musimy dodawać dalej.
        if (pos < second.length()) {
            //Dodawanie staje się problematyczne, gdy na tej samej pozycji fibity są równe dodatnie.
            if (value[pos] && second.value[pos]) {
                solve_double_positive(pos);
            } else {
                value[pos] = value[pos] || second.value[pos];
            }
        }
    }
    normalize();
    return *this;
}

Fibo &Fibo::operator&=(const Fibo &second) {
    for (size_t i = 0; i < this->value.size(); i++) {
        if (second.value.size() <= i) this->value[i] = false;
        else this->value[i] = this->value[i] & second.value[i];
    }
    this->normalize();
    return *this;
}


Fibo &Fibo::operator|=(const Fibo &second) {
    this->value.resize(std::max(this->length(), second.length()), false);
    for (size_t pos = 0; pos < length(); pos++) {
        this->value[pos] = this->value[pos] | second.value[pos];
    }
    this->normalize();
    return *this;
}

Fibo &Fibo::operator<<=(size_t n) {
    value.resize(length() + n, false);
    for (size_t pos = length() - 1; pos >= n; pos--) {
        value[pos] = value[pos - n];
    }
    for (size_t pos = 0; pos < n; pos++) {
        value[pos] = false;
    }
    return *this;
}

Fibo &Fibo::operator^=(const Fibo &second) {
    value.resize(std::max(length(), second.length()), false);
    for (size_t pos = 0; pos < length(); pos++) {
        value[pos] = value[pos] ^ second.value[pos];
    }
    normalize();

    return *this;
}


bool operator<(const Fibo &first, const Fibo &second) {
    if (first.length() != second.length()) {
        return first.length() < second.length();
    } else {
        for (size_t pos = first.length(); pos > 0; pos--) {
            if (first.value[pos] != second.value[pos]) {
                return first.value[pos] == false;
            }
        }
        if (first.value[0] != second.value[0]) return first.value[0] == false;
        return false;
    }
}

bool operator==(const Fibo &first, const Fibo &second) {
    return first.value == second.value;
}

bool operator!=(const Fibo &first, const Fibo &second) {
    return !(first == second);
}

bool operator<=(const Fibo &first, const Fibo &second) {
    return !(first == second || first < second);
}

bool operator>(const Fibo &first, const Fibo &second) {
    return !(first <= second);
}

bool operator>=(const Fibo &first, const Fibo &second) {
    return !(first < second);
}

Fibo &Fibo::operator=(const Fibo &second) {
    value.resize(second.length(), false);
    *this |= second;
    return *this;
}

const Fibo Fibo::operator+(const Fibo &f) const {
    return Fibo(*this) += f;
}

const Fibo Fibo::operator&(const Fibo &f) const {
    return Fibo(*this) &= f;
}

const Fibo Fibo::operator|(const Fibo &f) const {
    return Fibo(*this) |= f;
}

const Fibo Fibo::operator^(const Fibo &f) const {
    return Fibo(*this) ^= f;
}

const Fibo Fibo::operator<<(const int x) const {
    return Fibo(*this) <<= x;
}


std::ostream &operator<<(std::ostream &stream, const Fibo &f) {
    for (auto it = f.value.rbegin(); it != f.value.rend(); it++) {
        stream << *it;
    }
    return stream;
}


const Fibo &Zero() {
    static const Fibo *f = new Fibo();
    return *f;
}

const Fibo &One() {
    static const Fibo *f = new Fibo("1");
    return *f;
}


/* PRYWATNE */

std::string Fibo::create_string(long long x) {
    std::string s = "";
    long long fib0 = 0, fib1 = 1, fib2 = 1;
    while (fib2 < x) {
        long long fib3 = fib2 + fib1;
        fib0 = fib1;
        fib1 = fib2;
        fib2 = fib3;
    }
    while (fib1 > 0) {
        if (fib2 <= x) {
            s += '1';
            x -= fib2;
        } else {
            s += '0';
        }
        long long fib3 = fib1 - fib0;
        fib2 = fib1;
        fib1 = fib0;
        fib0 = fib3;
    }
    return s;
}

void Fibo::clean_back() {
    //Kasuje nieistotne zera z końca wektora.
    while (length() > 1 && !value[length() - 1]) {
        value.pop_back();
    }
}


void Fibo::normalize() {
    //Zakladamy, że mamy zapis fibitów w wektorze od najmniej istotnego.
    value.resize(length() + 2, false);
    for (size_t i = length(); i > 0; i--) {
        normalize(i - 1);
    }
    clean_back();
}

void Fibo::normalize(size_t i) {
    for (size_t pos = i;
         pos < length() - 1 && value[pos] && value[pos + 1]; pos += 2) {
        value[pos] = false;
        value[pos + 1] = false;
        value[pos + 2] = true;
    }
}

void Fibo::solve_double_positive(size_t pos) {
    //Skaczemy co dwa fibity korzystając z zależności 2*f(n)=f(n-2)+f(n+1).
    for (; pos < length(); pos += 2) {
        if (!value[pos]) {
            value[pos] = true;
            pos = length();
        } else if (value[pos + 1]) {
            value[pos] = false;
            value[pos + 1] = false;
            value[pos + 2] = true;
            pos = length();
        } else {
            value[pos] = false;
            value[pos + 1] = true;
            if (pos > 1) {
                pos -= 2;
            } else {
                if (pos == 1) {
                    if (value[0]) {
                        value[pos] = true;
                        value[0] = false;
                    } else {
                        value[0] = true;
                    }
                }
                pos = length();
            }
        }
    }
}
