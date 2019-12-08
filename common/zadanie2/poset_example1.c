#include "poset.h"

#ifdef NDEBUG
  #undef NDEBUG
#endif

#include <assert.h>

int main() {
  unsigned long p1;

  p1 = poset_new();
  assert(poset_size(p1) == 0);
  assert(poset_size(p1 + 1) == 0);
  assert(!poset_insert(p1, NULL));
  assert(poset_insert(p1, "A"));
  assert(poset_test(p1, "A", "A"));
  assert(!poset_insert(p1, "A"));
  assert(!poset_insert(p1 + 1, "B"));
  assert(poset_size(p1) == 1);
  assert(!poset_remove(p1 + 1, "A"));
  assert(poset_remove(p1, "A"));
  assert(!poset_remove(p1, "A"));
  assert(poset_insert(p1, "B"));
  assert(poset_insert(p1, "C"));
  assert(poset_add(p1, "B", "C"));
  assert(!poset_remove(p1, "A"));
  assert(!poset_add(p1, NULL, "X"));
  assert(!poset_del(p1, NULL, "X"));
  assert(!poset_test(p1, NULL, "X"));
  assert(!poset_add(p1, "X", NULL));
  assert(!poset_del(p1, "X", NULL));
  assert(!poset_test(p1, "X", NULL));
  assert(!poset_add(p1, NULL, NULL));
  assert(!poset_del(p1, NULL, NULL));
  assert(!poset_test(p1, NULL, NULL));
  assert(!poset_add(p1, "C", "D"));
  assert(!poset_add(p1, "D", "C"));
  assert(!poset_del(p1, "C", "D"));
  assert(!poset_del(p1, "D", "C"));
  assert(!poset_test(p1, "C", "D"));
  assert(!poset_test(p1, "D", "C"));
  assert(!poset_add(p1 + 1, "C", "D"));
  assert(!poset_del(p1 + 1, "C", "D"));
  assert(!poset_test(p1 + 1, "C", "D"));
  poset_clear(p1);
  poset_clear(p1 + 1);
  assert(poset_insert(p1, "E"));
  assert(poset_insert(p1, "F"));
  assert(poset_insert(p1, "G"));
  assert(poset_add(p1, "E", "F"));
  assert(!poset_add(p1, "E", "F"));
  assert(!poset_add(p1, "F", "E"));
  assert(poset_test(p1, "E", "F"));
  assert(!poset_test(p1, "F", "E"));
  assert(poset_add(p1, "F", "G"));
  assert(poset_test(p1, "E", "G"));
  assert(!poset_del(p1, "E", "G"));
  assert(poset_del(p1, "E", "F"));
  assert(!poset_del(p1, "E", "F"));
  assert(!poset_del(p1, "G", "F"));
  assert(!poset_del(p1, "G", "G"));
  assert(poset_size(p1) == 3);
  poset_delete(p1);
  poset_delete(p1);
  poset_delete(p1 + 1);

  return 0;
}
