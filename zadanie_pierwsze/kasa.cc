/*
  ZADANIE KASA BILETOWA
  JNPI 2019/2020
  Autorzy:
  Marcin Kurowski mk406210
  Anita Śledź as406384
*/

#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <regex>

/* Funkcje użytkowe i stałe */

const int MAX_INT = 2147483647;

int char_to_int(char num) {
	return num - '0';
}

int time_to_minutes (int hour, int minutes) {
	return 60 * hour + minutes;
}

// Funkcja konwertująca string z opisem godziny do minut.
// W przypadku niepoprawnej godziny, zwraca -1.
int string_to_time (std::string time) {
	if(time.size() < 4 || time.size() > 5) {
		return -1;
	}
	int minutes = char_to_int(time[time.size() - 1]) + 10 * char_to_int(time[time.size() - 2]);
	int hours = char_to_int(time[0]);
	if(time[1] != ':') {
		hours = hours * 10 + char_to_int(time[1]);
	}
	return time_to_minutes(hours, minutes);
}

void set_array_to(int value, int size, int tab[]) {
  for(int i = 0; i < size; i++) {
    tab[i] = value;
  }
}

// Funkcja konwertująca string do liczby zmiennoprzecinkowej
// o rozwinięciu dziesiętnym długosci 2
double string_to_double (std::string &text) {
	double power = 0.01;
	double result = 0;
	for (int i=text.size() - 1; i >= 0; i--) {
		if (text[i] == '.') continue;
		result += power * char_to_int(text[i]);
		power *= 10;
	}
	return result;
}

// Funkcja konwertująca string do liczby całkowitej
int string_to_int (std::string &text) {
	int power = 1;
	int result = 0;
	for (int i=text.size() - 1; i >= 0; i--) {
		result += power * char_to_int(text[i]);
		power *= 10;
	}
	return result;
}

/* Funkcje obsługi rozkładu jazdy */

// Skalowanie numerów linii
std::map<int, int> line_id;

// Liczba kursów tramwajowych
unsigned number_of_courses;

// Przechowywanie linii tramwajowych
// Dla każdej linii(kursu) trzymamy godzinę przyjazdu na przystanek
std::vector<std::map<std::string, int>> timetable;

// Opis biletu - dla każdej nazwy trzymamy długość trwania, cenę
std::vector<std::pair<std::string, std::pair<double, int>>> tickets;

// Sprawdza, czy dany numer linii kursu tramwajowego zatrzymuje się o danej godzinie na przystanku.
bool stop_exists (int number_of_the_line, std::string name_of_the_stop, int time) {
	// nie istnieje taka linia tramwajowa
	if(line_id.find(number_of_the_line) == line_id.end() ) {
		return false;
	}
	int id = line_id[number_of_the_line];
	if(timetable[id][name_of_the_stop] == time) {
		return true;
	}
	else {
		return false;
	}
}

/* Funkcje obługi biletów */

std::string name_of_ticket(int id) {
	return tickets[id].first;
}

void add_ticket(std::string name, int duration, double prize) {
  tickets.push_back((std::make_pair(name, std::make_pair(prize, duration))));
}

double prize_of_ticket (int id) {
	return tickets[id].second.first;
}

int duration_of_ticket (int id) {
	return tickets[id].second.second;
}

std::string first_match(std::string &text, std::regex &r) {
  std::smatch match;
  std::regex_search(text, match, r);
  std::string res = match.str(0);
  text = match.suffix().str();
  return res;
}

/* Rozwiązanie w n^3 */

void create_output (const int id_st, const int id_nd, const int id_rd) {
  if(id_st == -1) {
    std::cout << ":-|\n";
  }
  else if(id_nd == -1) {
    std::cout << "! " << name_of_ticket(id_st) << "\n";
  }
  else if(id_rd == -1) {
    std::cout << "! " << name_of_ticket(id_st) << "; " << name_of_ticket(id_nd) << "\n";
  }
  else {
    std::cout << "! " << name_of_ticket(id_st) << "; " << name_of_ticket(id_nd) << "; " << name_of_ticket(id_rd) << "\n";
  }
}

double calculate_cost (const int id_st, const int id_nd, const int id_rd) {
  double prize = MAX_INT;
  if (id_st != -1) {
    prize = prize_of_ticket(id_st);
  }
  if (id_nd != -1) {
    prize += prize_of_ticket(id_nd);
  }
  if(id_rd != -1) {
    prize += prize_of_ticket(id_rd);
  }
  return prize;
}

int calculate_time(const int id_st, const int id_nd, const int id_rd) {
  int time = 0;
  if (id_st != -1) {
    time += duration_of_ticket(id_st);
  }
  if (id_nd != -1) {
    time += duration_of_ticket(id_nd);
  }
  if(id_rd != -1) {
    time += duration_of_ticket(id_rd);
  }
  return time;
}

void change(double &value, const int time, int &st, int &nd, int &rd, int ticket_st, int ticket_nd, int ticket_rd) {
  if (calculate_cost(ticket_st, ticket_nd, ticket_rd) <= value) {
    if(calculate_time(ticket_st, ticket_nd, ticket_rd) >= time) {
      st = ticket_st;
      nd = ticket_nd;
      rd = ticket_rd;
      value = calculate_cost(st, nd, rd);
    }
  }
}

void create_solution(const int duration) {
  int none = -1;
  double best_val = MAX_INT;
  int st = none, nd = none, rd = none;
  int id_st = none, id_nd = none, id_rd = none;
  for(int i = 0; i < tickets.size(); i++) {
    for(int j = 0; j < tickets.size(); j++) {
      for(int k = 0; k < tickets.size(); k++) {
        id_st = i, id_nd = j, id_rd = k;
        change(best_val, duration, st, nd, rd, i, none, none);
        change(best_val, duration, st, nd, rd, i, j, none);
        change(best_val, duration, st, nd, rd, i, j, k);
      }
    }
  }
  create_output(st, nd, rd);
}

void find_best_combination (const int duration) {
  create_solution(duration);
}

/* FUNKCJE DO TESTOWANIA */

void testuj_algorytm_plecakowy () {
  add_ticket("Trwanie 2", 2, 1);
  add_ticket("Trwanie 3", 3, 2);
  add_ticket("Tanie trwanie 2", 2, 0.4);

  find_best_combination(2);
  find_best_combination(3);
  find_best_combination(5);
  find_best_combination(10);
}

void execute_course_adding(std::string line, int line_number) {
	std::regex course_number_regex("^([0-9]+)");
	std::regex time_regex("((?:2[0-3]|1[0-9]|[1-9]):[0-5][0-9])");
	std::regex stop_name_regex("([a-zA-Z0-9^]+)");

	std::cout << "course_regex\n";
	std::string course_number, time_jprdl, course_name;

	course_number = first_match(line, course_number_regex);
	std::cout << course_number << "\n";

	unsigned course = string_to_int(course_number);
	line_id[course] = number_of_courses;

	map<std::string, int> course_timetable;
	int time_check = -1;
	while(!line.empty()) {
		time_jprdl = first_match(line, time_regex);
		minutes = time_to_minutes(time_jprdl);
		if(time_check >= minutes) {
			number_of_courses--;
			line_id.erase(number_of_courses);

		}
		std::cout << time_jprdl << "\n";

		course_name = first_match(line, stop_name_regex);
		std::cout << course_name << "\n";
	}
}

void execute_ticket_adding(std::string line, int line_number) {
	std::regex ticket_name_regex("((?:[a-zA-Z ])+)");
  std::regex ticket_prize_regex("((?:[0-9])+\\.[0-9]{2})");
  std::regex ticket_valid_regex("([1-9][0-9]*)");

	std::cout << "ticket_regex\n";

	std::string ticket_name, ticket_prize, ticket_valid;

	ticket_name = first_match(line, ticket_name_regex);
	std::cout << ticket_name << "\n";

	ticket_prize = first_match(line, ticket_prize_regex);
	std::cout << ticket_prize << "\n";

	ticket_valid = first_match(line, ticket_valid_regex);
	std::cout << ticket_valid << "\n";
}

void execute_course_query(std::string line, int line_number) {
	std::regex stop_name_regex("([a-zA-Z0-9^]+)");
	std::regex course_number_regex("([0-9]+)");

	std::cout << "query_regex\n";

	std::string start, stop, course_number;

	start = first_match(line, stop_name_regex);
	std::cout << start << "\n";

	while(!line.empty()) {
		course_number = first_match(line, course_number_regex);
		std::cout << course_number << "\n";

		stop = first_match(line, stop_name_regex);
		std::cout << stop << "\n";
	}
}

void execute_line(int line_number, std::string line) {
  std::regex course_regex("^([0-9]+)((?: (?:(?:2[0-3]|1[0-9]|[1-9]):[0-5][0-9]) (?:[a-zA-Z0-9^]+))+)$");
  std::regex ticket_regex("^((?:[a-zA-Z ])+) ((?:[0-9])+\\.[0-9]{2}) ([1-9][0-9]*)$");
  std::regex query_regex("^\\? (?:[a-zA-Z0-9^]+)( (?:[0-9]+) (?:[a-zA-Z0-9^]+))+$");

  if(std::regex_match(line, course_regex)) {
		execute_course_adding(line, line_number);
  }
  else if(std::regex_match(line, ticket_regex)) {
    execute_ticket_adding(line, line_number);
  }
  else if(std::regex_match(line, query_regex)) {
    execute_course_query(line, line_number);
  }
}

int main() {
  testuj_algorytm_plecakowy();
  std::string line;
	int line_number = 1;

	while(std::getline(std::cin, line)) {
		execute_line(line_number++, line);
	}

	return 0;
}
