#include "insertion_ordered_map.h"
#include <cassert>
#include <vector>
#include <iostream>

/*
  svn import insertion_ordered_map.h https://svn.mimuw.edu.pl/repos/jnp1-students/grupa3/zadanie5/as406384+dc394127/insertion_ordered_map.h -m "Dodano rozwiązanie"
*/

namespace {
  auto f(insertion_ordered_map<int, int> q)
  {
    return q;
  }
}

int main()
{
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
  // assert(iom2.contains(2) && iom2.contains(4) && iom2.contains(1));
  insertion_ordered_map<int, int> const iom4 = iom2;
  {
    int order[] = {2, 4, 1};
    int values[] = {2, 10, 1};
    int i = 0;
    for (auto it = iom2.begin(), end = iom2.end(); it != end; ++it, ++i)
      // std::cout << it->first << " " << it->second << "\n";
      assert(it->first == order[i] && it->second == values[i]);
    i = 0;
    for (auto it = iom4.begin(), end = iom4.end(); it != end; ++it, ++i)
      assert(it->first == order[i] && it->second == values[i]);
  }

  auto piom5 = std::make_unique<insertion_ordered_map<int, int>>();

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
}
