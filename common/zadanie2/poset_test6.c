#include "poset.h"

#ifdef NDEBUG
  #undef NDEBUG
#endif

#include <assert.h>

#define M 999999
#define N 93
#define K 4321

// Szybka konwersja małej liczby na napis ósemkowy.
static void int2str(char *s, int i) {
  s[M - 4] = '0' + i / 512;
  i %= 512;
  s[M - 3] = '0' + i / 64;
  i %= 64;
  s[M - 2] = '0' + i / 8;
  i %= 8;
  s[M - 1] = '0' + i;
}

int main() {
  static char el1[M + 1];
  static char el2[M + 1];

  for (int i = 0; i < M - 4; ++i)
    el2[i] = el1[i] = 'a';
  el2[M] = el1[M] = 0;

  unsigned long p = poset_new();

  for (int i = 0; i < N; ++i) {
    int2str(el1, i);
    assert(poset_insert(p, el1));
    for (int j = 0; j < i; ++j) {
      int2str(el2, j);
      assert(poset_add(p, el2, el1));
    }
  }

  for (int i = 0; i < N; ++i) {
    int2str(el1, i);
    for (int j = 0; j < i; ++j) {
      int2str(el2, j);
      assert(poset_test(p, el2, el1));
      assert(!poset_test(p, el1, el2));
    }
    assert(poset_test(p, el1, el1));
  }

  el1[0] = 'b';
  for (int i = 0; i < K; ++i) {
    unsigned long q = poset_new();
    int2str(el1, i);
    assert(poset_insert(q, el1));
    poset_delete(q);
  }

  poset_delete(p);

  return 0;
}
