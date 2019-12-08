#include "poset.h"

#ifdef NDEBUG
  #undef NDEBUG
#endif

#include <assert.h>

#define N 513

// Szybka konwersja małej liczby na napis ósemkowy.
static void int2str(char *s, int i) {
  s[0] = '0' + i / 512;
  i %= 512;
  s[1] = '0' + i / 64;
  i %= 64;
  s[2] = '0' + i / 8;
  i %= 8;
  s[3] = '0' + i;
  s[4] = 0;
}

int main() {
  static char el1[5];
  static char el2[5];

  unsigned long p = poset_new();

  for (int i = 0; i < N; ++i) {
    int2str(el1, i);
    assert(poset_insert(p, el1));
  }

  for (int i = 0; i + 2 < N; ++i) {
    int2str(el1, i);
    int2str(el2, i + 2);
    assert(poset_add(p, el1, el2));
  }

  for (int i = 0; i + 3 < N; ++i) {
    int2str(el1, i);
    int2str(el2, i + 3);
    assert(poset_add(p, el1, el2));
  }

  for (int i = 0; i < N; ++i) {
    int2str(el1, i);
    assert(poset_test(p, el1, el1));
    int2str(el2, i + 1);
    assert(!poset_test(p, el1, el2));
    assert(!poset_test(p, el2, el1));
    for (int j = i + 2; j < N; ++j) {
      int2str(el2, j);
      assert(poset_test(p, el1, el2));
      assert(!poset_test(p, el2, el1));
    }
  }

  poset_delete(p);

  return 0;
}
