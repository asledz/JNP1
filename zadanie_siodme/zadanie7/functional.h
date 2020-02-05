#ifndef FUNCTIONAL_H
#define FUNCTIONAL_H

inline auto compose() {
  return [](auto x) { return x; };
}

template <typename F, typename... TAIL> auto compose(F f, TAIL... functions) {
  return [=](auto x) { return compose(functions...)(f(x)); };
}

template <typename F, typename... TAIL> auto lift(F f, TAIL... functions) {
  return [=](auto x) { return f(functions(x)...); };
}

#endif /* FUNCTIONAL_H */