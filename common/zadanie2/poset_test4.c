#include "poset.h"

#ifdef NDEBUG
  #undef NDEBUG
#endif

#include <assert.h>

#define K 470016

static unsigned long p[K];

int main() {
  for (int i = 0; i < K; ++i) {
    p[i] = poset_new();
  }

  for (int i = 0; i < K; ++i) {
    switch (i & 3) {
      case 0:
        assert(poset_insert(p[i], "Ala"));
        assert(poset_insert(p[i], "Bob"));
        assert(poset_add(p[i], "Ala", "Bob"));
        break;
      case 1:
        assert(poset_insert(p[i], "Ala"));
        assert(poset_insert(p[i], "Bob"));
        break;
      case 2:
        assert(poset_insert(p[i], "Ala"));
        assert(poset_insert(p[i], "Bob"));
        assert(poset_add(p[i], "Bob", "Ala"));
        break;
      case 3:
        break;
    }
  }

  for (int i = 0; i < K; ++i) {
    if ((i & 7) == 0 || (i & 3) == 1) {
      poset_delete(p[i]);
    }
  }

  for (int i = 0; i < K; ++i) {
    if ((i & 7) == 4) {
      assert(poset_test(p[i], "Ala", "Bob"));
    }
    else if ((i & 3) == 2) {
      assert(poset_test(p[i], "Bob", "Ala"));
    }
  }

  for (int i = 0; i < K; ++i) {
    if ((i & 7) != 0 && (i & 3) != 1) {
      poset_delete(p[i]);
    }
  }

  return 0;
}
