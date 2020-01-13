// Anita Śledź as406384
// Damian Chańko dc394127


#ifndef INSERTION_ORDERED_MAP_H
#define INSERTION_ORDERED_MAP_H


#include <functional>
#include <unordered_map>
#include <utility>
#include<algorithm>
#include <list>
#include <memory>
#include<iostream>
#include<queue>
#include <type_traits>


class lookup_error : public std::exception {
    const char *what() const noexcept override {
        return "Lookup error.";
    }
};

template<class K, class V, class Hash = std::hash<K>>
class insertion_ordered_map {

private:
    /****** Parametry ******/

    size_t map_size;

    std::shared_ptr<std::list<std::pair<K, V>>> list;
    std::shared_ptr<std::unordered_map<K, typename std::list<std::pair<K, V>>::iterator, Hash>> map;

    bool flag = false;

    /***** Util funkcje ******/

    insertion_ordered_map &ref() {
        return *this;
    }

    // Bezpieczna funkcja kopiująca na wypadki. O(n).
    void copy_if_have_to(bool force_copy) {
    	std::list<std::pair<K, V>> *backup_list = list.get();
        std::unordered_map<K, typename std::list<std::pair<K, V>>::iterator, Hash> *backup_map = map.get();
        try {
        	if ((!this->list.unique() || !this->map.unique()) || force_copy == true) {
	            std::unordered_map<K, typename std::list<std::pair<K, V>>::iterator, Hash> nMap;
	            for (auto it = this->list->begin(); it != this->list->end(); it++) {
	                nMap.insert({it->first, it});
	            }
	            std::list<std::pair<K, V>> newList(*list);

	            map.reset();
	            list.reset();
	            map = std::make_shared<std::unordered_map<K, typename std::list<std::pair<K, V>>::iterator, Hash>>(nMap);
	            list = std::make_shared<std::list<std::pair<K, V>>>(newList);

	            this->flag = false;
        	}
        }
        catch (std::exception &e) {
            list.reset(backup_list);
            map.reset(backup_map);
            throw e;
        }
    }

public:
    /****** Konstruktory ******/

    //Konstruktor bezparametrowy tworzący pusty słownik. O(1).
    insertion_ordered_map() {
        map_size = 0;
        list = std::make_shared<std::list<std::pair<K, V>>>();
        map = std::make_shared<std::unordered_map<K, typename std::list<std::pair<K, V>>::iterator, Hash> >();
    }

    // Konstruktor kopiujący. O(1).
    insertion_ordered_map(insertion_ordered_map const &other) {
        if (!other.flag) {
            this->list = other.list;
            this->map = other.map;
            this->map_size = other.map_size;
            this->flag = false;
        } else {
            try {
                list = std::make_shared<std::list<std::pair<K, V>>>(std::list<std::pair<K, V>>(*other.list));

                std::unordered_map<K, typename std::list<std::pair<K, V>>::iterator, Hash> nMap;
                for (auto it = this->list->begin(); it != this->list->end(); it++) {
                    nMap.insert({it->first, it});
                }
                map.reset();
                map = std::make_shared<std::unordered_map<K, typename std::list<std::pair<K, V>>::iterator, Hash>>(
                        nMap);

                flag = false;
            } catch (std::exception &e) {
                map.reset();
                throw e;
            }
        }
    }

    // Konstruktor przenoszący. O(1).
    insertion_ordered_map(insertion_ordered_map &&other) {
        this->list = std::move(other.list);
        this->map = std::move(other.map);
        this->flag = other.flag;
        this->map_size = this->list->size();
    }


    /****** Operatory ******/


    // Operator przypisania. O(1)
    insertion_ordered_map &operator=(insertion_ordered_map other) {
        this->list = other.list;
        this->map = other.map;
        this->flag = other.flag;
        this->map_size = other.map_size;
        return ref();
    }

    // Referencje wartości. O(1).
    V &at(K const &k) {
        copy_if_have_to(true);
        if (this->map->find(k) == this->map->end()) {
            insert(k, V());
        }
        copy_if_have_to(false);
        this->flag = true;
        return map->at(k)->second;
    }

    V const &at(K const &k) const {
        return this->map->at(k)->second;
    }

    // Sprawdzenie niepustości słownika. O(1).
    bool empty() const {
        return this->map_size == 0;
    }

    size_t size() const {
        return this->list->size();
    }

    // Operator indeksowania, jeśli elementu brakuje - dodaje do mapy. O(1).
    V &operator[](K const &k) {
        copy_if_have_to(false);
        if (this->map->find(k) == this->map->end()) {
            insert(k, V{});
        }
        flag = true;
        return map->at(k)->second;
    }

    /****** Inne ******/

    // Wtawianie do słownika. O(1).
    bool insert(K const &k, V const &v) {
        copy_if_have_to(false);
        if (this->map->find(k) == map->end()) {
            this->list->push_back({k, v});
            auto iter = this->list->end();
            iter--;
            this->map->insert({k, iter});
            this->map_size++;
            return true;
        } else {

            V val = (*(this->map->find(k)->second)).second;

            erase(k);
            insert(k, val);
            return false;
        }
    }

    // Usuwanie ze słownika. O(1).
    void erase(K const &k) {
        copy_if_have_to(false);
        if (this->map->find(k) == this->map->end()) {
            throw lookup_error();
        } else {
            V val = (*(this->map->find(k)->second)).second;

            this->list->remove({k, val});
            this->map->erase(k);
            this->map_size--;
        }
    }

    // Merge z drugim słownikiem. O(n + m), gdzie n i m to rozmiary.
    void merge(insertion_ordered_map const &other) {
        copy_if_have_to(true);
        std::vector<std::pair<V, K>> q;
        for (auto elem : *this->list) {
            if (other.map->find(elem.first) != other.map->end()) {
                q.push_back({elem.first, elem.second});
            }
        }
        for (size_t i = 0; i < q.size(); i++) {
            erase(q[i].first);
            insert(q[i].first, q[i].second);
        }
        q.clear();
        for (auto elem : *other.list) {
            if (this->map->find(elem.first) == this->map->end()) {
                insert(elem.first, elem.second);
            }
        }
        this->map_size = this->list->size();
    }

    void clear() {
        copy_if_have_to(false);
        this->list->clear();
        this->map->clear();
        this->flag = true;
        this->map_size = 0;
    }

    bool contains(K const &k) const {
        return this->map->find(k) != map->end();
    }

    /****** ITERATOR ******/

    class iterator {
        typename std::list<std::pair<K, V>>::const_iterator it;
        friend insertion_ordered_map;
    public:
        const std::pair<K, V> &operator*() const {
            return *it;
        }
        const std::pair<K, V> *operator->() const {
            return &(*it);
        }
        iterator() = default;
        iterator(iterator const &other) : it(other.it) {}
        iterator &operator++() {
            ++it;
            return *this;
        }
        bool operator==(const iterator &other) const {
            return it == other.it;
        }
        bool operator!=(const iterator &other) const {
            return !(other == *this);
        }
    };

    iterator begin() const {
        iterator iterator;
        iterator.it = this->list->cbegin();
        return iterator;
    }

    iterator end() const {
        iterator iterator;
        iterator.it = this->list->cend();
        return iterator;
    }

};


#endif //INSERTION_ORDERED_MAP_H