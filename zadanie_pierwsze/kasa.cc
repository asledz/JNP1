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

/* Funkcje obsługi rozkładu jazdy */

// Skalowanie numerów linii
std::map<int, int> line_id;

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

/* Algorytm plecakowy i funkcje pomocnicze */

void set_array_to(int value, int size, int tab[]) {
  for(int i = 0; i < size; i++) {
    tab[i] = value;
  }
}

void print_all_the_tickets () {
  for (int i = 0; i < tickets.size(); i++) {
    std::cout << "ticket[" << i << "] --> " << name_of_ticket(i) << " kosztuje  " << prize_of_ticket(i) << "  i trwa " << duration_of_ticket(i) << "\n";
  }
}

void one_ticket_iteration (int one_ticket[], int time) {
  for(int i = 0; i < tickets.size(); i++) {
    if(duration_of_ticket(i) < time) {
      if(one_ticket[duration_of_ticket(i)] == -1) {
        one_ticket[duration_of_ticket(i)] = i;
      }
      else {
        if (prize_of_ticket(one_ticket[duration_of_ticket(i)]) >= prize_of_ticket(i)) {
          one_ticket[duration_of_ticket(i)] = i;
        }
      }
    }
    else {
      // TO DO: zaimplementować co jeśli wychodzi poza zakres doby.
    }
  }
}

void multiple_tickets_iteration (int one_ticket[], int two_tickets[], int time) {
  for (int i = 1; i < time; i++) {
    if (one_ticket[i] != -1) {
      for (int j = 0; j < tickets.size(); j++) {
        if (i + duration_of_ticket(j) < time) {
          if (two_tickets[duration_of_ticket(j) + i] == -1) {
            two_tickets[duration_of_ticket(j) + i] = j;
          }
          else {
            if(prize_of_ticket(two_tickets[duration_of_ticket(j) + i]) >= prize_of_ticket(j)) {
              two_tickets[duration_of_ticket(j) + i] = j;
            }
          }
        }
        else {
          // TO DO: zaimplementować, co jeśli wychodzi poza zakres doby.
        }
      }
    }
  }
}

std::pair<std::string, double> one_ticket_value(int one_ticket[], int time, int duration) {
  double best_val = MAX_INT;
  int start_point = 0;
  for (int i = duration; i < time; i++) {
    if(one_ticket[i] > -1) {
      if(best_val > prize_of_ticket(one_ticket[i])) {
        best_val = prize_of_ticket(one_ticket[i]);
        start_point = i; 
      }
    }
  }
  if (best_val == MAX_INT) {
    return std::make_pair(":-|\n", best_val);
  }
  std::string solution = "! " + name_of_ticket(one_ticket[start_point]) + "\n"; 
  return std::make_pair(solution, best_val);
}

std::pair<std::string, double> two_ticket_value (int one_ticket[], int two_tickets[], int time, int duration) {
  double best_val = MAX_INT;
  int start_point = 0;
  for (int i = duration; i < time; i++) {
    if (two_tickets[i] > -1) {
      int id_st = two_tickets[i];
      int id_nd = one_ticket[i - duration_of_ticket(id_st)];
      if (best_val > prize_of_ticket(id_st) + prize_of_ticket(id_nd)) {
        start_point = i;
        best_val = prize_of_ticket(id_st) + prize_of_ticket(id_nd);
      }
    }
  }
  if (best_val == MAX_INT) {
    return std::make_pair(":-|\n", best_val);
  }
  int id_st = two_tickets[start_point];
  int id_nd = one_ticket[start_point - duration_of_ticket(id_st)];
  std::string solution = "! " + name_of_ticket(id_st) + "; " + name_of_ticket(id_nd) + "\n";
  return std::make_pair(solution, best_val);  
}

std::pair<std::string, double> three_ticket_value (int one_ticket[], int two_tickets[], int three_tickets[], int time, int duration) {
  double best_val = MAX_INT;
  int start_point = 0;
  for (int i = duration; i < time; i++) {
    if (three_tickets[i] > -1) {
      int id_st = three_tickets[i];
      int id_nd = two_tickets[i - duration_of_ticket(id_st)];
      int id_rd = one_ticket[i - duration_of_ticket(id_st) - duration_of_ticket(id_nd)];
      if(best_val > prize_of_ticket(id_st) + prize_of_ticket(id_nd) + prize_of_ticket(id_rd)) {
        start_point = i;
        best_val = prize_of_ticket(id_st) + prize_of_ticket(id_nd) + prize_of_ticket(id_rd);
      }
    }
  }
  if(best_val == MAX_INT) {
    return std::make_pair(":-|\n", best_val); 
  }
  int id_st = three_tickets[start_point];
  int id_nd = two_tickets[start_point - duration_of_ticket(id_st)];
  int id_rd = one_ticket[start_point - duration_of_ticket(id_st) - duration_of_ticket(id_nd)];
  std::string solution = "! " + name_of_ticket(id_st) + "; " + name_of_ticket(id_nd) + "; " + name_of_ticket(id_rd) + "\n";
  return std::make_pair(solution, best_val);
}

void calculate_solution (int one_ticket[], int two_tickets[], int three_tickets[], int time, int duration) {
  std::pair<std::string, double> solutions[3];
  solutions[0] = one_ticket_value(one_ticket, time, duration);
  solutions[1] = two_ticket_value(one_ticket, two_tickets, time, duration);
  solutions[2] = three_ticket_value(one_ticket, two_tickets, three_tickets, time, duration);

  if (solutions[0].second > solutions[1].second) {
    std::swap (solutions[0], solutions[1]);
  }
  if(solutions[0].second > solutions[2].second) {
    std::swap(solutions[0], solutions[2]);
  }

  std::cout << solutions[0].first;
}

void knapsack_algorithm (int duration) {

  int time = time_to_minutes(21, 21) - time_to_minutes(5, 55) + 1;
  int size = time+2;
  int one_ticket[size], two_tickets[size], three_tickets[size];
  
  set_array_to(-1, size, one_ticket);
  set_array_to(-1, size, two_tickets);
  set_array_to(-1, size, three_tickets);

  one_ticket_iteration(one_ticket, time);
  multiple_tickets_iteration(one_ticket, two_tickets, time);
  multiple_tickets_iteration(two_tickets, three_tickets, time);

  calculate_solution(one_ticket, two_tickets, three_tickets, time, duration);

}

/* FUNKCJE DO TESTOWANIA */

void testuj_algorytm_plecakowy () {
  add_ticket("Trwanie 2", 2, 1);
  add_ticket("Trwanie 3", 3, 2);
  add_ticket("Tanie trwanie 2", 2, 0.4);
  // print_all_the_tickets();

  knapsack_algorithm(1);
  knapsack_algorithm(2);
  knapsack_algorithm(3);
  knapsack_algorithm(5);
  knapsack_algorithm(10);
}

int main() {

  testuj_algorytm_plecakowy();
  
  return 0;


  std::string slowo;

  std::getline(std::cin, slowo);

  std::regex course_regex("^([0-9]+)((?: (?:(?:2[0-3]|1[0-9]|0?[1-9]):[0-5][0-9]) (?:[a-zA-Z0-9^]+))+)$");
  std::regex ticket_regex("^((?:[a-zA-Z ])+) ((?:[0-9])+\\.[0-9]{2}) ([1-9][0-9]*)$");
  std::regex query_regex("^\\? (?:[a-zA-Z0-9^]+)( (?:[0-9]+) (?:[a-zA-Z0-9^]+))+$");

  std::regex course_number_regex("^([0-9]+)");
  std::regex time_regex("^ ((?:2[0-3]|1[0-9]|0?[1-9]):[0-5][0-9])");
  std::regex stop_name_regex("^ ([a-zA-Z0-9^]+)");
  std::regex ticket_name_regex("^((?:[a-zA-Z ])+)");
  std::regex ticket_prize_regex("^((?:[0-9])+\\.[0-9]{2})");
  std::regex ticket_valid_regex("^ ([1-9][0-9]*)");

  if(std::regex_match(slowo, course_regex)) {
    std::cout << "course_regex\n";
    std::string course_number, time_jprdl, course_name;

    course_number = first_match(slowo, course_number_regex);
    std::cout << course_number << "\n";

    while(!slowo.empty()) {
      time_jprdl = first_match(slowo, time_regex);
      std::cout << time_jprdl << "\n";

      course_name = first_match(slowo, stop_name_regex);
      std::cout << course_name << "\n";
    }
  }
  else if(std::regex_match(slowo, ticket_regex)) {
    std::cout << "ticket_regex\n";

    std::string ticket_name, ticket_prize, ticket_valid;

    ticket_name = first_match(slowo, ticket_name_regex);
    std::cout << ticket_name << "\n";

    ticket_prize = first_match(slowo, ticket_prize_regex);
    std::cout << ticket_prize << "\n";

    ticket_valid = first_match(slowo, ticket_valid_regex);
    std::cout << ticket_valid << "\n";
  }
  else if(std::regex_match(slowo, query_regex)) {
    std::cout << "query_regex\n";
		slowo.erase(0, 1);

		std::string start, stop, course_number;

		start = first_match(slowo, stop_name_regex);
		std::cout << start << "\n";

		while(!slowo.empty()) {
			slowo.erase(0, 1);
			course_number = first_match(slowo, course_number_regex);
			std::cout << course_number << "\n";

			stop = first_match(slowo, stop_name_regex);
			std::cout << stop << "\n";
		}
  }

	return 0;
}
