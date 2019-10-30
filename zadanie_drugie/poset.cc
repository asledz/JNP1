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

using namespace std;

using id_type = unsigned long;

int globalCounter = 0;

vector <id_type> free_ids;

map <id_type, map<string, set<string>>> graph;
map <id_type, map<string, set<string>>> transposed_graph;

map <id_type, size_t> poset_active_elements;

map <id_type, map<string, bool>> element_exists;

map <id_type, bool> poset_exists;

id_type poset_new(void){
	id_type newId;

	if (free_ids.empty() == false){
		newId = free_ids.back();
		free_ids.pop_back();
	}else{
		globalCounter += 1;
		newId = globalCounter;
	}

	poset_exists[newId] = true;

	return newId;
}

void poset_delete(id_type id){
	if (poset_exists[id] == true){
		poset_exists[id] = false;
		poset_active_elements[id] = 0;
		element_exists[id].clear();
		graph[id].clear();
		transposed_graph[id].clear();

		free_ids.push_back(id);

		// pomyslnie usunieto poset
	}else{
		// poset o takim numerze nie istnieje
	}
}

size_t poset_size(id_type id){
	if (poset_exists[id] == true){
		return poset_active_elements[id];
	}else{
		// poset o takim numerze nie istnieje
		return 0;
	}
}

// funkcja pomocnicza do skopiowania nazwy elementu
string copyName(char const *value){
	int i = 0;
	string copiedName = "";

	while(value[i] != '\0'){
		copiedName += value[i];
		i++;
	}

	return copiedName;
}

bool check_name(char const *value){
	if (value == NULL){
		// niepoprawna nazwa 
		return false;
	}else{
		return true;
	}
}

void add_relation(id_type id, string name1, string name2){
	graph[id][name1].insert(name2);
	transposed_graph[id][name2].insert(name1);

	return;
}

void delete_relation(id_type id, string name1, string name2){
	auto it1 = graph[id][name1].find(name2);
	graph[id][name1].erase(it1);

	auto it2 = transposed_graph[id][name2].find(name1);
	transposed_graph[id][name2].erase(it2);

	return;
}

bool poset_insert(id_type id, char const *value){
	if (poset_exists[id] == true){
		if (check_name(value) == false){
			return false;
		}

		string copiedName = copyName(value);

		if (element_exists[id][copiedName] == false){
			element_exists[id][copiedName] = true;
			poset_active_elements[id] += 1;

			add_relation(id, copiedName, copiedName);

			// wierzcholek pomyslnie dodany
			return true;
		}else{
			// taki element juz istnieje w danym posecie
			return false;
		}
	}else{
		// poset o takim id nie istnieje
		return false;
	}
}

bool poset_remove(id_type id, char const *value){
	if (poset_exists[id]){	
		if (check_name(value) == false){
			return false;
		}

		string name = copyName(value);

		if (element_exists[id][name] == false){
			// element nie istnieje

			return false;
		}

		for(auto v:graph[id][name]){
			auto it = transposed_graph[id][v].find(name);
			transposed_graph[id][v].erase(it);
		}

		graph[id][name].clear();

		for(auto v:transposed_graph[id][name]){
			auto it = graph[id][v].find(name);
			graph[id][v].erase(it);
		}

		transposed_graph[id][name].clear();

		element_exists[id][name] = false;

		poset_active_elements[id] -= 1;

		return true;
	}else{
		// poset o tym id nie istnieje
		return false;
	}
}

bool poset_add(id_type id, char const *value1, char const *value2){
	if (poset_exists[id]){	
		if (check_name(value1) == false || check_name(value2) == false){
			return false;
		}

		string name1 = copyName(value1);
		string name2 = copyName(value2);

		if (element_exists[id][name1] == false || 
				element_exists[id][name2] == false){
			// ktorys z elementow nie istnieje

			return false;
		}

		if (graph[id][name1].find(name2) != graph[id][name1].end() || 
			graph[id][name2].find(name1) != graph[id][name2].end() ){
			// relacja nie moze byc dodana
			return false;
		}else{
			for(auto v:transposed_graph[id][name1]){
				for(auto u:graph[id][name2]){
					add_relation(id, v, u);
				}
			}

			return true;
		}

	}else{
		// poset o tym id nie istnieje
		return false;
	}
}

map <string, bool> visited;

void dfs(string node, id_type id){
	visited[node] = true;

	for(auto v:graph[id][node]){
		if (visited[v] == false){
			dfs(v, id);
		}
	}

	return;
}

bool poset_del(id_type id, char const *value1, char const *value2){
	if (poset_exists[id]){	
		if (check_name(value1) == false || check_name(value2) == false){
			return false;
		}

		string name1 = copyName(value1);
		string name2 = copyName(value2);

		if (element_exists[id][name1] == false || 
				element_exists[id][name2] == false){
			// ktorys z elementow nie istnieje

			return false;
		}

		if (graph[id][name1].find(name2) != graph[id][name1].end()){
			delete_relation(id, name1, name2);

			visited.clear();

			dfs(name1, id);

			if (visited[name2] == true){
				// usuniecie relacji zaburzy czesciowy porzadek
				add_relation(id, name1, name2);

				return false;
			}else{
				return true;
			}
		}else{
			// relacja nie istnieje
			return false;
		}
	}else{
		// poset o tym id nie istnieje
		return false;
	}
}

bool poset_test(id_type id, char const *value1, char const *value2){
	if (poset_exists[id]){	
		if (check_name(value1) == false || check_name(value2) == false){
			return false;
		}

		string name1 = copyName(value1);
		string name2 = copyName(value2);

		if (element_exists[id][name1] == false || 
				element_exists[id][name2] == false){
			// ktorys z elementow nie istnieje

			return false;
		}

		if (graph[id][name1].find(name2) != graph[id][name1].end()){
			// relacja istnieje
			return true;
		}else{
			// relacja nie istnieje
			return false;
		}
	}else{
		// poset o tym id nie istnieje
		return false;
	}
}

void poset_clear(id_type id){
	if (poset_exists[id]){
		poset_active_elements[id] = 0;
		element_exists[id].clear();
		graph[id].clear();
		transposed_graph[id].clear();

		return;
	}else{
		// poset o takim id nie istnieje
		return;
	}
}

int32_t main(){
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