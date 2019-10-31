#include "poset.h"

#include<iostream>
#include<vector>
#include<map>
#include<set>
#include<unordered_map>
#include<algorithm>
#include<bitset>
#include<queue>
#include <math.h>
#include <assert.h>

#ifdef NDEBUG
const bool debug = false;
#else
const bool debug = true;
#endif

#define debugStream if (!debug) {} else std::cerr 


namespace {
    using id_type = unsigned long;

    int globalCounter = 0;

    std::vector<id_type> &get_free_ids () {
        static std::vector<id_type> *free_ids = new std::vector<id_type>();
        return *free_ids;
    }

    std::map<id_type, std::map<std::string, std::set<std::string>>> &get_graph () {
        static std::map<id_type, std::map<std::string, std::set<std::string>>> *graph = new std::map<id_type, std::map<std::string, std::set<std::string>>>();
        return *graph;
    }

    std::map<id_type, std::map<std::string, std::set<std::string>>> &get_transposed_graph () {
        static std::map<id_type, std::map<std::string, std::set<std::string>>> *transposed_graph = new std::map<id_type, std::map<std::string, std::set<std::string>>>();
        return *transposed_graph;
    }

    std::map<id_type, size_t> &get_poset_active_elements () {
        static std::map<id_type, size_t> *poset_active_elements = new std::map<id_type, size_t>();
        return *poset_active_elements;
    }

    std::map<id_type, std::map<std::string, bool>> &get_element_exists () {
        static std::map<id_type, std::map<std::string, bool>> *element_exists = new std::map<id_type, std::map<std::string, bool>>();
        return *element_exists;
    }

    std::map<id_type, bool> &get_poset_exists () {
        static std::map<id_type, bool> *poset_exists = new std::map<id_type, bool>();
        return *poset_exists;
    }

    std::map<std::string, bool> &get_visited () {
        static std::map<std::string, bool> *visited = new std::map<std::string, bool>();
        return *visited;
    }

    // funkcja pomocnicza do skopiowania nazwy elementu
    std::string copyName(char const *value) {
        int i = 0;
        std::string copiedName = "";

        while (value[i] != '\0') {
            copiedName += value[i];
            i++;
        }

        return copiedName;
    }

    bool check_name(char const *value) {
        if (value == NULL) {
            // niepoprawna nazwa
            return false;
        } else {
            return true;
        }
    }

    void add_relation(id_type id, std::string name1, std::string name2) {
        get_graph()[id][name1].insert(name2);
        get_transposed_graph()[id][name2].insert(name1);

        return;
    }

    void delete_relation(id_type id, std::string name1, std::string name2) {
        auto it1 = get_graph()[id][name1].find(name2);
        get_graph()[id][name1].erase(it1);

        auto it2 = get_transposed_graph()[id][name2].find(name1);
        get_transposed_graph()[id][name2].erase(it2);

        return;
    }

    void dfs(std::string node, id_type id) {
        get_visited()[node] = true;

        for (auto v:get_graph()[id][node]) {
            if (get_visited()[v] == false) {
                dfs(v, id);
            }
        }

        return;
    }

}


namespace jnp1 {

    id_type poset_new(void) {
        debugStream << "poset_new()" << std::endl;
        
        id_type newId;

        if (get_free_ids().empty() == false) {
            newId = get_free_ids().back();
            get_free_ids().pop_back();
        } else {
            globalCounter += 1;
            newId = globalCounter;
        }

        get_poset_exists()[newId] = true;

        return newId;
    }

    void poset_delete(id_type id) {
        if (get_poset_exists()[id] == true) {
            get_poset_exists()[id] = false;
            get_poset_active_elements()[id] = 0;
            get_element_exists()[id].clear();
            get_graph()[id].clear();
            get_transposed_graph()[id].clear();

            get_free_ids().push_back(id);

            // pomyslnie usunieto poset
        } else {
            // poset o takim numerze nie istnieje
        }
    }

    size_t poset_size(id_type id) {
        if (get_poset_exists()[id] == true) {
            return get_poset_active_elements()[id];
        } else {
            // poset o takim numerze nie istnieje
            return 0;
        }
    }

    bool poset_insert(id_type id, char const *value) {
        if (get_poset_exists()[id] == true) {
            if (check_name(value) == false) {
                return false;
            }

            std::string copiedName = copyName(value);

            if (get_element_exists()[id][copiedName] == false) {
                get_element_exists()[id][copiedName] = true;
                get_poset_active_elements()[id] += 1;

                add_relation(id, copiedName, copiedName);

                // wierzcholek pomyslnie dodany
                return true;
            } else {
                // taki element juz istnieje w danym posecie
                return false;
            }
        } else {
            // poset o takim id nie istnieje
            return false;
        }
    }

    bool poset_remove(id_type id, char const *value) {
        if (get_poset_exists()[id]) {
            if (check_name(value) == false) {
                return false;
            }

            std::string name = copyName(value);

            if (get_element_exists()[id][name] == false) {
                // element nie istnieje

                return false;
            }

            for (auto v:get_graph()[id][name]) {
                auto it = get_transposed_graph()[id][v].find(name);
                get_transposed_graph()[id][v].erase(it);
            }

            get_graph()[id][name].clear();

            for (auto v:get_transposed_graph()[id][name]) {
                auto it = get_graph()[id][v].find(name);
                get_graph()[id][v].erase(it);
            }

            get_transposed_graph()[id][name].clear();

            get_element_exists()[id][name] = false;

            get_poset_active_elements()[id] -= 1;

            return true;
        } else {
            // poset o tym id nie istnieje
            return false;
        }
    }


    bool poset_add(id_type id, char const *value1, char const *value2) {
        if (get_poset_exists()[id]) {
            if (check_name(value1) == false || check_name(value2) == false) {
                return false;
            }

            std::string name1 = copyName(value1);
            std::string name2 = copyName(value2);

            if (get_element_exists()[id][name1] == false ||
                get_element_exists()[id][name2] == false) {
                // ktorys z elementow nie istnieje

                return false;
            }

            if (get_graph()[id][name1].find(name2) != get_graph()[id][name1].end() ||
                get_graph()[id][name2].find(name1) != get_graph()[id][name2].end()) {
                // relacja nie moze byc dodana
                return false;
            } else {
                for (auto v:get_transposed_graph()[id][name1]) {
                    for (auto u:get_graph()[id][name2]) {
                        add_relation(id, v, u);
                    }
                }

                return true;
            }

        } else {
            // poset o tym id nie istnieje
            return false;
        }
    }

    bool poset_del(id_type id, char const *value1, char const *value2) {
        if (get_poset_exists()[id]) {
            if (check_name(value1) == false || check_name(value2) == false) {
                return false;
            }

            std::string name1 = copyName(value1);
            std::string name2 = copyName(value2);

            if (get_element_exists()[id][name1] == false ||
                get_element_exists()[id][name2] == false) {
                // ktorys z elementow nie istnieje

                return false;
            }

            if (get_graph()[id][name1].find(name2) != get_graph()[id][name1].end()) {
                delete_relation(id, name1, name2);

                get_visited().clear();

                dfs(name1, id);

                if (get_visited()[name2] == true) {
                    // usuniecie relacji zaburzy czesciowy porzadek
                    add_relation(id, name1, name2);

                    return false;
                } else {
                    return true;
                }
            } else {
                // relacja nie istnieje
                return false;
            }
        } else {
            // poset o tym id nie istnieje
            return false;
        }
    }

    bool poset_test(id_type id, char const *value1, char const *value2) {
        if (get_poset_exists()[id]) {
            if (check_name(value1) == false || check_name(value2) == false) {
                return false;
            }

            std::string name1 = copyName(value1);
            std::string name2 = copyName(value2);

            if (get_element_exists()[id][name1] == false ||
                get_element_exists()[id][name2] == false) {
                // ktorys z elementow nie istnieje

                return false;
            }

            if (get_graph()[id][name1].find(name2) != get_graph()[id][name1].end()) {
                // relacja istnieje
                return true;
            } else {
                // relacja nie istnieje
                return false;
            }
        } else {
            // poset o tym id nie istnieje
            return false;
        }
    }

    void poset_clear(id_type id) {
        if (get_poset_exists()[id]) {
            get_poset_active_elements()[id] = 0;
            get_element_exists()[id].clear();
            get_graph()[id].clear();
            get_transposed_graph()[id].clear();

            return;
        } else {
            // poset o takim id nie istnieje
            return;
        }
    }
}


using namespace jnp1;

int32_t main() {
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

    std::cout << "ZAJEBIŚCIE\n";
    return 0;
}