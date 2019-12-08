#include "fibo.h"
#if TEST_NUM == 601
#include "fibo.h"
#endif

#include <algorithm>
#include <cassert>
#include <climits>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <type_traits>

using namespace std;

// Opcje konfigurujące testy
static const bool showFailedTests = true;
static const bool showTotalResuls = false;
static const bool showEmptyTests = false;

using TestFunc = void (void);

static int totalPoints = 0;
static int testCount;
static int passedCount;
static bool allTestsOk = true;

static void resetTestCounters(void) {
    testCount = 0;
    passedCount = 0;
}

static void printTestStats(void) {
    if (testCount == 0 && !showEmptyTests) {
        return;
    }

    string result;
    if (testCount == passedCount) {
        totalPoints++;
        result = "PASS";
    } else {
        allTestsOk = false;
        result = "FAIL";
    }
    cout << passedCount << "/" << testCount << "\t : " << result << endl;
}

static void printTotalResult(void) {
    cout << "---------------" << endl;
    cout << "Total points: " << totalPoints << endl;
}

// Nie jest static, żeby kompilator nie wypisywał ostrzeżenia.
void check(bool condition) {
    testCount++;
    if (condition) {
        passedCount++;
    } else if (showFailedTests) {
        cout << testCount << " failed" << endl;
    }
}

static void test1KnownByStudents(void) {
#if TEST_NUM == 101
    Fibo f;

    check(f == Zero());
    check(Fibo(f) == Zero());
    check(Zero() < One());
    check(Fibo("11") == Fibo("100"));
    check((Fibo("1001") + Fibo("10")) == Fibo("1011"));
    check((Fibo("1001") & Fibo("1100")) == Zero()); // 1100 == 10000
    check((Fibo("1100") | Fibo("11")) == Fibo("10100")); // 1100 == 10000, 11 == 100
    check((Fibo("1001") ^ Fibo("1010")) == Fibo("11"));
    check((Fibo("101") << 3) == Fibo("101000"));

    f = One();
    f <<= 3;
    check(f == Fibo("1000"));

    f = One();
    check(f + Fibo("1") == Fibo("10"));
    check(f == One());

    Fibo f1("101");
    Fibo f2 = Fibo("101");
    check(f1 == f2);

    check(Fibo("11").length() == 3); // 11 == 100

    // std::cout << Fibo("11") << std::endl; // prints 100
#endif
}

static void test2Comparison(void) {
#if TEST_NUM == 201
    check(Fibo() == Zero());
    check(Fibo() >= Zero());
    check(Fibo() <= Zero());
    check(!(Fibo() != Zero()));
    check(!(Fibo() > Zero()));
    check(!(Fibo() < Zero()));

    check(Zero() != One());
    check(Zero() < One());
    check(Zero() <= One());
    check(!(Zero() == One()));
    check(!(Zero() > One()));
    check(!(Zero() >= One()));

    check(Fibo("11") == Fibo("100"));
    check(Fibo("11011") == Fibo("100100"));
    check(Fibo("111") == Fibo("1001"));
    check(Fibo("1111") == Fibo("10100"));
    check(Fibo("11111") == Fibo("101001"));
    check(Fibo("100001100") == Fibo("100010000"));
    check(Fibo("100011") > Fibo("100010"));
    check(Fibo("11000") > Fibo("10011"));
    check(Fibo("10100") < Fibo("100100"));
    check(Fibo("11") < Fibo("100000"));

    check(Fibo(0) == Zero());
    check(Fibo(1) == One());
    check(Fibo(2) == Fibo("10"));
    check(Fibo(3) == Fibo("11"));
    check(Fibo(10) == Fibo("10010"));
    check(Fibo(123456789) == Fibo("100010100000101010000010100100000101001"));

    Fibo f(2);
    check(f >= Fibo(2));
    check(Fibo(2) >= f);
    check(f <= Fibo(2));
    check(Fibo(2) <= f);
    check(f > Fibo(1));
    check(f < Fibo(3));
#endif
}

static void test2Add(void) {
#if TEST_NUM == 202
    check((Zero() + Zero()) == Zero());
    check((Zero() + One()) == One());
    check((One() + Zero()) == One());

    check((One() + One()) == Fibo("10"));
    check((Fibo("1") + Fibo("10000")) == Fibo("10001"));
    check((Fibo("10000") + Fibo("1")) == Fibo("10001"));
    check((Fibo("10010") + Fibo("100010")) == Fibo("1000101"));
    check((Fibo("10101") + Fibo("1010")) == Fibo("101001"));
    check((Fibo("1000101") + Fibo("1000101")) == Fibo("10100100"));

    Fibo f(Zero());
    check(f == Zero());
    check ((f + One()) == One());
    check(f == Zero());

    f = Fibo("1") + Fibo("10") + Fibo("100");
    check(f == Fibo("1001"));

    f = Fibo("101");
    f += f;
    check(f == Fibo("10000"));
    f += f += f;
    check(f == Fibo("1010100"));

    f = f;
    check(f == Fibo("1010100"));
#endif
}

#if TEST_NUM == 301
template <typename V, typename S, std::enable_if_t<std::is_constructible<Fibo, V>::value, int> = 0>
static bool constructor_test(V value, S str) {
    check(Fibo(value) == Fibo(str));
    return true;
}

template <typename V, typename S, std::enable_if_t<!std::is_constructible<Fibo, V>::value, int> = 0>
static bool constructor_test(V, S) {
    return false;
}
#endif

static void test3Big(void) {
#if TEST_NUM == 301
    stringstream ss{};
    constexpr char const * const max_31_bit_number = "100010001000010001001000001001010001000101000";
    constexpr char const * const max_32_bit_number = "1010001000010101000101000100000001000100100100";
    constexpr char const * const max_63_bit_number = "1000101010000000010001000100010100000010010100000010101000100100010100101001000010000101010";
    constexpr char const * const max_64_bit_number = "10100101000100000101000100010010001001000000001001000100100010101000100000101000101000001010";
    constexpr char const * const big_number  = "101010100010101000100001010100010100010000000100010100010000101010001010001000000010001001001001010001000010101000101000";
    constexpr char const * const plus_one    = "101010100010101000100001010100010100010000000100010100010000101010001010001000000010001001001001010001000010101000101001";
    constexpr char const * const shift_one   = "1010101000101010001000010101000101000100000001000101000100001010100010100010000000100010010010010100010000101010001010000";
    constexpr char const * const shift_two   = "10101010001010100010000101010001010001000000010001010001000010101000101000100000001000100100100101000100001010100010100000";
    constexpr char const * const shift_three = "101010100010101000100001010100010100010000000100010100010000101010001010001000000010001001001001010001000010101000101000000";
    constexpr char const * const shift_four  = "1010101000101010001000010101000101000100000001000101000100001010100010100010000000100010010010010100010000101010001010000000";

    check(Fibo(33) == Fibo("1010101"));

    // Wymagamy przynajmniej jednego konstruktora dla liczb 32-bitowych
    // i jednego dla liczb 64-bitowych.
    bool f1 = constructor_test(static_cast<int>(INT_MAX), max_31_bit_number) ||
              constructor_test(static_cast<unsigned>(UINT_MAX), max_32_bit_number);
    bool f2 = constructor_test(static_cast<long>(LONG_MAX), max_63_bit_number) ||
              constructor_test(static_cast<unsigned long>(ULONG_MAX), max_64_bit_number) ||
              constructor_test(static_cast<long long>(LLONG_MAX), max_63_bit_number) ||
              constructor_test(static_cast<unsigned long long>(ULLONG_MAX), max_64_bit_number);
    check(f1 && f2);

    ss << Fibo(big_number);
    check(ss.str() == big_number);

    ss.str("");
    ss << (Fibo(big_number) + One());
    check(ss.str() == plus_one);

    ss.str("");
    ss << (Fibo(big_number) << 1);
    check(ss.str() == shift_one);

    ss.str("");
    ss << (Fibo(big_number) << 2);
    check(ss.str() == shift_two);

    ss.str("");
    ss << (Fibo(big_number) << 3);
    check(ss.str() == shift_three);
    ss.str("");

    ss << (Fibo(big_number) << 4);
    check(ss.str() == shift_four);
#endif
}

static void test3Printing(void) {
#if TEST_NUM == 302
    ifstream file("testPrinting.txt");
    assert(file.is_open());

    std::string in, out, fiboStr;
    while (!file.eof()) {
        file >> in >> out;
        stringstream ss;
        // Nie wymagamy konstruktora od std::string.
        ss << Fibo(in.c_str());
        ss >> fiboStr;
        check(out == fiboStr);
    }
#endif
}

static void test4BitOperations(void) {
#if TEST_NUM == 401
    check((Zero() | Zero()) == Zero());
    check((Zero() & Zero()) == Zero());
    check((Zero() ^ Zero()) == Zero());

    check((Zero() | One()) == One());
    check((Zero() & One()) == Zero());
    check((Zero() ^ One()) == One());

    Fibo a;

    a = Fibo("101010") & Fibo("10101");
    check(a == Zero());
    a = Fibo("101010") | Fibo("10101");
    check(a == Fibo("1001111"));
    a = Fibo("101010") ^ Fibo("10101");
    check(a == Fibo("1010100"));

    check((Fibo("1001") & Fibo("1010")) == Fibo("1000"));
    check((Fibo("1001") | Fibo("1010")) == Fibo("1011"));
    check((Fibo("1001") ^ Fibo("1010")) == Fibo("11"));

    a = Fibo("1") & Fibo("100000");
    check(a == Zero());
    check(a.length() == 1);
    a = Fibo("100000") & Fibo("1");
    check(a == Zero());
    check(a.length() == 1);

    check((Fibo("1") | Fibo("100000")) == Fibo("100001"));
    check((Fibo("100000") | Fibo("1")) == Fibo("100001"));

    check((Fibo("1") ^ Fibo("100000")) == Fibo("100001"));
    check((Fibo("100000") ^ Fibo("1")) == Fibo("100001"));

    check((Fibo("0") & Fibo("100000")) == Zero());
    check((Fibo("100000") & Fibo("0")) == Zero());

    check((Fibo("0") | Fibo("100000")) == Fibo("100000"));
    check((Fibo("100000") | Fibo("0")) == Fibo("100000"));

    check((Fibo("0") ^ Fibo("100000")) == Fibo("100000"));
    check((Fibo("100000") ^ Fibo("0")) == Fibo("100000"));

    Fibo f(One());
    check ((f & Zero()) == Zero());
    check(f == One());
    check ((f | Fibo("10")) == Fibo("11"));
    check(f == One());
    check ((f ^ One()) == Zero());
    check(f == One());

    check((Fibo() << 0) == Zero());
    check((Fibo() << 100) == Zero());

    check((Fibo("1") << 0) == One());
    check(((Fibo("1") << 3) << 5) == (Fibo("1") << 8));
    a = Fibo("1") << 5;
    check(a == Fibo("100000"));

    check((Fibo("101") << 3) == Fibo("101000"));
    check((Fibo("11") << 3) == Fibo("11000"));

    Fibo f2(One());
    check ((f2 << 1) == Fibo("10"));
    check(f2 == One());

    f = Fibo("101");
    f ^= Fibo("1");
    check(f == Fibo("100"));
    f ^= f ^= f;
    check(f == Fibo("0"));

    f = Fibo("100");
    f |= Fibo("1");
    check(f == Fibo("101"));
    f |= f |= f;
    check(f == Fibo("101"));

    f = Fibo("100");
    f &= Fibo("101");
    check(f == Fibo("100"));
    f &= f &= f;
    check(f == Fibo("100"));
#endif
}

static void test5Const(void) {
// Poniższe testy 5xx mają się kompilować.
#if TEST_NUM == 501
    char const * const a = "100000";
    (Fibo)a;
#endif

#if TEST_NUM >= 502 && TEST_NUM <= 599
    const Fibo f1(5);
    const Fibo f2("1001");
#endif

#if TEST_NUM == 502
    check(Zero().length() == 1);
    check(One().length() == 1);
    bool b;
    stringstream ss;
    auto len = f1.length();
    b = f1 < f2;
    b = f1 <= f2;
    b = f1 > f2;
    b = f1 >= f2;
    b = f1 == f2;
    b = f1 != f2;
    ss << f1;
    f1 + f2;
    f1 | f2;
    f1 & f2;
    f1 ^ f2;
    (void)b; // Aby kompilator nie wypisywał ostrzeżenia.
    (void)len; // Aby kompilator nie wypisywał ostrzeżenia.
#endif

// Kolejne testy 5xx nie powinny się już kompilować.
#if TEST_NUM == 503
    f1 += f2;
#endif

#if TEST_NUM == 504
    f1 |= f2;
#endif

#if TEST_NUM == 505
    f1 &= f2;
#endif

#if TEST_NUM == 506
    f1 ^= f2;
#endif

#if TEST_NUM == 507
    f1 <<= 3;
#endif

#if TEST_NUM == 508
    f1 = f2;
#endif

#if TEST_NUM == 509
    Zero() += Zero();
#endif

#if TEST_NUM == 510
    One() |= One();
#endif

#if TEST_NUM == 511
    Zero() &= One();
#endif

#if TEST_NUM == 512
    One() ^= Zero();
#endif

#if TEST_NUM == 513
    Zero() <<= 1;
#endif

#if TEST_NUM == 514
    One() = One();
#endif
}

static void test6Compilation(void) {
// Poniższe testy 6xx mają się kompilować.
#if TEST_NUM == 601
    Fibo f = 0;

    f += 2;
    f &= 2;
    f |= 2;
    f ^= 2;
    f <<= 2;

    f = f + 2;
    f = 2 + f;
    f = f & 2;
    f = 2 & f;
    f = f | 2;
    f = 2 | f;
    f = f ^ 2;
    f = 2 ^ f;
    f = f << 2;
#endif

#if TEST_NUM == 602
    Fibo f(1);
    bool b;

    b = (f == 2);
    b = (2 == f);
    b = (f >= 2);
    b = (2 >= f);
    b = (f <= 2);
    b = (2 <= f);
    b = (f > 1);
    b = (1 > f);
    b = (f < 3);
    b = (3 < f);

    b = ((Zero() + 1) == One());
    b = ((1 + Zero()) == One());

    b = ((Zero() | 1) == One());
    b = ((1 | Zero()) == One());

    b = ((Zero() & 1) == Zero());
    b = ((1 & Zero()) == Zero());

    b = ((Zero() ^ 1) == One());
    b = ((1 ^ Zero()) == One());

    (void)b; // Aby kompilator nie wypisywał ostrzeżenia.
#endif

// Kolejne testy 6xx nie powinny się już kompilować.
#if TEST_NUM == 603
    Fibo f(true);
#endif

#if TEST_NUM == 604
    Fibo f('1');
#endif

#if TEST_NUM == 605
    bool b;
    b = Zero() < true;
#endif

#if TEST_NUM == 606
    bool b;
    b = Zero() <= '1';
#endif

#if TEST_NUM == 607
    bool b;
    b = Zero() == "1";
#endif

#if TEST_NUM == 608
    bool b;
    b = true > One();
#endif

#if TEST_NUM == 609
    bool b;
    b = '1' >= One();
#endif

#if TEST_NUM == 610
    bool b;
    b = "1" != One();
#endif

#if TEST_NUM == 611
    Fibo f;
    f += true;
#endif

#if TEST_NUM == 612
    Fibo f1, f2;
    f1 = f2 + true;
#endif

#if TEST_NUM == 613
    Fibo f1, f2;
    f1 = false + f2;
#endif

#if TEST_NUM == 614
    Fibo f;
    f += '1';
#endif

#if TEST_NUM == 615
    Fibo f1, f2;
    f1 = f2 + '1';
#endif

#if TEST_NUM == 616
    Fibo f1, f2;
    f1 = '1' + f2;
#endif

#if TEST_NUM == 617
    Fibo f;
    f += "10";
#endif

#if TEST_NUM == 618
    Fibo f1, f2;
    f1 = f2 + "10";
#endif

#if TEST_NUM == 619
    Fibo f1, f2;
    f1 = "10" + f2;
#endif
}

static void doTest(TestFunc* testFunc) {
    resetTestCounters();
    (*testFunc)();
    printTestStats();
}

int main(void) {
    TestFunc* tests[] = {
        &test1KnownByStudents,
        &test2Comparison, &test2Add,
        &test3Big, &test3Printing,
        &test4BitOperations,
        &test5Const,
        &test6Compilation
    };

    for_each(tests, tests + 8, doTest);
    if (showTotalResuls) {
        printTotalResult();
    }

    return !allTestsOk;
}
