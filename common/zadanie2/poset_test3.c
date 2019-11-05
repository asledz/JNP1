#include "poset.h"

#ifdef NDEBUG
  #undef NDEBUG
#endif

#include <assert.h>

int main() {
  unsigned long p1, p2, p3;

  assert(!poset_insert(0, NULL));
  assert(!poset_remove(0, NULL));
  assert(!poset_add(0, NULL, NULL));
  assert(!poset_del(0, NULL, NULL));
  assert(!poset_test(0, NULL, NULL));

  p1 = poset_new();
  p2 = poset_new();
  p3 = poset_new();

  assert(poset_insert(p1, "a"));
  assert(poset_insert(p1, "b"));
  assert(poset_insert(p1, "c"));
  assert(poset_insert(p1, "p"));
  assert(poset_insert(p1, "q"));
  assert(poset_insert(p1, "r"));
  assert(poset_add(p1, "a", "b"));
  assert(poset_add(p1, "p", "q"));
  assert(poset_add(p1, "b", "q"));
  assert(poset_add(p1, "b", "c"));
  assert(poset_add(p1, "q", "r"));
  assert(poset_del(p1, "b", "q"));

  assert(poset_insert(p2, "a"));
  assert(poset_insert(p2, "b"));
  assert(poset_insert(p2, "c"));
  assert(poset_insert(p2, "d"));
  assert(poset_add(p2, "a", "b"));
  assert(poset_add(p2, "a", "c"));
  assert(poset_add(p2, "b", "d"));
  assert(poset_add(p2, "c", "d"));
  assert(!poset_remove(p2, NULL));
  assert(poset_remove(p2, "b"));
  assert(poset_insert(p2, "NULL"));

  assert(poset_insert(p3, "w"));
  assert(poset_insert(p3, "x"));
  assert(poset_insert(p3, "y"));
  assert(poset_insert(p3, "z"));
  assert(poset_add(p3, "w", "x"));
  assert(poset_add(p3, "x", "y"));
  assert(poset_add(p3, "y", "z"));
  assert(poset_del(p3, "x", "y"));

  assert(poset_test(p1, "a", "a"));
  assert(poset_test(p1, "a", "b"));
  assert(poset_test(p1, "a", "c"));
  assert(!poset_test(p1, "a", "p"));
  assert(poset_test(p1, "a", "q"));
  assert(poset_test(p1, "a", "r"));
  assert(!poset_test(p1, "b", "a"));
  assert(poset_test(p1, "b", "b"));
  assert(poset_test(p1, "b", "c"));
  assert(!poset_test(p1, "b", "p"));
  assert(!poset_test(p1, "b", "q"));
  assert(poset_test(p1, "b", "r"));
  assert(!poset_test(p1, "c", "a"));
  assert(!poset_test(p1, "c", "b"));
  assert(poset_test(p1, "c", "c"));
  assert(!poset_test(p1, "c", "p"));
  assert(!poset_test(p1, "c", "q"));
  assert(!poset_test(p1, "c", "r"));
  assert(!poset_test(p1, "p", "a"));
  assert(!poset_test(p1, "p", "b"));
  assert(!poset_test(p1, "p", "c"));
  assert(poset_test(p1, "p", "p"));
  assert(poset_test(p1, "p", "q"));
  assert(poset_test(p1, "p", "r"));
  assert(!poset_test(p1, "q", "a"));
  assert(!poset_test(p1, "q", "b"));
  assert(!poset_test(p1, "q", "c"));
  assert(!poset_test(p1, "q", "p"));
  assert(poset_test(p1, "q", "q"));
  assert(poset_test(p1, "q", "r"));
  assert(!poset_test(p1, "r", "a"));
  assert(!poset_test(p1, "r", "b"));
  assert(!poset_test(p1, "r", "c"));
  assert(!poset_test(p1, "r", "p"));
  assert(!poset_test(p1, "r", "q"));
  assert(poset_test(p1, "r", "r"));

  assert(poset_test(p2, "a", "a"));
  assert(!poset_test(p2, "a", "b"));
  assert(poset_test(p2, "a", "c"));
  assert(poset_test(p2, "a", "d"));
  assert(!poset_test(p2, "b", "a"));
  assert(!poset_test(p2, "b", "b"));
  assert(!poset_test(p2, "b", "c"));
  assert(!poset_test(p2, "b", "d"));
  assert(!poset_test(p2, "c", "a"));
  assert(!poset_test(p2, "c", "b"));
  assert(poset_test(p2, "c", "c"));
  assert(poset_test(p2, "c", "d"));
  assert(!poset_test(p2, "d", "a"));
  assert(!poset_test(p2, "d", "b"));
  assert(!poset_test(p2, "d", "c"));
  assert(poset_test(p2, "d", "d"));

  assert(poset_test(p3, "w", "w"));
  assert(poset_test(p3, "w", "x"));
  assert(poset_test(p3, "w", "y"));
  assert(poset_test(p3, "w", "z"));
  assert(!poset_test(p3, "x", "w"));
  assert(poset_test(p3, "x", "x"));
  assert(!poset_test(p3, "x", "y"));
  assert(poset_test(p3, "x", "z"));
  assert(!poset_test(p3, "y", "w"));
  assert(!poset_test(p3, "y", "x"));
  assert(poset_test(p3, "y", "y"));
  assert(poset_test(p3, "y", "z"));
  assert(!poset_test(p3, "z", "w"));
  assert(!poset_test(p3, "z", "x"));
  assert(!poset_test(p3, "z", "y"));
  assert(poset_test(p3, "z", "z"));

  assert(poset_size(p1) == 6);
  assert(poset_size(p2) == 4);
  assert(poset_size(p3) == 4);

  poset_delete(p1);
  poset_delete(p2);
  poset_delete(p3);

  p1 = poset_new();
  assert(poset_insert(p1, ""));
  assert(poset_test(p1, "", ""));
  assert(poset_size(p1) == 1);
  poset_delete(p1);
  assert(!poset_test(p1, "", ""));

  p2 = poset_new();
  assert(poset_insert(p2, "1"));
  assert(poset_insert(p2, "2"));
  assert(poset_insert(p2, "3"));
  assert(poset_add(p2, "1", "2"));
  assert(poset_add(p2, "2", "3"));
  assert(poset_del(p2, "2", "3"));
  assert(poset_test(p2, "1", "3"));
  poset_delete(p2);

  p2 = poset_new();
  assert(poset_insert(p2, "a"));
  assert(poset_insert(p2, "b"));
  assert(poset_insert(p2, "c"));
  assert(poset_add(p2, "a", "b"));
  assert(poset_add(p2, "b", "c"));
  assert(poset_del(p2, "a", "b"));
  assert(!poset_test(p2, "a", "b"));
  assert(poset_test(p2, "a", "c"));
  assert(poset_test(p2, "b", "c"));
  poset_delete(p2);

  p3 = poset_new();
  assert(poset_insert(p3, "a"));
  assert(poset_insert(p3, "b"));
  assert(poset_insert(p3, "c"));
  assert(poset_add(p3, "a", "b"));
  assert(poset_add(p3, "b", "c"));
  assert(poset_remove(p3, "b"));
  assert(poset_test(p3, "a", "c"));
  poset_clear(p3);
  assert(poset_insert(p3, "a"));
  assert(poset_insert(p3, "b"));
  assert(poset_insert(p3, "c"));
  assert(poset_insert(p3, "d"));
  assert(poset_add(p3, "a", "b"));
  assert(poset_add(p3, "b", "c"));
  assert(poset_add(p3, "c", "d"));
  assert(poset_remove(p3, "c"));
  assert(poset_del(p3, "b", "d"));
  assert(poset_test(p3, "a", "a"));
  assert(poset_test(p3, "a", "b"));
  assert(!poset_test(p3, "a", "c"));
  assert(poset_test(p3, "a", "d"));
  assert(!poset_test(p3, "b", "a"));
  assert(poset_test(p3, "b", "b"));
  assert(!poset_test(p3, "b", "c"));
  assert(!poset_test(p3, "b", "d"));
  assert(!poset_test(p3, "c", "a"));
  assert(!poset_test(p3, "c", "b"));
  assert(!poset_test(p3, "c", "c"));
  assert(!poset_test(p3, "c", "d"));
  assert(!poset_test(p3, "d", "a"));
  assert(!poset_test(p3, "d", "b"));
  assert(!poset_test(p3, "d", "c"));
  assert(poset_test(p3, "d", "d"));
  poset_delete(p3);

  p1 = poset_new();
  assert(poset_insert(p1, "a"));
  assert(poset_insert(p1, "b"));
  assert(poset_insert(p1, "c"));
  assert(poset_insert(p1, "d"));
  assert(poset_add(p1, "a", "b"));
  assert(poset_add(p1, "c", "d"));
  assert(poset_add(p1, "b", "c"));
  assert(poset_test(p1, "a", "d"));
  poset_delete(p1);

  return 0;
}
