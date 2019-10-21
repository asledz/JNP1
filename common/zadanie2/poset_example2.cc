#include "poset.h"

#ifdef NDEBUG
  #undef NDEBUG
#endif

#include <cassert>

namespace {
  unsigned long test() {
    unsigned long id = ::jnp1::poset_new();
    ::jnp1::poset_insert(id, "testowy1");
    ::jnp1::poset_insert(id, "testowy2");
    ::jnp1::poset_add(id, "testowy1", "testowy2");
    return id;
  }

  unsigned long id = test();
}

int main() {
  ::jnp1::poset_insert(id, nullptr);
  assert(::jnp1::poset_test(id, "testowy1", "testowy2"));
  assert(!::jnp1::poset_test(id, "testowy2", "testowy1"));
  ::jnp1::poset_delete(id);
}
