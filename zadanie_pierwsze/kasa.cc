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

/* Funkcje użytkowe */

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

/* Algorytm plecakowy */

void knapsack_algorithm (int duration) {
  int time = time_to_minutes(21, 21) - time_to_minutes(5, 55) + 1;
  int one_ticket[time], two_tickets[time], three_tickets[time];
  // Ustawienie tablic na -1
  for(int i = 0; i < time; i++) {
    one_ticket[0] = -1;
    two_tickets[0] = -1;
    three_tickets[0] = -1;
  }
  // iteracja po jednym bilecie 
  for(int i = 0; i < tickets.size(); i++) {
    if (duration_of_ticket(i) < time - 1) {
      if (one_ticket[duration_of_ticket(i)] == -1) {
        one_ticket[duration_of_ticket(i)] = i;
      }
      else {
        if (prize_of_ticket(i) <= prize_of_ticket(one_ticket[duration_of_ticket(i)])) {
          one_ticket[duration_of_ticket(i)] = i;
        }
      }
    }
  }
  // iteracja po drugim bilecie
  for(int i = 0; i < tickets.size(); i++) {
    for(int j = 1; j < time; j++) {
      if(duration_of_ticket(i) + j < time - 1 && one_ticket[j] != -1) {
        if(two_tickets[duration_of_ticket(i) + j] == -1) {
          two_tickets[duration_of_ticket(i) + j] = i;
        }
        else {
          if(prize_of_ticket(i) <= prize_of_ticket(two_tickets[duration_of_ticket(i) + j])) {
            two_tickets[duration_of_ticket(i) + j] = i;
          }
        }
      }
    }
  }
  // iteracja po trzecim bilecie
  for(int i = 0; i < tickets.size(); i++) {
    for(int j = 1; j < time; j++) {
      if(duration_of_ticket(i) + j < time - 1 && two_tickets[j] != -1 ) {
        if(three_tickets[duration_of_ticket(i) + j] == -1) {
          three_tickets[duration_of_ticket(i) + j] = i;
        }
        else {
          if(prize_of_ticket(i) <= prize_of_ticket(three_tickets[duration_of_ticket(i) + j])) {
            three_tickets[duration_of_ticket(i) + j] = i;
          }
        }
      }
    }
  }
  // znalezienie najtanszego rozwiazania 
  int one_ticket_solution = MAX_INT, one_ticket_start;
  int two_tickets_solution = MAX_INT, two_tickets_start;
  int three_tickets_solution = MAX_INT, three_tickets_start;
  for (int i = duration; i < time; i++) {
    if (one_ticket_solution > prize_of_ticket(one_ticket[i])) {
      one_ticket_start = i;
      one_ticket_solution = prize_of_ticket(one_ticket[i]);
    }
  } 

  for (int i = duration; i < time; i++) {
    if (two_tickets_solution > prize_of_ticket(two_tickets[i]) 
                  + prize_of_ticket(one_ticket[i - duration_of_ticket(two_tickets[i])])) {
      two_tickets_start = i;
      two_tickets_solution = prize_of_ticket(two_tickets[i]) 
                  + prize_of_ticket(one_ticket[i - duration_of_ticket(two_tickets[i])]);
    }
  }

  for (int i = duration; i < time; i++) {
    if(three_tickets_solution > prize_of_ticket(three_tickets[i]) 
                  + prize_of_ticket(two_tickets[i - duration_of_ticket(three_tickets[i])])
                  + prize_of_ticket(one_ticket[i - duration_of_ticket(three_tickets[i] 
                                   - duration_of_ticket(two_tickets[i - duration_of_ticket(three_tickets[i])] ))] ) )  {
      three_tickets_start = i;
      three_tickets_solution = prize_of_ticket(three_tickets[i]) 
                  + prize_of_ticket(two_tickets[i - duration_of_ticket(three_tickets[i])])
                  + prize_of_ticket(one_ticket[i - duration_of_ticket(three_tickets[i] 
                                   - duration_of_ticket(two_tickets[i - duration_of_ticket(three_tickets[i])] ))] ); 
    }
  }

  cout << one_ticket_solution << " " << two_tickets_solution << " " << three_tickets_solution << "\n";

}


int main() {
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
