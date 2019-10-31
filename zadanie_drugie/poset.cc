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
const bool debugEnabled = false;
#else
const bool debugEnabled = true;
#endif

#define debugStream if (!debugEnabled) {} else std::cerr << __func__

namespace {
    using id_type = unsigned long;
    using free_id_vector = std::vector<id_type>;
    using string_set = std::set<std::string>;
    using graph_type = std::map<id_type, std::map<std::string, string_set>>;
    using active_elements_map = std::map<id_type, size_t>;
    using element_exist_map = std::map<id_type, std::map<std::string, bool>>;
    using poset_exsit_map = std::map<id_type, bool>;
    using visited_map = std::map<std::string, bool>;

    int globalCounter = 0;

    // Gettery do globalnych zmiennych

    free_id_vector &get_free_ids() {
        static free_id_vector *free_ids = new free_id_vector();
        return *free_ids;
    }

    graph_type &get_graph() {
        static graph_type *graph = new graph_type();
        return *graph;
    }

    graph_type &get_transposed_graph() {
        static graph_type *transposed_graph = new graph_type();
        return *transposed_graph;
    }

    active_elements_map &get_poset_active_elements() {
        static active_elements_map *poset_active_elements =
                new active_elements_map();
        return *poset_active_elements;
    }

    element_exist_map &get_element_exists() {
        static element_exist_map *element_exists = new element_exist_map();
        return *element_exists;
    }

    poset_exsit_map &get_poset_exists() {
        static poset_exsit_map *poset_exists = new poset_exsit_map();
        return *poset_exists;
    }

    visited_map &get_visited() {
        static visited_map *visited = new visited_map();
        return *visited;
    }

    // Funkcje pomocnicze

    // Funkcja pomocnicza do skopiowania nazwy elementu
    std::string copyName(char const *value) {
        int i = 0;
        std::string copiedName = "";

        while (value[i] != '\0') {
            copiedName += value[i];
            i++;
        }

        return copiedName;
    }

    // Funkcja sprawdzająca poprawność parametru nazwy
    bool check_name(char const *value) {
        if (value == NULL) {
            return false;
        } else {
            return true;
        }
    }

    // Funkcje służące do zwracania stringa w formacie do logów debugu

    std::string framed_string(char const *value, std::string frame_st,
                              std::string frame_nd) {
        if (value == NULL) {
            return frame_st + "NULL" + frame_nd;
        } else {
            return frame_st + copyName(value) + frame_nd;
        }
    }

    std::string quotation_mark_string(char const *value) {
        return framed_string(value, "\"", "\"");
    }

    std::string bracket_string(char const *value) {
        return framed_string(value, "(", ")");
    }

    // Funckje pomocnicze do obsługi posetu

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
        debugStream << "()" << std::endl;

        id_type newId;

        if (!get_free_ids().empty()) {
            newId = get_free_ids().back();
            get_free_ids().pop_back();
        } else {
            globalCounter += 1;
            newId = globalCounter;
        }

        get_poset_exists()[newId] = true;
        debugStream << ": poset " << newId << " created" << std::endl;
        return newId;
    }

    void poset_delete(id_type id) {
        debugStream << "(" << id << ")" << std::endl;

        if (get_poset_exists()[id]) {
            get_poset_exists()[id] = false;
            get_poset_active_elements()[id] = 0;
            get_element_exists()[id].clear();
            get_graph()[id].clear();
            get_transposed_graph()[id].clear();

            get_free_ids().push_back(id);

            debugStream << ": poset " << id << " deleted" << std::endl;
        } else {
            debugStream << ": poset " << id << " does not exist" << std::endl;
        }
    }

    size_t poset_size(id_type id) {
        debugStream << "(" << id << ")" << std::endl;

        if (get_poset_exists()[id]) {
            debugStream << ": poset " << id << " contains "
                        << get_poset_active_elements()[id] << " element(s)"
                        << std::endl;
            return get_poset_active_elements()[id];
        } else {
            debugStream << ": poset " << id << " does not exist" << std::endl;
            return 0;
        }
    }

    bool poset_insert(id_type id, char const *value) {
        debugStream << "(" << id << ", " << quotation_mark_string(value) << ")"
                    << std::endl;
        if (get_poset_exists()[id]) {
            if (!check_name(value)) {
                debugStream << ": invalid value "
                            << quotation_mark_string(value) << std::endl;
                return false;
            }

            std::string copiedName = copyName(value);

            if (!get_element_exists()[id][copiedName]) {
                get_element_exists()[id][copiedName] = true;
                get_poset_active_elements()[id] += 1;

                add_relation(id, copiedName, copiedName);

                debugStream << ": poset " << id << ", element "
                            << quotation_mark_string(value) << " inserted"
                            << std::endl;
                return true;
            } else {
                debugStream << ": poset " << id << ", element "
                            << quotation_mark_string(value) << " already exists"
                            << std::endl;
                return false;
            }
        } else {
            debugStream << ": poset " << id << " does not exists" << std::endl;
            return false;
        }
    }

    bool poset_remove(id_type id, char const *value) {
        debugStream << "(" << id << ", " << quotation_mark_string(value) << ")"
                    << std::endl;
        if (get_poset_exists()[id]) {
            if (!check_name(value)) {
                debugStream << ": invalid value "
                            << quotation_mark_string(value) << std::endl;
                return false;
            }

            std::string name = copyName(value);

            if (!get_element_exists()[id][name]) {
                debugStream << ": poset " << id << ", element "
                            << quotation_mark_string(value)
                            << " does not exists" << std::endl;
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

            debugStream << ": poset " << id << ", element "
                        << quotation_mark_string(value) << " removed"
                        << std::endl;
            return true;
        } else {
            debugStream << ": poset " << id << " does not exist" << std::endl;
            return false;
        }
    }


    bool poset_add(id_type id, char const *value1, char const *value2) {
        debugStream << "(" << id << ", " << quotation_mark_string(value1)
                    << ", " << quotation_mark_string(value2) << ")"
                    << std::endl;
        if (get_poset_exists()[id]) {
            if (!check_name(value1) || !check_name(value2)) {
                if (!check_name(value1)) {
                    debugStream << ": invalid value1 " << bracket_string(value1)
                                << std::endl;
                }
                if (!check_name(value2)) {
                    debugStream << ": invalid value2 " << bracket_string(value2)
                                << std::endl;
                }
                return false;
            }

            std::string name1 = copyName(value1);
            std::string name2 = copyName(value2);

            if (!get_element_exists()[id][name1] ||
                !get_element_exists()[id][name2]) {
                debugStream << ": poset " << id << ", element "
                            << quotation_mark_string(value1) << " or "
                            << quotation_mark_string(value2)
                            << " does not exist" << std::endl;
                return false;
            }

            if (get_graph()[id][name1].find(name2) !=
                get_graph()[id][name1].end() ||
                get_graph()[id][name2].find(name1) !=
                get_graph()[id][name2].end()) {
                debugStream << ": poset " << id << ", relation ("
                            << quotation_mark_string(value1) << ", "
                            << quotation_mark_string(value2)
                            << ") cannot be added" << std::endl;
                return false;
            } else {
                for (auto v:get_transposed_graph()[id][name1]) {
                    for (auto u:get_graph()[id][name2]) {
                        add_relation(id, v, u);
                    }
                }
                debugStream << ": poset " << id << ", relation ("
                            << quotation_mark_string(value1) << ", "
                            << quotation_mark_string(value2)
                            << ") added" << std::endl;
                return true;
            }

        } else {
            debugStream << ": poset " << id << " does not exist" << std::endl;
            return false;
        }
    }

    bool poset_del(id_type id, char const *value1, char const *value2) {
        debugStream << "(" << id << ", " << quotation_mark_string(value1)
                    << ", " << quotation_mark_string(value2) << ")"
                    << std::endl;

        if (get_poset_exists()[id]) {
            if (!check_name(value1) || !check_name(value2)) {
                if (!check_name(value1)) {
                    debugStream << ": invalid value1 " << bracket_string(value1)
                                << std::endl;
                }
                if (!check_name(value2)) {
                    debugStream << ": invalid value2 " << bracket_string(value2)
                                << std::endl;
                }
                return false;
            }

            std::string name1 = copyName(value1);
            std::string name2 = copyName(value2);

            if (!get_element_exists()[id][name1] ||
                !get_element_exists()[id][name2]) {
                debugStream << ": poset " << id << ", element "
                            << quotation_mark_string(value1) << " or "
                            << quotation_mark_string(value2)
                            << " does not exist" << std::endl;
                return false;
            }

            if (get_graph()[id][name1].find(name2) !=
                get_graph()[id][name1].end()) {
                delete_relation(id, name1, name2);

                get_visited().clear();

                dfs(name1, id);

                if (get_visited()[name2]) {
                    debugStream << ": poset " << id << ", relation ("
                                << quotation_mark_string(value1) << ", "
                                << quotation_mark_string(value2)
                                << ") cannot be deleted" << std::endl;
                    add_relation(id, name1, name2);
                    return false;
                } else {
                    debugStream << ": poset " << id << ", relation ("
                                << quotation_mark_string(value1) << ", "
                                << quotation_mark_string(value2)
                                << ") deleted" << std::endl;
                    return true;
                }
            } else {
                debugStream << ": poset " << id << ", relation ("
                            << quotation_mark_string(value1) << ", "
                            << quotation_mark_string(value2)
                            << ") does not exist" << std::endl;
                return false;
            }
        } else {
            debugStream << ": poset " << id << " does not exist" << std::endl;
            return false;
        }
    }

    bool poset_test(id_type id, char const *value1, char const *value2) {
        debugStream << "(" << id << ", " << quotation_mark_string(value1)
                    << ", " << quotation_mark_string(value2) << ")"
                    << std::endl;
        if (get_poset_exists()[id]) {
            if (!check_name(value1) || !check_name(value2)) {
                if (!check_name(value1)) {
                    debugStream << ": invalid value1 " << bracket_string(value1)
                                << std::endl;
                }
                if (!check_name(value2)) {
                    debugStream << ": invalid value2 " << bracket_string(value2)
                                << std::endl;
                }
                return false;
            }

            std::string name1 = copyName(value1);
            std::string name2 = copyName(value2);

            if (!get_element_exists()[id][name1] ||
                !get_element_exists()[id][name2]) {
                debugStream << ": poset " << id << ", element "
                            << quotation_mark_string(value1) << " or "
                            << quotation_mark_string(value2)
                            << " does not exist" << std::endl;
                return false;
            }

            if (get_graph()[id][name1].find(name2) !=
                get_graph()[id][name1].end()) {
                debugStream << ": poset " << id << ", relation ("
                            << quotation_mark_string(value1) << ", "
                            << quotation_mark_string(value2)
                            << ") exists" << std::endl;
                return true;
            } else {
                debugStream << ": poset " << id << ", relation ("
                            << quotation_mark_string(value1) << ", "
                            << quotation_mark_string(value2)
                            << ") does not exists" << std::endl;
                return false;
            }
        } else {
            debugStream << ": poset " << id << " does not exist" << std::endl;
            return false;
        }
    }

    void poset_clear(id_type id) {
        debugStream << "(" << id << ")" << std::endl;
        if (get_poset_exists()[id]) {
            get_poset_active_elements()[id] = 0;
            get_element_exists()[id].clear();
            get_graph()[id].clear();
            get_transposed_graph()[id].clear();
            debugStream << ": poset " << id << " cleared" << std::endl;
            return;
        } else {
            debugStream << ": poset " << id << " does not exist" << std::endl;
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