#ifndef FIBO_H
#define FIBO_H

#include <string>
#include <vector>
#include <ostream>
#include<iostream>
#include<string>


class Fibo {
public:
    /* KONSTRUKTORY */

    Fibo();

    explicit Fibo(const std::string &str);

    explicit Fibo(const char *str);

    Fibo(const Fibo &f);

    Fibo(const long long x);

    Fibo(const long x);

    Fibo(const unsigned x);

    Fibo(const unsigned long x);

    Fibo(const int x);

    Fibo(const unsigned long long x);

    Fibo(bool x) = delete;

    Fibo(char x) = delete;

    /* OPERATORY < i == */

    friend bool operator<(const Fibo &a, const Fibo &b);

    friend bool operator==(const Fibo &a, const Fibo &b);

    friend bool operator!=(const Fibo &a, const Fibo &b);

    /* OPERATORY x= */

    Fibo &operator+=(const Fibo &f);

    Fibo &operator&=(const Fibo &f);

    Fibo &operator|=(const Fibo &f);

    Fibo &operator^=(const Fibo &f);

    Fibo &operator<<=(size_t n);

    [[nodiscard]] size_t length() const;

    /* POZOSTAŁE OPERATORY */

    const Fibo operator+(const Fibo &f) const;

    const Fibo operator&(const Fibo &f) const;

    const Fibo operator|(const Fibo &f) const;

    const Fibo operator^(const Fibo &f) const;

    const Fibo operator<<(const int x) const;

    Fibo &operator=(const Fibo &f);

    friend std::ostream &operator<<(std::ostream &stream, const Fibo &lhs);

private:

    std::vector<bool> value;

    void normalize();

    void normalize(size_t i);

    void solve_double_positive(size_t pos);

    void clean_back();

    std::string create_string(long long x);
};

const Fibo &Zero();

const Fibo &One();

#endif /* FIBO_H */