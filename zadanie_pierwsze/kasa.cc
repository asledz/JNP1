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

void execute_line(int line_number, std::string line) {
  std::regex course_regex("^([0-9]+)((?: (?:(?:2[0-3]|1[0-9]|0?[1-9]):[0-5][0-9]) (?:[a-zA-Z0-9^]+))+)$");
  std::regex ticket_regex("^((?:[a-zA-Z ])+) ((?:[0-9])+\\.[0-9]{2}) ([1-9][0-9]*)$");
  std::regex query_regex("^\\? (?:[a-zA-Z0-9^]+)( (?:[0-9]+) (?:[a-zA-Z0-9^]+))+$");

  std::regex course_number_regex("^([0-9]+)");
  std::regex time_regex("((?:2[0-3]|1[0-9]|0?[1-9]):[0-5][0-9])");
  std::regex stop_name_regex("([a-zA-Z0-9^]+)");
  std::regex ticket_name_regex("((?:[a-zA-Z ])+)");
  std::regex ticket_prize_regex("((?:[0-9])+\\.[0-9]{2})");
  std::regex ticket_valid_regex("([1-9][0-9]*)");

	if(std::regex_match(line, course_regex)) {
    std::cout << "course_regex\n";
    std::string course_number, time_jprdl, course_name;

    course_number = first_match(line, course_number_regex);
    std::cout << course_number << "\n";

    while(!line.empty()) {
      time_jprdl = first_match(line, time_regex);
      std::cout << time_jprdl << "\n";

      course_name = first_match(line, stop_name_regex);
      std::cout << course_name << "\n";
    }
  }
  else if(std::regex_match(line, ticket_regex)) {
    std::cout << "ticket_regex\n";

    std::string ticket_name, ticket_prize, ticket_valid;

    ticket_name = first_match(line, ticket_name_regex);
    std::cout << ticket_name << "\n";

    ticket_prize = first_match(line, ticket_prize_regex);
    std::cout << ticket_prize << "\n";

    ticket_valid = first_match(line, ticket_valid_regex);
    std::cout << ticket_valid << "\n";
  }
  else if(std::regex_match(line, query_regex)) {
    std::cout << "query_regex\n";

		std::string start, stop, course_number;

		start = first_match(line, stop_name_regex);
		std::cout << start << "\n";

		while(!line.empty()) {
			line.erase(0, 1);
			course_number = first_match(line, course_number_regex);
			std::cout << course_number << "\n";

			stop = first_match(line, stop_name_regex);
			std::cout << stop << "\n";
		}
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
