#include "insertion_ordered_map.h"

#include <cstdlib>
#include <cassert>
#include <iostream>
#include <vector>
#include <set>
#include <string>
#include <utility>
#include <algorithm>
#include <numeric>
#include <random>
#include <memory>
#include <boost/operators.hpp>

// ukradzione z https://github.com/facebook/folly/blob/master/folly/Benchmark.h
template <typename T>
auto doNotOptimizeAway(const T& datum) {
    asm volatile("" ::"m"(datum) : "memory");
}

template<class K, class V, class H>
bool operator==(insertion_ordered_map<K, V, H> &q1, insertion_ordered_map<K, V, H> &q2)
{
    if (q1.size() != q2.size())
        return false;

    auto q1c = q1;
    auto q2c = q2;
    // Sprawdzamy równość i spójność kolejek.
    std::set<K> keys;
    std::multiset<std::pair<K, V>> removed_by_key;
    std::multiset<std::pair<K, V>> removed_from_front;
    size_t q1_size = q1.size();
    size_t q2_size = q2.size();
    size_t iter_seq_len1 = 0;
    size_t iter_seq_len2 = 0;

    // Sprawdzamy spójność.
    // Iteratory bierzemy z oryginalnych słowników i dlatego parametry nie mogą być const.
    // To jest dlatego, że konstruktor kopiujący może ukryć pewne problemy ze spójnością
    // wewnętrznych struktur, szczególnie jeśli jedna z nich jest budowana na podstawie drugiej.
    auto it1 = q1.begin(), end1 = q1.end(), it2 = q2.begin(), end2 = q2.end();
    for (; it1 != end1 && end2 != it2; ++it1, ++it2) {
        if (*it1 != *it2)
            return false;
        if (q1.contains(it1->first) != q2.contains(it2->first))
            return false;
        keys.insert(it1->first);
        iter_seq_len1++;
        iter_seq_len2++;
    }
    if (it1 != end1 || end2 != it2)
        return false;

    for (auto &k : keys) {
        while (q1c.contains(k)) {
            if (q1.at(k) != q2.at(k))
                return false;
            if (q1[k] != q2[k])
                return false;
            removed_by_key.emplace(k, q1c.at(k));
            q1c.erase(k);
            q2c.erase(k);
        }
    }
    assert(q1c.empty() && q1c.size() == 0 && q2c.empty() && q2c.size() == 0);
    assert(iter_seq_len1 == q1_size && iter_seq_len2 == q2_size);
    // Jeszcze raz kopiujemy i sprawdzamy.
    q1c = q1;
    q2c = q2;
    while (!q1c.empty()) {
        if (*q1c.begin() != *q2c.begin())
            return false;
        removed_from_front.insert(*q1c.begin());
        q1c.erase(q1c.begin()->first);
        q2c.erase(q2c.begin()->first);
    }
    assert(removed_by_key == removed_from_front);
    assert(q2c.empty());
    return true;
}

#if TEST_NUM != 100

long const max_throw_countdown = 50;
bool gChecking = false;
long throw_countdown;
long gInstancesCounter;

int ThisCanThrow(const int& i = 0)
{
    (void)max_throw_countdown;
    if (gChecking) {
        if (--throw_countdown <= 0) {
            throw 0;
        }
    }
    return i;
}

void* operator new(size_t size)
{
    try {
        ThisCanThrow();
        void* p = malloc(size);
        if (!p)
            throw "operator new() error";
        return p;
    } catch (...) {
        throw std::bad_alloc();
    }
}

void* operator new[](size_t size)
{
    try {
        ThisCanThrow();
        void* p = malloc(size);
        if (!p)
            throw "operator new[]() error";
        return p;
    } catch (...) {
        throw std::bad_alloc();
    }
}

void* operator new(std::size_t size, std::align_val_t al)
{
    try {
        ThisCanThrow();
        void* p = aligned_alloc(static_cast<size_t>(al), size);
        if (!p)
            throw "operator new() error";
        return p;
    } catch (...) {
        throw std::bad_alloc();
    }
}

void* operator new[](std::size_t size, std::align_val_t al)
{
    try {
        ThisCanThrow();
        void* p = aligned_alloc(static_cast<size_t>(al), size);
        if (!p)
            throw "operator new[]() error";
        return p;
    } catch (...) {
        throw std::bad_alloc();
    }
}

void operator delete (void* p) noexcept
{
    free(p);
}

void operator delete (void* p, size_t s) noexcept
{
    (void)s; free(p);
}

void operator delete[](void *p) noexcept
{
    free(p);
}

void operator delete[](void *p, size_t s) noexcept
{
    (void)s; free(p);
}

void operator delete(void* p, std::align_val_t al) noexcept
{
    (void)al; free(p);
}

void operator delete[](void* p, std::align_val_t al) noexcept
{
    (void)al; free(p);
}

void operator delete(void* p, std::size_t sz,
                     std::align_val_t al) noexcept
{
    (void)al; (void)sz; free(p);
}

void operator delete[](void* p, std::size_t sz,
                       std::align_val_t al) noexcept
{
    (void)al; (void)sz; free(p);
}

struct Tester
{
    int* p;
    explicit Tester (int v = 0) : p(new int(ThisCanThrow(v))) {
        ++gInstancesCounter;
    }

    Tester (const Tester& rhs) : p(new int(ThisCanThrow(*rhs.p))) {
        ++gInstancesCounter;
    }

    Tester(Tester&& t) noexcept : p(t.p) {
        ++gInstancesCounter;
        t.p = nullptr;
    }

    Tester& operator=(const Tester& rhs) {
        ThisCanThrow();
        *p = *rhs.p;
        return *this;
    }

    ~Tester( ) noexcept {
        /* This CANNOT throw! */
        gInstancesCounter--;
        delete p;
    }

    bool operator<(const Tester& rhs) const {
        ThisCanThrow();
        return *p < *rhs.p;
    }

    bool operator>(const Tester& rhs) const {
        ThisCanThrow();
        return *p > *rhs.p;
    }

    bool operator<=(const Tester& rhs) const {
        ThisCanThrow();
        return *p <= *rhs.p;
    }

    bool operator>=(const Tester& rhs) const {
        ThisCanThrow();
        return *p >= *rhs.p;
    }

    bool operator==(const Tester& rhs) const {
        ThisCanThrow();
        return *p == *rhs.p;
    }

    bool operator!=(const Tester& rhs) const {
        ThisCanThrow();
        return *p != *rhs.p;
    }
};

struct TesterHash {
    std::hash<int> h;
    auto operator()(Tester const &i) const { ThisCanThrow(); return h(*i.p); }
};

int main();

class CopyOnly {
    friend int main();
    CopyOnly() {};
public:
    CopyOnly(CopyOnly &&) = delete;
    CopyOnly(CopyOnly const &other) = default;
    CopyOnly& operator=(CopyOnly const &) = delete;
    CopyOnly& operator=(CopyOnly &&) = delete;
};

struct IdentityTester : boost::equality_comparable<IdentityTester>
{
    static size_t next;
    size_t const id;

    IdentityTester() : id(next++) {
    }

    IdentityTester(IdentityTester const &other) : id(next++) {
        (void)other;
    }

    IdentityTester(IdentityTester &&other) = default;

    bool operator==(IdentityTester const &other) const {
        return id == other.id;
    }

};

size_t IdentityTester::next = 0;

std::ostream & operator << (std::ostream& os, Tester t) {
    return os << "<tester(" << t.p << ")>";
}


template <class Value, class Operation, class Result>
Result NoThrowCheck(Value &v, Operation const &op, std::string const &name)
{
    Result result;

    try {
        throw_countdown = 0;
        gChecking = true;
        result = op(v); /* Try the operation. */
        gChecking = false;
    } catch (...) { /* Catch all exceptions. */
        gChecking = false;
        std::clog << "Operacja '" << name << "' okazala sie nie byc NO-THROW\n" << std::endl;
        assert(false);
    }

    return result;
}

template <class Value, class Operation>
void NoThrowCheckVoid(Value &v, Operation const &op, std::string const &name)
{
    try {
        throw_countdown = 0;
        gChecking = true;
        op(v); /* Try the operation. */
        gChecking = false;
    } catch (...) { /* Catch all exceptions. */
        gChecking = false;
        std::clog << "Operacja '" << name << "' okazala sie nie byc NO-THROW\n";
        assert(false);
    }
}

template <class Value, class Operation, class Result>
Result StrongCheck(Value &v, Operation const &op, std::string const &name)
{
    Result result;
    bool succeeded = false;

    Value duplicate = v;
    try {
        gChecking = true;
        result = op(duplicate); /* Try the operation. */
        gChecking = false;
        succeeded = true;
    } catch (...) { /* Catch all exceptions. */
        gChecking = false;
        bool unchanged = duplicate == v; /* Test strong guarantee. */
        if (!unchanged)
        {
            std::clog << "Operacja '" << name << "' okazala sie nie byc STRONG." << std::endl;
            assert(unchanged);
        }
    }

    if (succeeded) {
        v = duplicate;
    }
    return result;
}

template <class Value, class Operation>
void StrongCheckVoid(Value &v, Operation const &op, std::string const &name)
{
    bool succeeded = false;
    Value duplicate = v;
    // Wymuś kopię -- bez tego zazwyczaj nie uda się operacja COW
    // i będzie trywialnie spełniona asercja.
    if (duplicate.size() > 0)
        duplicate.at(duplicate.begin()->first);

    try {
        gChecking = true;
        op(duplicate); /* Try the operation. */
        gChecking = false;
        succeeded = true;
    } catch (...) { /* Catch all exceptions. */
        gChecking = false;
        bool unchanged = duplicate == v; /* Test strong guarantee. */
        if (!unchanged) {
            std::clog << "Operacja '" << name << "' okazala sie nie byc STRONG."
                        << std::endl;
            assert(unchanged);
        }
    }
    if (succeeded) {
        v = duplicate;
    }
}

template<class K, class V>
std::set<K> get_keys(insertion_ordered_map<K, V> &q) {
    std::set<K> ret;
    for (auto &kv : q)
        ret.insert(kv.first);
    return ret;
}

template<class K, class V>
void test_insert_spec(insertion_ordered_map<K, V> &q1, K const &k, V v)
{
    auto q_old = q1;
    std::set<K> keys = get_keys(q1);

    auto res = q1.insert(k, v);

    for (auto kk : get_keys(q1)) {
        // insert() should not add any other keys.
        if (kk != k)
            assert(keys.count(kk));
    }

    // Check effects on contains().
    assert(q1.contains(k));
    for (auto kk : keys) {
        if (kk != k)
            assert(q1.contains(kk) && q_old.contains(kk));
    }

    // Check effects on size() and empty().
    assert(!q1.empty());
    assert(q1.size() >= 1);
    assert(q_old.contains(k) || q1.size() == q_old.size() + 1);

    // Check effects on at().
    if (q_old.contains(k)) {
        // If q_old already had an element with key k,
        // then at(k) should be the same.
        assert(q1.at(k) == q_old.at(k));
        assert(q1[k] == q_old[k]);
        assert(!res);
    } else {
        assert(q1.at(k) == v);
        assert(res);
    }
    for (auto kk : keys) {
        // If the key was present in q_old, that means
        // it had at least one element mapped to it.
        // Therefore at(k) should give the same result on q1.
        assert(q1.at(kk) == q_old.at(kk));
        assert(q1[kk] == q_old[kk]);
    }

    // Check effects on iteration order.
    if (q_old.contains(k)) {
        auto k_old_it = q_old.end();
        auto it1 = q1.begin(), end1 = q1.end(), it2 = q_old.begin(), end2 = q_old.end();
        for (; it1 != end1 && end2 != it2; ++it1, ++it2) {
            if (it2->first == k) {
                k_old_it = it2;
                ++it2;
            }
            assert(*it1 == *it2);
        }
        assert(it1 != end1 && it1->first == k && *it1 == *k_old_it);
    } else {
        auto it1 = q1.begin(), it2 = q_old.begin(), end2 = q_old.end();
        for (; end2 != it2; ++it1, ++it2) {
            assert(*it1 == *it2);
        }
    }
}

template<class K, class V>
void test_erase_spec(insertion_ordered_map<K, V> &q1, K const &k)
{
    auto q_old = q1;
    q1.erase(k);

    std::set<K> keys = get_keys(q_old);

    // erase() should not add any keys.
    for (auto kk : get_keys(q1)) {
        assert(keys.count(kk));
    }

    // Check effects on contains().
    assert(!q1.contains(k) && q_old.contains(k));
    for (auto kk : keys) {
        if (kk != k)
            assert(q1.contains(kk) && q_old.contains(kk));
    }

    // Check effects on size() and empty().
    assert(q1.size() == q_old.size() - 1);
    assert((q1.size() > 0 && !q1.empty()) || (q1.size() == 0 && q1.empty()));

    // Check effects on at().
    for (auto kk : keys) {
        if (kk != k) {
            assert(q1.at(kk) == q_old.at(kk));
            assert(q1[kk] == q_old[kk]);
        }
    }

    // Check effects on iteration order.
    auto it1 = q1.begin(), end1 = q1.end(), it2 = q_old.begin(), end2 = q_old.end();
    for (; it1 != end1 && end2 != it2; ++it1, ++it2) {
        if (it2->first == k)
            ++it2;
        assert(it1->first != k);
        assert(*it1 == *it2);
    }
}

typedef insertion_ordered_map<Tester, Tester, TesterHash> TesterMap;

void empty_exception_check(const TesterMap& q, const Tester& (TesterMap::*m)() const)
{
    bool exception_occured = false;
    try {
        (q.*m)();
    } catch (lookup_error& e) {
        exception_occured = true;
    } catch (...) {
    }

    assert(exception_occured);
}

#endif

auto f(insertion_ordered_map<int, int> q)
{
    return q;
}

int main() {
// Test z treści zadania
#if TEST_NUM == 100
    int keys[] = {3, 1, 2};

    insertion_ordered_map<int, int> iom1 = f({});

    for (int i = 0; i < 3; ++i) {
        iom1[keys[i]] = i;
    }
    auto &ref = iom1[3];

    insertion_ordered_map<int, int> iom2(iom1); // Wykonuje się pełna kopia. Dlaczego?
    insertion_ordered_map<int, int> iom3;
    iom3 = iom2;

    ref = 10;
    assert(iom1[3] == 10);
    assert(iom2[3] != 10);

    iom2.erase(3); // Przy wyłączonych asercjach obiekt iom2 tutaj dokonuje kopii i przestaje współdzielić dane z iom3.
    assert(iom2.size() == 2);
    assert(!iom2.contains(3));
    assert(iom2.contains(2));

    assert(iom3.size() == 3);
    assert(iom3.contains(3));

    iom2.insert(4, 10);
    iom2.insert(1, 10);
    assert(iom2.size() == 3);
    insertion_ordered_map<int, int> const iom4 = iom2;
    {
        int order[] = {2, 4, 1};
        int values[] = {2, 10, 1};
        int i = 0;
        for (auto it = iom2.begin(), end = iom2.end(); it != end; ++it, ++i)
            assert(it->first == order[i] && it->second == values[i]);
        i = 0;
        for (auto it = iom4.begin(), end = iom4.end(); it != end; ++it, ++i)
            assert(it->first == order[i] && it->second == values[i]);
    }

    auto piom5 = std::make_unique<insertion_ordered_map<int, int>>();
    piom5->insert(4, 0);
    assert(piom5->at(4) == 0);
    auto iom6(*piom5);
    piom5.reset();
    assert(iom6.at(4) == 0);
    iom6[5] = 5;
    iom6[6] = 6;

    iom2.merge(iom6);
    {
        int order[] = {2, 1, 4, 5, 6};
        int values[] = {2, 1, 10, 5, 6};
        int i = 0;
        for (auto it = iom2.begin(), end = iom2.end(); it != end; ++it, ++i)
            assert(it->first == order[i] && it->second == values[i]);
    }

    std::swap(iom1, iom2);
    std::vector<insertion_ordered_map<int, int>> vec;
    for (int i = 0; i < 100000; i++) {
        iom1.insert(i, i);
    }
    for (int i = 0; i < 1000000; i++) {
        vec.push_back(iom1);  // Wszystkie obiekty w vec współdzielą dane.
    }

    return 0;

#endif

// Testy poprawnościowe: sprawdzają, czy wszystkie funcje dają wyniki i efekty zgodne z treścią zadania.
// Ten test nie sprawdza kwestii związanych z wyjątkami.

// Konstruktor bezparametrowy, konstruktor kopiujący, konstruktor przenoszący, operator przypisania
#if TEST_NUM == 200
    insertion_ordered_map<int, int> q;

    insertion_ordered_map<int, int> r = insertion_ordered_map<int, int>(q);
    assert(r == q);

    insertion_ordered_map<int, int> s(f(q));
    assert(s == q);

    insertion_ordered_map<int, int> t;
    t = q;
    assert(t == q);

    r.insert(3,33);
    r.insert(4,44);
    r.insert(5,44);

    insertion_ordered_map<int, int> u = insertion_ordered_map<int, int>(r);
    assert(u == r);

    insertion_ordered_map<int, int> v = insertion_ordered_map<int, int>(f(r));
    assert(v == r);

    t = r;
    assert(t == r);

    t = f(r);
    assert(t == r);

    // r powinno pozostać w spójnym stanie po przeniesieniu
    t = std::move(r);
    doNotOptimizeAway(r.size());
    doNotOptimizeAway(r.empty());
    r.insert(1, 1);
    assert(r.contains(1));
    r.erase(1);
    assert(!r.contains(1));

    insertion_ordered_map<int, int> x(std::move(r));
    doNotOptimizeAway(r.size());
    doNotOptimizeAway(r.empty());
    r.insert(1, 1);
    assert(r.contains(1));
    r.erase(1);
    assert(!r.contains(1));
#endif

// insert
#if TEST_NUM == 201
    insertion_ordered_map<int, int> q, q3;
    assert(q.empty());
    assert(q.size() == 0);

    for (int i = 0; i < 1000; i++)
        test_insert_spec(q, i, i);

    {
        int i = 0;
        for (auto it = q.begin(), end = q.end(); it != end; ++it)
            assert(it->first == i && it->second == i), i++;
    }
    assert(q.size() == 1000);
    assert(!q.empty());

    for (int s = 0; s < 5; ++s) {
        insertion_ordered_map<int, int> q2;
        std::vector<int> k(1000);
        std::vector<int> v(1000);
        std::iota(k.begin(), k.end(), 0);
        std::iota(v.begin(), v.end(), 0);
        std::shuffle(k.begin(), k.end(), std::mt19937(s));
        std::shuffle(v.begin(), v.end(), std::mt19937(10 + s));
        for (int i = 0; i < 1000; i++)
            test_insert_spec(q2, k[i], v[i]);
        assert(q2.size() == 1000);
        int i = 0;
        for (auto it = q2.begin(), end = q2.end(); it != end; ++it) {
            assert(it->first == k[i] && it->second == v[i]);
            i++;
        }
    }

    // test re-insertion
    for (int j = 0; j < 100; j++)
        for (int i = 0; i < 10; i++)
            test_insert_spec(q3, i, j);

    assert(q3.size() == 10);
    assert(!q3.empty());
    auto it = q3.begin();
    for (int i = 0; i < 10; i++) {
        assert(it->first == i && it->second == 0);
        ++it;
    }

    for (int s = 0; s < 5; ++s) {
        std::vector<int> v(10);
        std::iota(v.begin(), v.end(), 0);
        std::shuffle(v.begin(), v.end(), std::mt19937(s));
        for (int i = 0; i < 10; i++)
            test_insert_spec(q3, v[i], i);
        assert(q3.size() == 10);
    }
#endif

// erase
#if TEST_NUM == 202
    insertion_ordered_map<int, int> q;
    insertion_ordered_map<int, int> const &qc = q;
    assert(q.empty());
    assert(q.size() == 0);

    for (int i = 0; i < 1000; i++)
        q.insert(i, i);

    {
        int i = 0;
        for (auto it = q.begin(), end = q.end(); it != end; ++it)
            assert(it->first == i && it->second == i), i++;
    }
    assert(q.size() == 1000);
    assert(!q.empty());

    test_erase_spec(q, 0);
    assert(qc.begin()->first == 1 && qc.begin()->second == 1);
    assert(!q.empty());
    assert(q.size() == 999);

    test_erase_spec(q, 999);
    assert(!q.empty());
    assert(q.size() == 998);
    {
        int i = 1;
        for (auto it = q.begin(), end = q.end(); it != end; ++it)
            assert(it->first == i && it->second == i), i++;
        assert(i == 999);
    }

    // erasing in various orders
    for (int i = 1; i < 500; i++) {
        assert(qc.begin()->first == i && qc.begin()->second == i);
        test_erase_spec(q, i);
        test_erase_spec(q, 999 - i);
    }

    assert(q.size() == 0);
    assert(q.empty());

    for (int i = 0; i < 1000; i++)
        q.insert(i, i);
    assert(q.size() == 1000);
    assert(!q.empty());
    for (int i = 0; i < 500; i++)
        test_erase_spec(q, 2*i);
    assert(q.size() == 500);
    assert(!q.empty());
    for (int i = 0; i < 500; i++)
        test_erase_spec(q, 2*i + 1);
    assert(q.size() == 0);
    assert(q.empty());

    // pseudo-random order
    for (int s = 0; s < 5; ++s) {
        for (int i = 0; i < 1000; i++)
            q.insert(i, i);

        std::vector<int> v(1000);
        std::iota(v.begin(), v.end(), 0);
        std::shuffle(v.begin(), v.end(), std::mt19937(s));
        for (int i = 0; i < 1000; i++)
            test_erase_spec(q, v[i]);
        assert(q.size() == 0);
        assert(q.empty());
    }
#endif

// merge
#if TEST_NUM == 203
    // merge z samym sobą
    {
        insertion_ordered_map<int, int> q1;
        for (int i = 0; i < 100; i++)
            q1.insert(i, 1);
        q1.merge(q1);
        assert(q1.size() == 100);
        auto it = q1.begin();
        for (int i = 0; i < 100; ++i) {
            assert(it->first == i && it->second == 1);
            ++it;
        }
    }

    // rozłączne zbiory kluczy
    for (int s = 0; s < 5; ++s) {
        insertion_ordered_map<int, int> q1, q2;
        auto const &q2c = q2;
        std::vector<int> v1(1000);
        std::vector<int> v2(1000);
        std::iota(v1.begin(), v1.end(), 0);
        std::iota(v2.begin(), v2.end(), 2000);
        std::shuffle(v1.begin(), v1.end(), std::mt19937(s));
        std::shuffle(v2.begin(), v2.end(), std::mt19937(10 + s));
        for (int i = 0; i < 1000; i++)
            q1.insert(v1[i], 1);
        for (int i = 0; i < 1000; i++)
            q2.insert(v2[i], 2);

        q1.merge(q2c);
        assert(q1.size() == 2000);
        assert(q2.size() == 1000);

        auto it = q1.begin();
        for (int i = 0; i < 1000; ++i) {
            assert(it->first == v1[i] && it->second == 1);
            ++it;
        }
        for (int i = 0; i < 1000; ++i) {
            assert(it->first == v2[i] && it->second == 2);
            ++it;
        }
    }

    // takie same zbiory kluczy
    for (int s = 0; s < 5; ++s) {
        insertion_ordered_map<int, int> q1, q2;
        auto const &q2c = q2;
        std::vector<int> v1(1000);
        std::vector<int> v2(1000);
        std::iota(v1.begin(), v1.end(), 0);
        std::iota(v2.begin(), v2.end(), 0);
        std::shuffle(v1.begin(), v1.end(), std::mt19937(s));
        std::shuffle(v2.begin(), v2.end(), std::mt19937(10 + s));
        for (int i = 0; i < 1000; i++)
            q1.insert(v1[i], 1);
        for (int i = 0; i < 1000; i++)
            q2.insert(v2[i], 2);

        q1.merge(q2c);
        assert(q1.size() == 1000);
        assert(q2.size() == 1000);

        auto it = q1.begin();
        for (int i = 0; i < 1000; ++i) {
            assert(it->first == v2[i] && it->second == 1);
            ++it;
        }
    }

    // częściowo pokrywające się zbiory kluczy
    for (int s = 0; s < 5; ++s) {
        insertion_ordered_map<int, int> q1, q2;
        auto const &q2c = q2;
        std::vector<int> v1(1000);
        std::vector<int> v2(1000);
        std::iota(v1.begin(), v1.end(), 0);
        std::iota(v2.begin(), v2.end(), 500);
        std::shuffle(v1.begin(), v1.end(), std::mt19937(s));
        std::shuffle(v2.begin(), v2.end(), std::mt19937(10 + s));
        for (int i = 0; i < 1000; i++)
            q1.insert(v1[i], 1);
        for (int i = 0; i < 1000; i++)
            q2.insert(v2[i], 2);

        q1.merge(q2c);
        assert(q1.size() == 1500);
        assert(q2.size() == 1000);

        auto it = q1.begin();
        for (int i = 0; i < 1000; ++i) {
            if (v1[i] >= 500)
                continue;
            assert(it->first == v1[i] && it->second == 1);
            ++it;
        }
        for (int i = 0; i < 1000; ++i) {
            if (v2[i] < 1000)
                assert(it->first == v2[i] && it->second == 1);
            else
                assert(it->first == v2[i] && it->second == 2);
            ++it;
        }
    }

#endif

// clear
#if TEST_NUM == 204
    insertion_ordered_map<int, int> q;
    assert(q.size() == 0 && q.empty());
    q.clear();
    assert(q.size() == 0 && q.empty());
    q.insert(0, 0);
    assert(q.size() != 0 && !q.empty());
    q.clear();
    assert(q.size() == 0 && q.empty());

    for (int i = 0; i < 1000; ++i)
        q.insert(i, i);
    assert(q.size() != 0 && !q.empty());
    q.clear();
    assert(q.size() == 0 && q.empty());

    for (int i = 0; i < 1000; ++i)
        q.insert(i%11, i%17);
    assert(q.size() != 0 && !q.empty());
    q.clear();
    assert(q.size() == 0 && q.empty());
#endif

// begin, end, iteratory
#if TEST_NUM == 205
    insertion_ordered_map<int, int> q;
    for (int i = 0; i < 1000; i++) {
        q.insert(i, i%10);
    }

    int i = 0;
    for (auto it = q.begin(), end = q.end(); it != end; ++it, ++i) {
        assert(it->first == i && it->second == i%10);
    }
    assert(i == 1000);

    for (int i = 0; i < 1000; i++) {
        q.insert(i, i%10);
    }
    i = 0;
    for (auto it = q.begin(), end = q.end(); it != end; ++it, ++i) {
        assert(it->first == i && it->second == i%10);
    }
    assert(i == 1000);

    for (int i = 999; i >= 0; i--) {
        q.insert(i, i%10);
    }
    i = 0;
    for (auto it = q.begin(), end = q.end(); it != end; ++it, ++i) {
        assert(it->first == 999-i && it->second == (999-i)%10);
    }
    assert(i == 1000);
#endif

// swap (lokalny i globalny)
#if TEST_NUM == 206
    insertion_ordered_map<int, int> q;
    for (int i = 0; i < 1000; i++) {
        for (int j = 0; j < 10; j++) {
            q.insert(j,i);
        }
    }
    insertion_ordered_map<int, int> qq = insertion_ordered_map<int, int>(q);

    insertion_ordered_map<int, int> r;
    for (int i = 0; i > -1000; i--) {
        for (int j = 0; j < 10; j++) {
            r.insert(-j,i);
        }
    }
    insertion_ordered_map<int, int> rr = insertion_ordered_map<int, int>(r);

    std::swap(q, r);
    assert(q == rr && r == qq);

    std::swap(q, r);
    assert(q == qq && r == rr);

    std::swap(r, q);
    assert(q == rr && r == qq);

    std::swap(q, r);
    assert(q == qq && r == rr);

    std::swap(q, q);
    assert(q == qq);

    insertion_ordered_map<int, int> s, t;
    std::swap(s, t);
    assert(s.empty() && s == t);
#endif

// czy insert na istniejącym kluczu przypadkiem nie kopiuje wartości?
#if TEST_NUM == 207
    insertion_ordered_map<int, IdentityTester> q;
    q.insert(1, IdentityTester());
    auto id1 = q.at(1).id;
    q.insert(1, IdentityTester());
    auto id2 = q.at(1).id;

    assert(id1 == id2);
#endif

// Dodatkowe testy zgodności z treścią:
	// - testy gwarancji no-throw: konstruktor przenoszący, destruktor
	// - testy założeń nt. typu V

// czy empty rzuca wyjątki?
#if TEST_NUM == 300
    TesterMap q;

    auto f = [](auto &q) { return q.empty(); };
    bool empty = NoThrowCheck<TesterMap, decltype(f), bool>(q, f, "empty");
    assert(empty);
#endif

// size
#if TEST_NUM == 301
    TesterMap q;

    auto f = [](auto &q) { return q.size(); };
    bool size = NoThrowCheck<TesterMap, decltype(f), bool>(q, f, "size");
    assert(size == 0);
#endif

// std::swap (konstruktor przenoszący)
#if TEST_NUM == 302
    TesterMap q1, q2;

    for (int i = 1; i < 1000; i++) {
        for (int j = 1; j < 10; j++) {
            q1.insert(Tester(i), Tester(j));
            q2.insert(Tester(-i), Tester(-j));
        }
    }

    TesterMap q1Copy(q1);
    TesterMap q2Copy(q2);

    NoThrowCheckVoid(q1, [&q2](auto &q) { std::swap(q, q2); }, "std::swap");
    assert(q1Copy == q2);
    assert(q2Copy == q1);

    NoThrowCheckVoid(q1, [&q2](auto &q) { std::swap(q, q2); }, "std::swap");
    assert(q1Copy == q1);
    assert(q2Copy == q2);
#endif

// destruktor
#if TEST_NUM == 303
    char buf[sizeof(TesterMap)];
    TesterMap *q = new (buf) TesterMap();

    for (int i = 1; i < 1000; i++) {
        for (int j = 1; j < 10; j++) {
            q->insert(Tester(i), Tester(j));
        }
    }

    NoThrowCheckVoid(*q, [](auto &q) { q.~TesterMap(); }, "destruktor");
#endif

// Czy rozwiązanie nie zakłada za dużo na temat typu wartości?
#if TEST_NUM == 304
    insertion_ordered_map<int, CopyOnly> q;
    q.insert(0, CopyOnly());
    q.at(0);
    q.contains(0);
    q.insert(0, CopyOnly());
    q.clear();
#endif

// Czy rozwiązanie nie przechowuje zbyt dużo kopii wartości?
#if TEST_NUM == 305
    insertion_ordered_map<Tester, Tester, TesterHash> q1;

    for (int i = 0; i < 1000; i++) {
        for (int j = 0; j < 10; j++) {
            q1.insert(Tester(i), Tester(j));
        }
    }

    assert(q1.size() == 1000);
    assert(gInstancesCounter == 2*1000);
    q1.clear();

    insertion_ordered_map<Tester, Tester, TesterHash> q2;

    for (int i = 0; i < 1000; i++) {
        for (int j = 0; j < 10; j++) {
            q2.insert(Tester(i%17), Tester(j));
        }
    }

    assert(q2.size() == 17);
    assert(gInstancesCounter == 2*17);
    doNotOptimizeAway(q2);

#endif

// wartość zwracana z insert
#if TEST_NUM == 306
    insertion_ordered_map<int, int> q1;
    for (int j = 0; j < 10; j++) {
        assert(q1.insert(j, j));
    }
    for (int j = 0; j < 10; j++) {
        assert(!q1.insert(j, j));
    }

#endif

// Testy gwarancji silnych

// insert
#if TEST_NUM == 400
    // Żeby było trochę bardziej złośliwie -- nie rzucaj przy pierwszej możliwej operacji.
    for (int i = 0; i < max_throw_countdown; i++, throw_countdown = i) {
        TesterMap q;

        Tester t1 = Tester(-1);
        Tester t2 = Tester(-100);
        q.insert(t1, t2);
        Tester t3 = Tester(0);
        Tester t4 = Tester(100);
        StrongCheckVoid(q, [&](auto &q) { q.insert(t3, t4); }, "insert");

        for (int i = 100; i < 110; i++) {
            Tester t1 = Tester(i);
            Tester t2 = Tester(100);
            StrongCheckVoid(q, [&](auto &q) { q.insert(t1, t2); }, "insert");
        }

        for (int i = 0; i < 10; i++) {
            int k = (48271 * i) % 31;
            q.insert(Tester(k), Tester(100));
        }

        for (int i = 0; i < 10; i++) {
            int k = (16807 * i) % 31;
            Tester t1 = Tester(k);
            Tester t2 = Tester(100);
            StrongCheckVoid(q, [&](auto &q) { q.insert(t1, t2); }, "insert");
        }

        for (int i = 0; i < 1000; i++) {
            int k = (16807 * (i%10)) % 31;
            Tester t1 = Tester(k);
            Tester t2 = Tester(100);
            StrongCheckVoid(q, [&](auto &q) { q.insert(t1, t2); }, "insert");
        }

        for (auto it = q.begin(), end = q.end(); it != end; ++it) {
            assert(q.contains(it->first) && q.at(it->first) == it->second);
        }
    }
#endif

// Dla at to raczej nie powinno być problemem, ale dla kompletności można przetestować.
#if TEST_NUM == 401
    for (int i = 0; i < max_throw_countdown; i++, throw_countdown = i) {
        TesterMap q;

        q.insert(Tester(1), Tester(42));
        q.insert(Tester(2), Tester(13));

        Tester t1 = Tester(1);
        Tester t2 = Tester(2);
        Tester t3 = Tester(3);
        StrongCheckVoid(q, [&](auto &q) { q.at(t2); }, "at");
        StrongCheckVoid(q, [&](auto &q) { q[t1]; }, "operator[]");
        StrongCheckVoid(q, [&](auto &q) { q[t3]; }, "operator[]");
    }
#endif

// erase
#if TEST_NUM == 402
    for (int i = 0; i < max_throw_countdown; i++, throw_countdown = i) {
        TesterMap q;

        q.insert(Tester(1), Tester(42));
        q.insert(Tester(2), Tester(13));

        Tester t1 = Tester(1);
        Tester t2 = Tester(2);
        StrongCheckVoid(q, [&](auto &q) { q.erase(t2); }, "erase");
        StrongCheckVoid(q, [&](auto &q) { q.erase(t1); }, "erase");
        StrongCheckVoid(q, [&](auto &q) { q.erase(t2); }, "erase");
    }
#endif

// Dla clear zależnie od implementacji może to mieć sens.
#if TEST_NUM == 403
    for (int i = 0; i < max_throw_countdown; i++, throw_countdown = i) {
        TesterMap q;

        q.insert(Tester(1), Tester(100));
        q.insert(Tester(2), Tester(100));
        q.insert(Tester(3), Tester(300));

        StrongCheckVoid(q, [](auto &q) { q.clear(); }, "clear");

        TesterMap r;

        r.insert(Tester(1), Tester(100));
        r.insert(Tester(2), Tester(100));
        r.insert(Tester(3), Tester(300));
        q = r;

        StrongCheckVoid(q, [](auto &q) { q.clear(); }, "clear");
    }
#endif

// przypisanie
#if TEST_NUM == 404
    for (int i = 0; i < max_throw_countdown; i++, throw_countdown = i) {
        TesterMap p, q;

        StrongCheckVoid(p, [&q](auto &p) { p = q; }, "operator=");

        q.insert(Tester(3), Tester(9));
        q.insert(Tester(4), Tester(16));

        StrongCheckVoid(p, [&q](auto &p) { p = q; }, "operator=");

        TesterMap r = q;
        StrongCheckVoid(p, [&r](auto &p) { p = r; }, "operator=");
    }
#endif

// Ważność iteratorów
// Uwaga: ten test naprawdę powinien być wykonany pod valgrindem.
#if TEST_NUM == 405
    for (int i = 0; i < max_throw_countdown; i++, throw_countdown = i) {
        insertion_ordered_map<int, Tester> q1;
        q1.insert(0, Tester());
        q1.insert(1, Tester());
        q1.insert(2, Tester());
        auto q2 = std::make_unique<insertion_ordered_map<int, Tester>>(q1);
        q2->insert(3, Tester());
        insertion_ordered_map<int, Tester> q3 = *q2;
        auto it = q3.begin();
        auto end = q3.end();
        auto tester = Tester();
        try {
            gChecking = true;
            q3.insert(0, tester);
            gChecking = false;
            break; // Jeśli się udało, to poniżej będą problemy.
        } catch (...) {
            gChecking = false;
        }
        q2.reset();
        for (; it != end; ++it) {
            volatile auto _ = *it;
            (void)_;
        }
    }
#endif

// merge
#if TEST_NUM == 406
    TesterMap q2;
    q2.insert(Tester(1), Tester(42));
    q2.insert(Tester(2), Tester(13));
    q2.insert(Tester(4), Tester(13));
    q2.insert(Tester(5), Tester(13));
    for (int i = 0; i < max_throw_countdown; i++, throw_countdown = i) {
        TesterMap q;

        q.insert(Tester(1), Tester(42));
        q.insert(Tester(2), Tester(13));
        q.insert(Tester(3), Tester());
        q.insert(Tester(2), Tester());

        StrongCheckVoid(q, [&q2](auto &q) { q.merge(q2); }, "merge");
    }
#endif

// Czy rzucane są wyjątki zgodnie ze specyfikacją?

// at
#if TEST_NUM == 500
    TesterMap q;

    for (int i = 0; i < 10; i++)
        for (int j = 0; j < 10; j++)
            q.insert(Tester(j), Tester(i));

    (void)q.at(Tester(9));
    for (int i = 10; i < 100; i++) {
        bool exception_occured = false;
        try {
            q.at(Tester(10));
        } catch (lookup_error &e) {
            exception_occured = true;
        } catch (...) {
        }

        assert(exception_occured);
    }
#endif

// erase
#if TEST_NUM == 501
    TesterMap q;

    q.insert(Tester(0), Tester(0));
    bool exception_occured = false;
    try {
        q.erase(Tester(1));
    } catch (lookup_error &e) {
        exception_occured = true;
    } catch (...) {
    }

    assert(exception_occured);
#endif

// Testy kopiowania

// test COW
#if TEST_NUM == 600
    auto im1 = std::make_unique<insertion_ordered_map<int, int>>();
    std::vector<insertion_ordered_map<int, int>> vec;
    for (int i = 0; i < 100000; i++) {
        im1->insert(i, i);
    }
    for (int i = 0; i < 1000000; i++) {
        vec.push_back(*im1);
        doNotOptimizeAway(vec[i]);
    }
    im1.reset();
    for (int i = 0; i < 10; i++) {
        vec[i].insert(i, i);
    }

    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 11; j++) {
            if (i == j)
                continue;
            assert(!(vec[i] == vec[j]));
        }
    }
    assert(!(vec[0] == vec[999999]));
#endif

// zmiana wartości w prostej wersji
#if TEST_NUM == 601
    insertion_ordered_map<int, int> im1;

    for (int i = 0; i < 3; ++i) {
        im1.insert(i, i);
    }

    insertion_ordered_map<int, int> im2(im1);

    im1.at(0) = 10;

    assert(im1.at(0) == 10);
    assert(im2.at(0) != 10);
#endif

// zmiana wartości w prostej wersji, nr 2
#if TEST_NUM == 602
    insertion_ordered_map<int, int> im1;

    for (int i = 0; i < 3; ++i) {
        im1.insert(i, i);
    }

    insertion_ordered_map<int, int> im2(im1);

    im1[0] = 10;

    assert(im1[0] == 10);
    assert(im2[0] != 10);
#endif

// merge
#if TEST_NUM == 603
    insertion_ordered_map<int, int> q1, q2;

    q1.insert(1, 42);
    q1.insert(2, 13);
    q2.insert(3, 0);
    q2.insert(2, 0);

    insertion_ordered_map<int, int> qCopy1(q1);
    insertion_ordered_map<int, int> qCopy2;
    qCopy2 = q1;

    q1.merge(q2);

    assert(!(qCopy1 == q1));
    assert(qCopy1 == qCopy2);
    assert(!(qCopy2 == q1));
#endif

// clear
#if TEST_NUM == 604
    insertion_ordered_map<int, int> q;

    q.insert(1, 42);
    q.insert(2, 13);
    q.insert(3, 0);
    q.insert(2, 0);

    insertion_ordered_map<int, int> qCopy1(q);
    insertion_ordered_map<int, int> qCopy2;
    qCopy2 = q;

    q.clear();

    assert(q.empty());
    assert(!qCopy1.empty());
    assert(!qCopy2.empty());
    assert(!(qCopy1 == q));
    assert(qCopy1 == qCopy2);
    assert(!(qCopy2 == q));
#endif

// Czy flaga unshareable albo jej odpowiednik jest resetowana po modyfikacji?
#if TEST_NUM == 605
    auto im1 = insertion_ordered_map<int, int>();
    std::vector<insertion_ordered_map<int, int>> vec;
    for (int i = 0; i < 100000; i++) {
        im1.insert(i, i);
    }
    im1.at(0); // Ustaw flagę unshareable.
    im1.insert(-1, -1);

    for (int i = 0; i < 1000000; i++) {
        vec.push_back(im1);
        doNotOptimizeAway(vec[i]);
    }
#endif

// Test sprawdzający, czy jest header guard.

#if TEST_NUM == 700
#include "insertion_ordered_map.h"
#include "insertion_ordered_map.h"
#endif

// Test sprawdzający, czy wyjątek dziedziczy po std::exception.

#if TEST_NUM == 800
    try {
        throw lookup_error();
    } catch (std::exception &e) {
        return 0;
    } catch (...) {
        assert(false);
    }
#endif
    return 0;
}
