#include "poset.h"

#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <unordered_map>
#include <algorithm>
#include <bitset>
#include <queue>
#include <math.h>
#include <assert.h>

#ifdef NDEBUG
const bool debug = false;
#else
const bool debug = true;
#endif

#define debug if (!debug) {} else std::cerr << __func__
#define prettyString(str) "\"" << ((str) == nullptr ? "NULL" : (str)) << "\""
#define bracketString(str) "(" << ((str) == nullptr ? "NULL" : (str)) << ")"


namespace {
    using id_type = unsigned long;

    int globalCounter = 0;

    std::vector<id_type> &get_free_ids() {
        static std::vector<id_type> *free_ids = new std::vector<id_type>();
        return *free_ids;
    }

    std::map<id_type, std::map<std::string, std::set<std::string>>> &get_graph() {
        static std::map<id_type, std::map<std::string, std::set<std::string>>> *graph = new std::map<id_type, std::map<std::string, std::set<std::string>>>();
        return *graph;
    }

    std::map<id_type, std::map<std::string, std::set<std::string>>> &get_transposed_graph() {
        static std::map<id_type, std::map<std::string, std::set<std::string>>> *transposed_graph = new std::map<id_type, std::map<std::string, std::set<std::string>>>();
        return *transposed_graph;
    }

    std::map<id_type, size_t> &get_poset_active_elements() {
        static std::map<id_type, size_t> *poset_active_elements = new std::map<id_type, size_t>();
        return *poset_active_elements;
    }

    std::map<id_type, std::map<std::string, bool>> &get_element_exists() {
        static std::map<id_type, std::map<std::string, bool>> *element_exists = new std::map<id_type, std::map<std::string, bool>>();
        return *element_exists;
    }

    std::map<id_type, bool> &get_poset_exists() {
        static std::map<id_type, bool> *poset_exists = new std::map<id_type, bool>();
        return *poset_exists;
    }

    std::map<std::string, bool> &get_visited() {
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
            if (!get_visited()[v]) {
                dfs(v, id);
            }
        }

        return;
    }

}


namespace jnp1 {

    id_type poset_new(void) {
        debug << "()" << std::endl;

        id_type newId;

        if (!get_free_ids().empty()) {
            newId = get_free_ids().back();
            get_free_ids().pop_back();
        } else {
            globalCounter += 1;
            newId = globalCounter;
        }

        get_poset_exists()[newId] = true;
        debug << ": poset " << newId << " created" << std::endl;
        return newId;
    }

    void poset_delete(id_type id) {
        debug << "(" << id << ")" << std::endl;

        if (get_poset_exists()[id]) {
            get_poset_exists()[id] = false;
            get_poset_active_elements()[id] = 0;
            get_element_exists()[id].clear();
            get_graph()[id].clear();
            get_transposed_graph()[id].clear();

            get_free_ids().push_back(id);

            debug << ": poset " << id << " deleted" << std::endl;
        } else {
            debug << ": poset " << id << " does not exist" << std::endl;
        }
    }

    size_t poset_size(id_type id) {
        debug << "(" << id << ")" << std::endl;

        if (get_poset_exists()[id]) {
            debug << ": poset " << id << " contains " << get_poset_active_elements()[id] << " element(s)" << std::endl;
            return get_poset_active_elements()[id];
        } else {
            debug << ": poset " << id << " does not exist" << std::endl;
            return 0;
        }
    }

    bool poset_insert(id_type id, char const *value) {
        debug << "(" << id << ", " << prettyString(value) << ")" << std::endl;
        if (get_poset_exists()[id]) {
            if (!check_name(value)) {
                debug << ": invalid value " << prettyString(value) << std::endl;
                return false;
            }

            std::string copiedName = copyName(value);

            if (!get_element_exists()[id][copiedName]) {
                get_element_exists()[id][copiedName] = true;
                get_poset_active_elements()[id] += 1;

                add_relation(id, copiedName, copiedName);

                debug << ": poset " << id << ", element " << prettyString(value) << " inserted" << std::endl;
                return true;
            } else {
                debug << ": poset " << id << ", element " << prettyString(value) << " already exists" << std::endl;
                return false;
            }
        } else {
            debug << ": poset " << id << " does not exists" << std::endl;
            return false;
        }
    }

    bool poset_remove(id_type id, char const *value) {
        debug << "(" << id << ", " << prettyString(value) << ")" << std::endl;
        if (get_poset_exists()[id]) {
            if (!check_name(value)) {
                debug << ": invalid value " << prettyString(value) << std::endl;
                return false;
            }

            std::string name = copyName(value);

            if (!get_element_exists()[id][name]) {
                debug << ": poset " << id << ", element " << prettyString(value) << " does not exists" << std::endl;
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

            debug << ": poset " << id << ", element " << prettyString(value) << " removed" << std::endl;
            return true;
        } else {
            debug << ": poset " << id << " does not exist" << std::endl;
            return false;
        }
    }


    bool poset_add(id_type id, char const *value1, char const *value2) {
        debug << "(" << id << ", " << prettyString(value1) << ", " << prettyString(value2) << ")" << std::endl;
        if (get_poset_exists()[id]) {
            if (!check_name(value1) || !check_name(value2)) {
                if (!check_name(value1)) {
                    debug << ": invalid value1 " << bracketString(value1) << std::endl;
                }
                if (!check_name(value2)) {
                    debug << ": invalid value2 " << bracketString(value2) << std::endl;
                }
                return false;
            }

            std::string name1 = copyName(value1);
            std::string name2 = copyName(value2);

            if (!get_element_exists()[id][name1] ||
                !get_element_exists()[id][name2]) {
                debug << ": poset " << id << ", element " << prettyString(value1) << " or " << prettyString(value2)
                      << " does not exist" << std::endl;
                return false;
            }

            if (get_graph()[id][name1].find(name2) !=
                get_graph()[id][name1].end() ||
                get_graph()[id][name2].find(name1) !=
                get_graph()[id][name2].end()) {
                debug << ": poset " << id << ", relation (" << prettyString(value1) << ", " << prettyString(value2)
                      << ") cannot be added" << std::endl;
                return false;
            } else {
                for (auto v:get_transposed_graph()[id][name1]) {
                    for (auto u:get_graph()[id][name2]) {
                        add_relation(id, v, u);
                    }
                }
                debug << ": poset " << id << ", relation (" << prettyString(value1) << ", " << prettyString(value2)
                      << ") added" << std::endl;
                return true;
            }

        } else {
            debug << ": poset " << id << " does not exist" << std::endl;
            return false;
        }
    }

    bool poset_del(id_type id, char const *value1, char const *value2) {
        debug << "(" << id << ", " << prettyString(value1) << ", " << prettyString(value2) << ")" << std::endl;

        if (get_poset_exists()[id]) {
            if (!check_name(value1) || !check_name(value2)) {
                if (!check_name(value1)) {
                    debug << ": invalid value1 " << bracketString(value1) << std::endl;
                }
                if (!check_name(value2)) {
                    debug << ": invalid value2 " << bracketString(value2) << std::endl;
                }
                return false;
            }

            std::string name1 = copyName(value1);
            std::string name2 = copyName(value2);

            if (!get_element_exists()[id][name1] ||
                !get_element_exists()[id][name2]) {
                debug << ": poset " << id << ", element " << prettyString(value1) << " or " << prettyString(value2)
                      << " does not exist" << std::endl;
                return false;
            }

            if (get_graph()[id][name1].find(name2) !=
                get_graph()[id][name1].end()) {
                delete_relation(id, name1, name2);

                get_visited().clear();

                dfs(name1, id);

                if (get_visited()[name2]) {
                    debug << ": poset " << id << ", relation (" << prettyString(value1) << ", " << prettyString(value2)
                          << ") cannot be deleted" << std::endl;
                    add_relation(id, name1, name2);
                    return false;
                } else {
                    debug << ": poset " << id << ", relation (" << prettyString(value1) << ", " << prettyString(value2)
                          << ") deleted" << std::endl;
                    return true;
                }
            } else {
                debug << ": poset " << id << ", relation (" << prettyString(value1) << ", " << prettyString(value2)
                      << ") does not exist" << std::endl;
                return false;
            }
        } else {
            debug << ": poset " << id << " does not exist" << std::endl;
            return false;
        }
    }

    bool poset_test(id_type id, char const *value1, char const *value2) {
        debug << "(" << id << ", " << prettyString(value1) << ", " << prettyString(value2) << ")" << std::endl;
        if (get_poset_exists()[id]) {
            if (!check_name(value1) || !check_name(value2)) {
                if (!check_name(value1)) {
                    debug << ": invalid value1 " << bracketString(value1) << std::endl;
                }
                if (!check_name(value2)) {
                    debug << ": invalid value2 " << bracketString(value2) << std::endl;
                }
                return false;
            }

            std::string name1 = copyName(value1);
            std::string name2 = copyName(value2);

            if (!get_element_exists()[id][name1] ||
                !get_element_exists()[id][name2]) {
                debug << ": poset " << id << ", element " << prettyString(value1) << " or " << prettyString(value2)
                      << " does not exist" << std::endl;
                return false;
            }

            if (get_graph()[id][name1].find(name2) !=
                get_graph()[id][name1].end()) {
                debug << ": poset " << id << ", relation (" << prettyString(value1) << ", " << prettyString(value2)
                      << ") exists" << std::endl;
                return true;
            } else {
                debug << ": poset " << id << ", relation (" << prettyString(value1) << ", " << prettyString(value2)
                      << ") does not exists" << std::endl;
                return false;
            }
        } else {
            debug << ": poset " << id << " does not exist" << std::endl;
            return false;
        }
    }

    void poset_clear(id_type id) {
        debug << "(" << id << ")" << std::endl;
        if (get_poset_exists()[id]) {
            get_poset_active_elements()[id] = 0;
            get_element_exists()[id].clear();
            get_graph()[id].clear();
            get_transposed_graph()[id].clear();
            debug << ": poset " << id << " cleared" << std::endl;
            return;
        } else {
            debug << ": poset " << id << " does not exist" << std::endl;
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
    return 0;
}