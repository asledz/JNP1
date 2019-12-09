#ifndef JNP1_ZAD4_FIBIN_H
#define JNP1_ZAD4_FIBIN_H

#include <type_traits>
#include <cassert>
#include <typeinfo>
#include <iostream>

// Obsługa Var - konwertuje char* w jednoznaczny napis, sprawdza czy spełnia założenia zadania.
#define VarType unsigned long long

// valueOfChar(a) - zwraca kod znaku a.
// Zwraca 0, jeśli znak niepoprawny.
static constexpr unsigned long long valueOfChar(const char a) {
    if ('0' <= a && a <= '9') {
        return (a - '0' + 1);
    }
    if ('a' <= a && a <= 'z') {
        return (a - 'a' + 11);
    }
    if ('A' <= a && a <= 'Z') {
        return (a - 'A' + 11);
    }
    return 0;
}

// Var(str) - zwraca identyfikator zmiennej Var(str) typu VarType.
static constexpr VarType Var(const char* str) {
    unsigned long long res = 0;
    for (int i = 0; str[i] != '\0'; i++) {
        if (i >= 6) {
            throw std::invalid_argument("Var argument is too long.");
        }
        if (valueOfChar(str[i]) == 0) {
            throw std::invalid_argument("Var argument's characters are incorrect.");
        }
        res = res * 100 + valueOfChar(str[i]);
    }
    if (res == 0) {
        throw std::invalid_argument("Var argument is too short.");
    }
    return res;
}

// Obsługa typów liczbowych.
// x - wartość przechowywanej liczby.
template <typename ValueType, ValueType x>
struct Numeric {
    static constexpr ValueType value = x;
};

// Surowy typy boolowski True.
struct True {
};

// Surowy typy boolowski False.
struct False {
};

// Fib<n> - surowy typ reprezentujący liczbę Fibonacciego.
template <unsigned long long>
struct Fib {
};

// Literał Lit<T> - generyczny literał.
// T powinno być Fib<N> lub True/False.
template <typename, typename ValueType = uint16_t>
struct Lit {
};

// Surowe wyrażenie If<Condition, Then, Else>.
template <typename, typename, typename>
struct If {
};

// Surowe wyrażenie Let<Var, Value, Expression>.
template <VarType, typename, typename>
struct Let {
};

// Surowe odwołanie do zmiennej Ref<Var(str)>.
template <VarType>
struct Ref {
};

// Surowe porównanie Eq<Left, Right>.
template <typename, typename>
struct Eq;

// Surowe wyrażenie Lambda<Var, Body>.
template <VarType, typename>
struct Lambda {
};

// Wywołanie funkcji Invoke<Fun, Param>.
template <typename, typename>
struct Invoke {
};

// Obliczanie literałów logicznych.
template <>
struct Lit<True> {
};

template <>
struct Lit<False> {
};

// Obliczanie literałów liczbowych Fib.
template <unsigned long long N, typename ValueType>
struct Lit<Fib<N>, ValueType> {
    Numeric<ValueType, static_cast<ValueType>(Lit<Fib<N - 1>, ValueType>::result::value + Lit<Fib<N - 2>, ValueType>::result::value)> typedef result;
};

template <typename ValueType>
struct Lit<Fib<0>, ValueType> {
    Numeric<ValueType, static_cast<ValueType>(0)> typedef result;
};

template <typename ValueType>
struct Lit<Fib<1>, ValueType> {
    Numeric<ValueType, static_cast<ValueType>(1)> typedef result;
};

// ***
// Operacje arytmetyczne.
// ***

// Inc1<Arg> - specjalizacja dodawania, która zwiększa wartość Arg o Fib<1>.
template <typename>
struct Inc1 {
};

// Inc10<Arg> - specjalizacja dodawania, która zwiększa wartość Arg o Fib<10>.
template <typename>
struct Inc10 {
};

// Sum<Args...> - suma argumentów Args.
template <typename...>
struct Sum {
};

// Zakres środowiska dla Lambdy.
// Scope<Fun, EnvScope>
template <typename, typename>
struct Scope {
};

// ***
// Tworzenie Environment - przypisuje zmienne od ich nazw, oblicza się w czasie kompilacji.
// ***

// Puste środowisko.
struct EmptyEnvironment {
};

// Environment<Var, Value, NextEnv> - środowisko reprezentowane jako lista dowiązana.
template <VarType, typename, typename>
struct Environment {
};

// GetEnv<Var, Env> - dostęp do wartości zmiennej o nazwie Var ze środowiska Env.
template <VarType, typename = EmptyEnvironment>
struct GetEnv {
};

template <VarType Var, typename Value, typename NextEnv>
struct GetEnv<Var, Environment<Var, Value, NextEnv> > {
    Value typedef result;
};

template <VarType Var, VarType Var2, typename Value2, typename NextEnv>
struct GetEnv<Var, Environment<Var2, Value2, NextEnv> > {
    typename GetEnv<Var, NextEnv>::result typedef result;
};

// ***
// Funkcje pomocnicze do Evaluate.
// ***

// BoolToLit<X> -> Lit<X>
template <bool>
struct BoolToLit {
    Lit<False> typedef result;
};

template <>
struct BoolToLit<true> {
    Lit<True> typedef result;
};

// ***
// Evaluate - obliczane w zależności od zawartości.
// ***

// Evaluate<Expression, Env, ValueType> - gdy nie podpada pod inne szablony
template <typename Expression, typename Env, typename>
struct Evaluate {
    Expression typedef result;
};

// Obliczenie Evaluate dla Lit<Fib<N>>.
template <unsigned long long N, typename Env, typename ValueType, typename ValueType2>
struct Evaluate<Lit<Fib<N>, ValueType2>, Env, ValueType> {
    typename Lit<Fib<N>, ValueType>::result typedef result;
};

// Obliczenie Evaluate dla Inc1<Arg>.
template <typename Arg, typename Env, typename ValueType>
struct Evaluate<Inc1<Arg>, Env, ValueType> {
    Numeric<ValueType, static_cast<ValueType>(Evaluate<Arg, Env, ValueType>::result::value + Lit<Fib<1>, ValueType>::result::value)> typedef result;
};

// Obliczenie Evaluate dla Inc10<Arg>.
template <typename Val, typename Env, typename ValueType>
struct Evaluate<Inc10<Val>, Env, ValueType> {
    Numeric<ValueType, static_cast<ValueType>(Evaluate<Val, Env, ValueType>::result::value + Lit<Fib<10>, ValueType>::result::value)> typedef result;
};

// Obliczenie Evaluate dla Sum<Args>.
template <typename Arg, typename Env, typename ValueType>
struct Evaluate<Sum<Arg>, Env, ValueType> {
    Numeric<ValueType, static_cast<ValueType>(Evaluate<Arg, Env, ValueType>::result::value)> typedef result;
};

template <typename Arg1, typename Arg2, typename... Args, typename Env, typename ValueType>
struct Evaluate<Sum<Arg1, Arg2, Args...>, Env, ValueType> {
    Numeric<ValueType, static_cast<ValueType>(Evaluate<Arg1, Env, ValueType>::result::value
                           + Evaluate<Sum<Arg2, Args...>, Env, ValueType>::result::value)> typedef result;
};

// Obliczenie Evaluate Ref<Var>.
template <VarType Var, typename Env, typename ValueType>
struct Evaluate<Ref<Var>, Env, ValueType> {
    typename GetEnv<Var, Env>::result typedef result;
};

// Obliczenie Evaluate dla If<Cond, Then, Else>.
template <typename Then, typename Else, typename Env, typename ValueType>
struct Evaluate<If<Lit<True>, Then, Else>, Env, ValueType> {
    typename Evaluate<Then, Env, ValueType>::result typedef result;
};

template <typename Then, typename Else, typename Env, typename ValueType>
struct Evaluate<If<Lit<False>, Then, Else>, Env, ValueType> {
    typename Evaluate<Else, Env, ValueType>::result typedef result;
};

template <typename Condition, typename Then, typename Else, typename Env, typename ValueType>
struct Evaluate<If<Condition, Then, Else>, Env, ValueType> {
    typename Evaluate<If<typename Evaluate<Condition, Env, ValueType>::result, Then, Else>, Env, ValueType>::result typedef result;
};

// Obliczenie Evaluate dla Let<Var, Val, Exp>.
template <VarType Var, typename Value, typename Expression, typename Env, typename ValueType>
struct Evaluate<Let<Var, Value, Expression>, Env, ValueType> {
    typename Evaluate<Expression, Environment<Var, typename Evaluate<Value, Env, ValueType>::result, Env>, ValueType>::result typedef result;
};

// Obliczenie Evaluate dla Eq<LVal, RVal>.
template <typename LeftVal, typename RightVal, typename Env, typename ValueType>
struct Evaluate<Eq<LeftVal, RightVal>, Env, ValueType> {
    typename Evaluate<Eq<typename Evaluate<LeftVal, Env, ValueType>::result,
                          typename Evaluate<RightVal, Env, ValueType>::result>,
        Env, ValueType>::result typedef result;
};

template <typename ValueType, ValueType x, ValueType y, typename Env>
struct Evaluate<Eq<Numeric<ValueType, x>, Numeric<ValueType, y> >, Env, ValueType> {
    typename BoolToLit<x == y>::result typedef result;
};

// Obliczenie Evaluate Lambda<Var, Body>.
template <typename Body, VarType Var, typename Env, typename ValueType>
struct Evaluate<Lambda<Var, Body>, Env, ValueType> {
    Scope<Lambda<Var, Body>, Env> typedef result;
};

// Obliczenie Evaluate dla Invoke<Lambda, Parm>.
template <typename Lambda, typename Value, typename Env, typename ValueType>
struct Evaluate<Invoke<Lambda, Value>, Env, ValueType> {
    typename Evaluate<Invoke<typename Evaluate<Lambda, Env, ValueType>::result, Value>, Env, ValueType>::result typedef result;
};

template <typename Body, VarType Var, typename Parm, typename Env, typename EnvSc, typename ValueType>
struct Evaluate<Invoke<Scope<Lambda<Var, Body>, EnvSc>, Parm>, Env, ValueType> {
    typename Evaluate<Let<Var, typename Evaluate<Parm, Env, ValueType>::result, Body>, EnvSc, ValueType>::result typedef result;
};

// ***
// Klasa Fibin.
// ***

// Wersja dla typów, które nie są wbudowanymi liczbami całkowitymi.
template <typename ValueType, bool = std::is_integral<ValueType>::value>
class Fibin {
public:
    template <typename Expr>
    static constexpr void eval()
    {
        std::cout << "Fibin doesn't support: " << typeid(ValueType).name() << '\n';
    }
};

// Wersja dla wbudowanych typów całkowitych.
template <typename ValueType>
class Fibin<ValueType, true> {
public:
    template <typename Expr>
    static constexpr ValueType eval()
    {
        return Evaluate<Expr, EmptyEnvironment, ValueType>::result::value;
    }
};

#endif //JNP1_ZAD4_FIBIN_H