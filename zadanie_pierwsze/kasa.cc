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

void callError(int line_number, std::string &line) {
	std::cerr << "Error in line " << line_number << ": " << line << "\n";
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

bool check_if_exists(std::string name) {
	for(int i=0; i < tickets.size(); i++) {
		if(name_of_ticket(i) == name) return true;
	}
	return false;
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

void execute_course_adding(std::string &line, int line_number) {
	std::string line_backup = line;

	std::regex course_number_regex("^([0-9]+)");
	std::regex time_regex("((?:2[0-3]|1[0-9]|[1-9]):[0-5][0-9])");
	std::regex stop_name_regex("([a-zA-Z0-9^]+)");

	std::string course_number, time_jprdl, stop_name;

	course_number = first_match(line, course_number_regex);
	unsigned course = string_to_int(course_number);
	if(line_id.find(course) != line_id.end()) {
		callError(line_number, line_backup);
		return;
	}
	line_id[course] = number_of_courses;

	std::map<std::string, int> course_timetable;
	std::set<std::string> visited_stops;
	int time_limit_bottom = string_to_time("5:55") - 1;
	int time_limit_up = string_to_time("21:21");

	while(!line.empty()) {
		time_jprdl = first_match(line, time_regex);
		int minutes = string_to_time(time_jprdl);
		if(time_limit_bottom >= minutes || time_limit_up < minutes) {
			line_id.erase(course);
			callError(line_number, line_backup);
			return;
		}
		time_limit_bottom = minutes;

		stop_name = first_match(line, stop_name_regex);
		if(visited_stops.find(stop_name) != visited_stops.end()) {
			line_id.erase(course);
			callError(line_number, line_backup);
			return;
		}

		visited_stops.insert(stop_name);
		course_timetable[stop_name] = minutes;
	}
	number_of_courses++;
	timetable.push_back(course_timetable);
}

void execute_ticket_adding(std::string &line, int line_number) {
	std::string line_backup = line;

	std::regex ticket_name_regex("((?:[a-zA-Z ])+)");
  std::regex ticket_prize_regex("((?:[0-9])+\\.[0-9]{2})");
  std::regex ticket_valid_regex("([1-9][0-9]*)");

	std::string ticket_name, ticket_prize, ticket_valid;
	ticket_name = first_match(line, ticket_name_regex);
	ticket_name.erase(ticket_name.size()-1, ticket_name.size());
	ticket_prize = first_match(line, ticket_prize_regex);
	double price_int = string_to_double(ticket_prize);
	ticket_valid = first_match(line, ticket_valid_regex);
	int valid_time = string_to_int(ticket_valid);

	if(check_if_exists(ticket_name)) {
		callError(line_number, line_backup);
		return;
	}

  add_ticket(ticket_name, valid_time, price_int);
}

void execute_course_query(std::string &line, int line_number) {
	std::string line_backup = line;

	std::regex stop_name_regex("([a-zA-Z0-9^]+)");
	std::regex course_number_regex("([0-9]+)");

	std::string start, stop, course_number;

	start = first_match(line, stop_name_regex);

	course_number = first_match(line, course_number_regex);
	stop = first_match(line, stop_name_regex);
	int course_number_int = string_to_int(course_number);
	int start_time, last_time;

	if (timetable[line_id[course_number_int]].find(start) != timetable[line_id[course_number_int]].end()) {
		start_time = timetable[line_id[course_number_int]][start];
		if (timetable[line_id[course_number_int]].find(stop) != timetable[line_id[course_number_int]].end()) {
			last_time = timetable[line_id[course_number_int]][stop];
		}
		else {
			callError(line_number, line_backup);
			return;
		}
	}
	else {
		callError(line_number, line_backup);
		return;
	}

	start = stop;
	int last_course_number = course_number_int;

	while(!line.empty()) {
		course_number = first_match(line, course_number_regex);
		course_number_int = string_to_int(course_number);

		stop = first_match(line, stop_name_regex);

		if(timetable[line_id[course_number_int]].find(start) != timetable[line_id[course_number_int]].end() &&
		timetable[line_id[course_number_int]].find(stop) != timetable[line_id[course_number_int]].end()) {
			if(last_time == timetable[line_id[course_number_int]][start]) {
				last_time = timetable[line_id[course_number_int]][stop];
				start = stop;
				last_course_number = course_number_int;
			}
			else {
				if(last_time > timetable[line_id[course_number_int]][start]) {
					callError(line_number, line_backup);
					return;
				}
				std::cout << ":( " << start << "\n";
				return;
			}
		}
		else {
			callError(line_number,line_backup);
			return;
		}
	}
	find_best_combination(last_time - start_time + 1);
}

void execute_line(int line_number, std::string line) {
  std::regex course_regex("^([0-9]+)((?: (?:(?:2[0-3]|1[0-9]|[1-9]):[0-5][0-9]) (?:[a-zA-Z0-9^]+)){2,})$");
  std::regex ticket_regex("^((?:[a-zA-Z ])+) ((?:[0-9])+\\.[0-9]{2}) ([1-9][0-9]*)$");
  std::regex query_regex("^\\? (?:[a-zA-Z0-9^]+)( (?:[0-9]+) (?:[a-zA-Z0-9^]+))+$");

	if(line.empty()) {
		return;
	}
  else if(std::regex_match(line, course_regex)) {
		execute_course_adding(line, line_number);
  }
  else if(std::regex_match(line, ticket_regex)) {
    execute_ticket_adding(line, line_number);
  }
  else if(std::regex_match(line, query_regex)) {
    execute_course_query(line, line_number);
  }
	else {
		callError(line_number, line);
		return;
	}

}

int main() {
  std::string line;
	int line_number = 1;

	while(std::getline(std::cin, line)) {
		execute_line(line_number++, line);
	}

	return 0;
}
