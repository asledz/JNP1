#include "fibin.h"

#include <cassert>
#include <climits>
#include <string>

// test 1: przykład
// test 2: Var, Lit
// test 3: eval dla Lit, Sum, Inc1, Inc10, inne eval
// test 4: If, Eq, obliczenia w ValueType
// test 5: Let, Ref, podstawowa Lambda
// test 6: testy z forum + Y kombinator

int main() {

// test 1: przykład
#if TEST_NUM == 101
    // Testing: lambda(x) {x + (Fib(1) + Fib(10)) + Fib(2)}(Fib(3))
    // Fib(0) = 0, Fib(1) = 1, Fib(2) = 1, Fib(3) = 2, Fib(10) = 55
    static_assert(59 == Fibin<uint64_t>::eval<Invoke<Lambda<Var("x"),
                  Sum<Ref<Var("x")>, Inc10<Lit<Fib<1>>>, Lit<Fib<2>>>>, Lit<Fib<3>>>>());

    // Testing: if False then Fib(0) else Fib(1)
    static_assert(1 == Fibin<uint8_t>::eval<If<Lit<False>, Lit<Fib<0>>, Lit<Fib<1>>>>());

    // Testing: let z = Fib(0) in {z + Fib(1)}
    static_assert(1 == Fibin<int16_t>::eval<Let<Var("z"), Lit<Fib<0>>, Inc1<Ref<Var("Z")>>>>());

    // Prints out to std::cout: "Fibin doesn't support: PKc"
    Fibin<const char*>::eval<Lit<Fib<0>>>();
#endif

// test 2: poprawne identyfikatory zmiennych
#if TEST_NUM == 201
    static_assert(Var("abcdef") == Var("ABCDEF"));
    static_assert(Var("0") == Var("0"));
    static_assert(Var("0") != Var("000"));
    static_assert(Var("012345") == Var("012345"));
    static_assert(Var("zz") == Var("ZZ"));
    static_assert(Var("a") != Var("aa"));
    static_assert(Var("A") != Var("AA"));
    static_assert(Var("9") != Var("Z"));
#endif

// test 2: poprawne literały
#if TEST_NUM == 202
    Fibin<int>::eval<If<Lit<True>, Lit<Fib<0>>, Lit<Fib<10>>>>();
    Fibin<int>::eval<If<Lit<False>, Lit<Fib<0>>, Lit<Fib<10>>>>();
#endif

// test 2: niepoprawne identyfikatory zmiennych - nie powinny się kompilować
#if TEST_NUM == 203
    Fibin<int>::eval<Let<Var(""), Lit<Fib<0>>, Lit<Fib<0>>>>();
#endif

#if TEST_NUM == 204
    Fibin<int>::eval<Let<Var("abcdefg"), Lit<Fib<0>>, Lit<Fib<0>>>>();
#endif

#if TEST_NUM == 205
    Fibin<int>::eval<Let<Var("0123456"), Lit<Fib<0>>, Lit<Fib<0>>>>();
#endif

#if TEST_NUM == 206
    Fibin<int>::eval<Let<Var("a!"), Lit<Fib<0>>, Lit<Fib<0>>>>();
#endif

#if TEST_NUM == 207
    Fibin<int>::eval<Let<Var("a@"), Lit<Fib<0>>, Lit<Fib<0>>>>();
#endif

#if TEST_NUM == 208
    Fibin<int>::eval<Let<Var("a#"), Lit<Fib<0>>, Lit<Fib<0>>>>();
#endif

#if TEST_NUM == 209
    Fibin<int>::eval<Let<Var("a$"), Lit<Fib<0>>, Lit<Fib<0>>>>();
#endif

#if TEST_NUM == 210
    Fibin<int>::eval<Let<Var("a%"), Lit<Fib<0>>, Lit<Fib<0>>>>();
#endif

#if TEST_NUM == 211
    Fibin<int>::eval<Let<Var("a^"), Lit<Fib<0>>, Lit<Fib<0>>>>();
#endif

#if TEST_NUM == 212
    Fibin<int>::eval<Let<Var("a&"), Lit<Fib<0>>, Lit<Fib<0>>>>();
#endif

#if TEST_NUM == 213
    Fibin<int>::eval<Let<Var("a*"), Lit<Fib<0>>, Lit<Fib<0>>>>();
#endif

#if TEST_NUM == 214
    Fibin<int>::eval<Let<Var("a("), Lit<Fib<0>>, Lit<Fib<0>>>>();
#endif

#if TEST_NUM == 215
    Fibin<int>::eval<Let<Var("a)"), Lit<Fib<0>>, Lit<Fib<0>>>>();
#endif

#if TEST_NUM == 216
    Fibin<int>::eval<Let<Var("a_"), Lit<Fib<0>>, Lit<Fib<0>>>>();
#endif

#if TEST_NUM == 217
    Fibin<int>::eval<Let<Var("a-"), Lit<Fib<0>>, Lit<Fib<0>>>>();
#endif

#if TEST_NUM == 218
    Fibin<int>::eval<Let<Var("a+"), Lit<Fib<0>>, Lit<Fib<0>>>>();
#endif

#if TEST_NUM == 219
    Fibin<int>::eval<Let<Var("a."), Lit<Fib<0>>, Lit<Fib<0>>>>();
#endif

#if TEST_NUM == 220
    Fibin<int>::eval<Let<Var("a,"), Lit<Fib<0>>, Lit<Fib<0>>>>();
#endif

// test 2: niepoprawne literały - nie powinny się kompilować
#if TEST_NUM == 221
    Fibin<int>::eval<Lit<Fib<Fib<0>>>>();
#endif

#if TEST_NUM == 222
    Fibin<int>::eval<If<Lit<true>, Lit<Fib<0>>, Lit<Fib<10>>>>();
#endif

#if TEST_NUM == 223
    struct MyLiteral {};
    Fibin<int>::eval<If<Lit<MyLiteral>, Lit<Fib<0>>, Lit<Fib<10>>>>();
#endif

#if TEST_NUM == 224
    struct MyLiteral {};
    Fibin<int>::eval<Lit<MyLiteral>>();
#endif

// test 3: wyliczanie wartości literałów liczbowych
#if TEST_NUM == 301
    static_assert(0 == Fibin<int8_t>::eval<Lit<Fib<0>>>());
    static_assert(1 == Fibin<int8_t>::eval<Lit<Fib<1>>>());
    static_assert(1 == Fibin<int8_t>::eval<Lit<Fib<2>>>());
    static_assert(2 == Fibin<int8_t>::eval<Lit<Fib<3>>>());
    static_assert(2 == Fibin<int8_t>::eval<Lit<Fib<3>>>());
    static_assert(-112 == Fibin<int8_t>::eval<Lit<Fib<12>>>());
    static_assert(144 == Fibin<int64_t>::eval<Lit<Fib<12>>>());
#endif

// test 3: wyliczanie Sum, Inc1, Inc10
#if TEST_NUM == 302
    static_assert(2 == Fibin<int64_t>::eval<Sum<
            Lit<Fib<1>>,
            Lit<Fib<1>>
    >>());

    static_assert(0 == Fibin<int64_t>::eval<Sum<
        Lit<Fib<0>>,
        Lit<Fib<0>>,
        Lit<Fib<0>>,
        Lit<Fib<0>>,
        Lit<Fib<0>>,
        Lit<Fib<0>>,
        Lit<Fib<0>>,
        Lit<Fib<0>>,
        Lit<Fib<0>>,
        Lit<Fib<0>>,
        Lit<Fib<0>>,
        Lit<Fib<0>>,
        Lit<Fib<0>>,
        Lit<Fib<0>>,
        Lit<Fib<0>>,
        Lit<Fib<0>>,
        Lit<Fib<0>>,
        Lit<Fib<0>>,
        Lit<Fib<0>>,
        Lit<Fib<0>>
        >>());

    static_assert(20 == Fibin<int64_t>::eval<Sum<
        Lit<Fib<1>>,
        Lit<Fib<1>>,
        Lit<Fib<1>>,
        Lit<Fib<1>>,
        Lit<Fib<1>>,
        Lit<Fib<1>>,
        Lit<Fib<1>>,
        Lit<Fib<1>>,
        Lit<Fib<1>>,
        Lit<Fib<1>>,
        Lit<Fib<1>>,
        Lit<Fib<1>>,
        Lit<Fib<1>>,
        Lit<Fib<1>>,
        Lit<Fib<1>>,
        Lit<Fib<1>>,
        Lit<Fib<1>>,
        Lit<Fib<1>>,
        Lit<Fib<1>>,
        Lit<Fib<1>>
        >>());

    static_assert(2 == Fibin<int64_t>::eval<Inc1<Lit<Fib<1>>>>());

    static_assert(10 == Fibin<int64_t>::eval<Inc1<Inc1<Inc1<Inc1<Inc1<Inc1<Inc1<Inc1<Inc1<Inc1<Lit<Fib<0>>>>>>>>>>>>>());

    static_assert(56 == Fibin<int64_t>::eval<Inc10<Lit<Fib<1>>>>());

    static_assert(550 == Fibin<int64_t>::eval<Inc10<Inc10<Inc10<Inc10<Inc10<Inc10<Inc10<Inc10<Inc10<Inc10<Lit<Fib<0>>>>>>>>>>>>>());
#endif

// test 3: eval dla typów całkowitoliczbowych
#if TEST_NUM == 303
    static_assert(0 == Fibin<char>::eval<Lit<Fib<0>>>());
    static_assert(0 == Fibin<char16_t>::eval<Lit<Fib<0>>>());
    static_assert(0 == Fibin<char32_t>::eval<Lit<Fib<0>>>());
    static_assert(0 == Fibin<wchar_t>::eval<Lit<Fib<0>>>());
    static_assert(0 == Fibin<short>::eval<Lit<Fib<0>>>());
    static_assert(0 == Fibin<int>::eval<Lit<Fib<0>>>());
    static_assert(0 == Fibin<long>::eval<Lit<Fib<0>>>());
    static_assert(0 == Fibin<long long>::eval<Lit<Fib<0>>>());
    static_assert(0 == Fibin<unsigned short>::eval<Lit<Fib<0>>>());
    static_assert(0 == Fibin<unsigned int>::eval<Lit<Fib<0>>>());
    static_assert(0 == Fibin<unsigned long>::eval<Lit<Fib<0>>>());
    static_assert(0 == Fibin<unsigned long long>::eval<Lit<Fib<0>>>());
#endif

// test 3: eval dla typów niecałkowitoliczbowych
#if TEST_NUM == 304
    Fibin<double>::eval<Lit<Fib<0>>>();

    struct Anything {};
    Fibin<Anything>::eval<Lit<Fib<0>>>();

    Fibin<std::string>::eval<Lit<Fib<0>>>();
#endif

// test 3: niepoprawne Sum, Inc1, Inc10
#if TEST_NUM == 305
    Fibin<int64_t>::eval<Sum<Lit<Fib<1>>>>());
#endif

#if TEST_NUM == 305
    Fibin<int64_t>::eval<Inc1<Lit<Fib<1>>, Lit<Fib<1>>>>();
#endif

#if TEST_NUM == 306
    Fibin<int64_t>::eval<Inc10<Lit<Fib<1>>, Lit<Fib<1>>>>();
#endif

// test 4: niepoprawne eval dla literałów logicznych
#if TEST_NUM == 307
    Fibin<int64_t>::eval<Lit<True>>();
#endif

#if TEST_NUM == 308
    Fibin<int64_t>::eval<Lit<False>>();
#endif

// test 4: If oraz Eq
#if TEST_NUM == 401
    static_assert(0 == Fibin<int64_t>::eval<If<Lit<True>, Lit<Fib<0>>, Lit<Fib<1>>>>());
    static_assert(1 == Fibin<int64_t>::eval<If<Lit<False>, Lit<Fib<0>>, Lit<Fib<1>>>>());
    static_assert(0 == Fibin<int64_t>::eval<If<Eq<Lit<Fib<0>>, Lit<Fib<0>>>, Lit<Fib<0>>, Lit<Fib<1>>>>());
    static_assert(1 == Fibin<int64_t>::eval<If<Eq<Lit<Fib<0>>, Lit<Fib<1>>>, Lit<Fib<0>>, Lit<Fib<1>>>>());
#endif

// test 4: eval używa wskazanego typu ValueType do obliczeń
#if TEST_NUM == 402
    // Fib<13> + Fib<8> + Fib<3> == Fib<0> (int8_t)
    static_assert(0 == Fibin<int8_t>::eval<If<
        Eq<Sum<Lit<Fib<13>>, Lit<Fib<8>>, Lit<Fib<3>>>, Lit<Fib<0>>>,
        Lit<Fib<0>>,
        Lit<Fib<1>>>>());

    // Fib<13> + Fib<8> + Fib<3> != Fib<0> (int64_t)
    static_assert(1 == Fibin<int64_t>::eval<If<
         Eq<Sum<Lit<Fib<13>>, Lit<Fib<8>>, Lit<Fib<3>>>, Lit<Fib<0>>>,
         Lit<Fib<0>>,
         Lit<Fib<1>>>>());
#endif

// test 4: niepoprawne If oraz Eq
#if TEST_NUM == 403
    Fibin<int64_t>::eval<If<Lit<Fib<0>>, Lit<Fib<0>>, Lit<Fib<1>>>>();
#endif

#if TEST_NUM == 404
    Fibin<int64_t>::eval<If<Eq<True, True>, Lit<Fib<0>>, Lit<Fib<1>>>>();
#endif

#if TEST_NUM == 405
    Fibin<int>::eval<Let<If<Lit<True>, Var("x"), Lit<Fib<2>>>, Lit<Fib<3>>, Ref<Var("x")>>>();
#endif

#if TEST_NUM == 406
    Fibin<int>::eval<Let<If<Lit<True>, Var("x"), Var("x")>, Lit<Fib<3>>, Ref<Var("x")>>>();
#endif

// test 5: Let, Ref
#if TEST_NUM == 501
    static_assert(0 == Fibin<int>::eval<
        Let<Var("x"), Lit<Fib<0>>, Ref<Var("X")>>>());

    static_assert(1 == Fibin<int>::eval<
        Let<Var("x"),
            Lit<Fib<0>>,
            Let<Var("x"),
                Lit<Fib<1>>,
                Ref<Var("x")>>>>());

    static_assert(2 == Fibin<int>::eval<
        Let<Var("x"),
            Lit<Fib<0>>,
            Let<Var("y"),
                Lit<Fib<1>>,
                Let<Var("X"),
                    Lit<Fib<3>>,
                    Let<Var("y"),
                        Lit<Fib<5>>,
                        Ref<Var("x")>>>>>>());
#endif

// test 5: podstawowa Lambda
#if TEST_NUM == 502
    static_assert(0 == Fibin<int>::eval<
        Invoke<Lambda<Var("x"), Ref<Var("x")>>, Lit<Fib<0>>>>());

    static_assert(1 == Fibin<int>::eval<
        Invoke<Lambda<Var("x"), Ref<Var("x")>>, Lit<Fib<1>>>>());

    static_assert(56 == Fibin<int>::eval<
        Invoke<Lambda<Var("x"), Inc10<Ref<Var("x")>>>, Lit<Fib<1>>>>());
#endif

// test 5: niepoprawne konstrukcje Let, Ref, Lambda
#if TEST_NUM == 503
    Fibin<int>::eval<Let<Var("x"), Lit<Fib<0>>, Ref<Var("y")>>>();
#endif

#if TEST_NUM == 504
    Fibin<int>::eval<Lambda<Var("x"), Inc10<Ref<Var("x")>>>>();
#endif

#if TEST_NUM == 505
    Fibin<std::uint32_t>::eval<
        Let<Var("f"),
            Lambda<Var("x"), Ref<Var("x")>>,
            Ref<Var("f")>>>();
#endif

// test 6: testy z forum
#if TEST_NUM == 601
    // ((x) => (x) => x)(0)(1)
    static_assert(1 == Fibin<int>::eval<
        Invoke<
            Invoke<Lambda<Var("x"), Lambda<Var("x"), Ref<Var("x")>>>, Lit<Fib<0>>>,
            Lit<Fib<1>>>>());
#endif

#if TEST_NUM == 602
    // ((x) => (y) => x)(0)(1)
    static_assert(0 == Fibin<int>::eval<
         Invoke<
            Invoke<Lambda<Var("x"), Lambda<Var("y"), Ref<Var("x")>>>, Lit<Fib<0>>>,
            Lit<Fib<1>>>>());
#endif

#if TEST_NUM == 603
    // (let x = 0 in (x) => x)(1)
    static_assert(1 == Fibin<int>::eval<
        Invoke<
            Let<Var("x"), Lit<Fib<0>>, Lambda<Var("x"), Ref<Var("x")>>>,
            Lit<Fib<1>>>>());
#endif

#if TEST_NUM == 604
    // let f = (x) => x + 1 in f(0)
    static_assert(1 == Fibin<int>::eval<
            Let<Var("f"),
                Lambda<Var("x"), Inc1<Ref<Var("x")>>>,
                Invoke<Ref<Var("f")>, Lit<Fib<0>>>>>());
#endif

#if TEST_NUM == 605
    // let const = 1 in
    // let f = (x) => x + const in
    // let const = 2 in f(0)
    static_assert(1 == Fibin<int>::eval<
        Let<Var("const"),
            Lit<Fib<1>>,
            Let<Var("f"),
                Lambda<Var("x"), Sum<Ref<Var("const")>, Ref<Var("x")>>>,
                Let<Var("const"),
                    Lit<Fib<3>>,
                    Invoke<Ref<Var("f")>, Lit<Fib<0>>>>>>>());
#endif

#if TEST_NUM == 606
    // let x = 0 in ((x) => x)(1)
    static_assert(1 == Fibin<int>::eval<
        Let<Var("x"),
            Lit<Fib<0>>,
            Invoke<Lambda<Var("x"), Ref<Var("x")>>, Lit<Fib<1>>>>>());
#endif

#if TEST_NUM == 607
    // let f1 = x -> x in
    // let f2 = y -> y(1) in
    // f2 f1
    static_assert(1 == Fibin<int>::eval<
        Let<Var("f1"),
            Lambda<Var("x"), Ref<Var("x")>>,
            Let<Var("f2"),
                Lambda<Var("y"), Invoke<Ref<Var("y")>, Lit<Fib<1>>>>,
                Invoke<Ref<Var("f2")>, Ref<Var("f1")>>>>>());
#endif

#if TEST_NUM == 608
    // let t = True in t ? 0 : 1
    static_assert(0 == Fibin<int>::eval<
        Let<Var("t"),
            Lit<True>,
            If<Ref<Var("t")>, Lit<Fib<0>>, Lit<Fib<1>>>>>());
#endif

#if TEST_NUM == 609
    // let tf = (x) => True in tf(False) ? 0 : 1;
    static_assert(0 == Fibin<int>::eval<
        Let<Var("tf"),
            Lambda<Var("x"), Lit<True>>,
            If<Invoke<Ref<Var("tf")>, Lit<False>>, Lit<Fib<0>>, Lit<Fib<1>>>>>());
#endif

#if TEST_NUM == 610
    // kombinator Y: oblicza różnicę left-right (5-1)
    static_assert(4 == Fibin<int>::eval<
        Let<Var("left"),
            Lit<Fib<5>>,
            Let<Var("right"),
                Lit<Fib<1>>,
                Invoke<
                       Invoke<
                           // kombinator Y
                           Lambda<Var("f"), Invoke<Lambda<Var("a"), Invoke<Ref<Var("a")>, Ref<Var("a")>>>,
                           Lambda<Var("x"), Invoke<Ref<Var("f")>, Lambda<Var("y"), Invoke<Invoke<Ref<Var("x")>, Ref<Var("x")>>, Ref<Var("y")>>>>>>>,
                           // różnica: (diff) => ((r) => (left == r) ? 0 : diff(r + 1) + 1)
                           Lambda<Var("diff"), Lambda<Var("r"), If<Eq<Ref<Var("r")>, Ref<Var("left")>>, Lit<Fib<0>>, Inc1<Invoke<Ref<Var("diff")>, Inc1<Ref<Var("r")>>>>>>>>,
                       Ref<Var("right")>>>>>());
#endif

}
