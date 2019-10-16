/*
  ZADANIE KASA BILETOWA
  JNPI 2019/2020
  Autorzy:
  Marcin Kurowski mk406210
  Anita Śledź as406384
*/

#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <regex>

/* Funkcje użytkowe i stałe */

const int MAX_INT = 2147483647;

uint char_to_int(const char num) {
    return num - '0';
}

uint time_to_minutes(const int hour, const int minutes) {
    return 60 * hour + minutes;
}

// Funkcja konwertująca string z opisem godziny do minut.
// W przypadku niepoprawnej godziny, zwraca -1. Godzina jest podana w formacie hh:mm albo h:mm.
int string_to_time(const std::string time) {
    if (time.size() < 4 || time.size() > 5) {
        return -1;
    }
    uint minutes = char_to_int(time[time.size() - 1]) + 10 * char_to_int(time[time.size() - 2]);
    uint hours = char_to_int(time[0]);
    if (time[1] != ':') {
        hours = hours * 10 + char_to_int(time[1]);
    }
    return time_to_minutes(hours, minutes);
}

void set_array_to(const int value, const uint size, int tab[]) {
    for (uint i = 0; i < size; i++) {
        tab[i] = value;
    }
}

// Funkcja konwertująca string do liczby całkowitej
int string_to_int(const std::string &text) {
    int power = 1;
    int result = 0;
    for (int i = text.size() - 1; i >= 0; i--) {
        if (text[i] == '.') continue;
        result += power * char_to_int(text[i]);
        power *= 10;
    }
    return result;
}

// Wypisuje na standardowe wyjście diagnostyczne informację o błędzie
// w podanej linii oraz podaną linię.
void call_error(const int line_number, std::string &line) {
    std::cerr << "Error in line " << line_number << ": " << line << "\n";
}

/* Funkcje obsługi rozkładu jazdy */

// Skalowanie numerów linii
std::unordered_map<int, int> line_id;

// Liczba kursów tramwajowych
unsigned number_of_courses;

// Przechowywanie linii tramwajowych
// Dla każdej linii(kursu) trzymamy godzinę przyjazdu na przystanek
std::vector<std::unordered_map<std::string, int>> timetable;

// Opis biletu - dla każdej nazwy trzymamy cenę i czas ważności
std::vector<std::pair<std::string, std::pair<unsigned, int>>> tickets;

long long sold_tickets;

// Sprawdza, czy dany numer linii kursu tramwajowego zatrzymuje się o danej godzinie na przystanku.
bool stop_exists(int number_of_the_line, std::string name_of_the_stop, int time) {
    // nie istnieje taka linia tramwajowa
    if (line_id.find(number_of_the_line) == line_id.end()) {
        return false;
    }
    int id = line_id[number_of_the_line];
    if (timetable[id][name_of_the_stop] == time) {
        return true;
    } else {
        return false;
    }
}

// Funkcja sprawdzająca czy podany czas dojazdu na przystanek mieści
// się w przedziale (bottom_limit, koniec pracy tramwajów]
bool valid_minutes(unsigned bottom_limit, unsigned minutes) {
  unsigned time_limit_up = string_to_time("21:21");
  return bottom_limit < minutes && minutes <= time_limit_up;
}

/* Funkcje obługi biletów */

void add_ticket(std::string name, const int duration, unsigned price) {
    tickets.push_back((std::make_pair(name, std::make_pair(price, duration))));
}

unsigned price_of_ticket(const int id) {
    return tickets[id].second.first;
}

int duration_of_ticket(const int id) {
    return tickets[id].second.second;
}

std::string name_of_ticket(const int id) {
    return tickets[id].first;
}

bool check_if_exists(const std::string name) {
    for (uint i = 0; i < tickets.size(); i++) {
        if (name_of_ticket(i) == name) return true;
    }
    return false;
}

std::string first_match(std::string &text, std::regex &r) {
    std::smatch match;
    std::regex_search(text, match, r);
    std::string res = match.str(0);
    text = match.suffix().str();
    return res;
}

/* Rozwiązanie w n^3 */

void create_output(const int id_st, const int id_nd, const int id_rd) {
    if (id_st == -1) {
        std::cout << ":-|\n";
    } else if (id_nd == -1) {
        std::cout << "! " << name_of_ticket(id_st) << "\n";
        sold_tickets += 1;
    } else if (id_rd == -1) {
        std::cout << "! " << name_of_ticket(id_st) << "; " << name_of_ticket(id_nd) << "\n";
        sold_tickets += 2;
    } else {
        std::cout << "! " << name_of_ticket(id_st) << "; " << name_of_ticket(id_nd) << "; " << name_of_ticket(id_rd)
                  << "\n";
        sold_tickets += 3;
    }
}

unsigned calculate_cost(const int id_st, const int id_nd, const int id_rd) {
    unsigned price = MAX_INT;
    if (id_st != -1) {
        price = price_of_ticket(id_st);
    }
    if (id_nd != -1) {
        price += price_of_ticket(id_nd);
    }
    if (id_rd != -1) {
        price += price_of_ticket(id_rd);
    }
    return price;
}

int calculate_time(const int id_st, const int id_nd, const int id_rd) {
    int time = 0;
    if (id_st != -1) {
        time += duration_of_ticket(id_st);
    }
    if (id_nd != -1) {
        time += duration_of_ticket(id_nd);
    }
    if (id_rd != -1) {
        time += duration_of_ticket(id_rd);
    }
    return time;
}

void change(unsigned &value, const int time, int &st, int &nd, int &rd, int ticket_st, int ticket_nd, int ticket_rd) {
    if (calculate_cost(ticket_st, ticket_nd, ticket_rd) <= value) {
        if (calculate_time(ticket_st, ticket_nd, ticket_rd) >= time) {
            st = ticket_st;
            nd = ticket_nd;
            rd = ticket_rd;
            value = calculate_cost(st, nd, rd);
        }
    }
}

void create_solution(const int duration) {
    int none = -1;
    unsigned best_val = MAX_INT;
    int st = none, nd = none, rd = none;
    for (uint i = 0; i < tickets.size(); i++) {
        for (uint j = 0; j < tickets.size(); j++) {
            for (uint k = 0; k < tickets.size(); k++) {
                change(best_val, duration, st, nd, rd, i, none, none);
                change(best_val, duration, st, nd, rd, i, j, none);
                change(best_val, duration, st, nd, rd, i, j, k);
            }
        }
    }
    create_output(st, nd, rd);
}

void find_best_combination(const int duration) {
    create_solution(duration);
}

void execute_course_adding(std::string &line, int line_number) {
    std::regex course_number_regex("^([0-9]+)");
    std::regex time_regex("((?:2[0-3]|1[0-9]|[1-9]):[0-5][0-9])");
    std::regex stop_name_regex("([a-zA-Z0-9^]+)");
    std::string course_number, time_arriving, stop_name;
    std::string line_backup = line;

    course_number = first_match(line, course_number_regex);
    unsigned course = string_to_int(course_number);

    if (line_id.count(course)) {
        call_error(line_number, line_backup);
        return;
    }

    line_id[course] = number_of_courses;

    std::unordered_map<std::string, int> course_timetable;
    std::unordered_set<std::string> visited_stops;
    unsigned time_limit_bottom = string_to_time("5:55") - 1;
    unsigned minutes;

    while (!line.empty()) {
        time_arriving = first_match(line, time_regex);
        minutes = string_to_time(time_arriving);

        if (!valid_minutes(time_limit_bottom, minutes)) {
            line_id.erase(course);
            call_error(line_number, line_backup);
            return;
        }

        time_limit_bottom = minutes;

        stop_name = first_match(line, stop_name_regex);
        if (visited_stops.count(stop_name)) {
            line_id.erase(course);
            call_error(line_number, line_backup);
            return;
        }

        visited_stops.insert(stop_name);
        course_timetable[stop_name] = minutes;
    }
    number_of_courses++;
    timetable.push_back(course_timetable);
}

void execute_ticket_adding(std::string &line, int line_number) {
    std::regex ticket_name_regex("((?:[a-zA-Z ])+)");
    std::regex ticket_price_regex("((?:[0-9])+\\.[0-9]{2})");
    std::regex ticket_valid_regex("([1-9][0-9]*)");
    std::string ticket_name, ticket_price, ticket_valid;
    std::string line_backup = line;

    ticket_name = first_match(line, ticket_name_regex);
    ticket_name.erase(ticket_name.size() - 1, ticket_name.size());
    ticket_price = first_match(line, ticket_price_regex);
    ticket_valid = first_match(line, ticket_valid_regex);

    int price_int = string_to_int(ticket_price);
    int valid_time = string_to_int(ticket_valid);

    if (check_if_exists(ticket_name)) {
        call_error(line_number, line_backup);
        return;
    }

    add_ticket(ticket_name, valid_time, price_int);
}

void execute_course_query(std::string &line, int line_number) {
    std::regex stop_name_regex("([a-zA-Z0-9^]+)");
    std::regex course_number_regex("([0-9]+)");
    std::string start, stop, course_number;
    std::string line_backup = line;

    start = first_match(line, stop_name_regex);

    course_number = first_match(line, course_number_regex);
    stop = first_match(line, stop_name_regex);
    unsigned course_number_int = string_to_int(course_number);
    unsigned start_time, last_time;

    if (timetable[line_id[course_number_int]].count(start)) {
        start_time = timetable[line_id[course_number_int]][start];
        if (timetable[line_id[course_number_int]].count(stop)) {
            last_time = timetable[line_id[course_number_int]][stop];
        } else {
            call_error(line_number, line_backup);
            return;
        }
    } else {
        call_error(line_number, line_backup);
        return;
    }

    start = stop;

    while (!line.empty()) {
        course_number = first_match(line, course_number_regex);
        course_number_int = string_to_int(course_number);

        stop = first_match(line, stop_name_regex);

        if (timetable[line_id[course_number_int]].count(start) &&
            timetable[line_id[course_number_int]].count(stop)) {
            unsigned on_start = timetable[line_id[course_number_int]][start];
            if (last_time == on_start) {
                last_time = timetable[line_id[course_number_int]][stop];
                start = stop;
            } else {
                if (last_time > on_start) {
                    call_error(line_number, line_backup);
                    return;
                }
                std::cout << ":( " << start << "\n";
                return;
            }
        } else {
            call_error(line_number, line_backup);
            return;
        }
    }
    find_best_combination(last_time - start_time + 1);
}

void execute_line(int line_number, std::string &line) {
    std::regex course_regex("^([0-9]+)((?: (?:(?:2[0-3]|1[0-9]|[1-9]):[0-5][0-9]) (?:[a-zA-Z0-9^]+)){2,})$");
    std::regex ticket_regex("^((?:[a-zA-Z ])+) ((?:[0-9])+\\.[0-9]{2}) ([1-9][0-9]*)$");
    std::regex query_regex("^\\? (?:[a-zA-Z0-9^]+)( (?:[0-9]+) (?:[a-zA-Z0-9^]+))+$");

    if (line.empty()) {
        return;
    } else if (std::regex_match(line, course_regex)) {
        execute_course_adding(line, line_number);
    } else if (std::regex_match(line, ticket_regex)) {
        execute_ticket_adding(line, line_number);
    } else if (std::regex_match(line, query_regex)) {
        execute_course_query(line, line_number);
    } else {
        call_error(line_number, line);
        return;
    }

}

int main() {
    std::string line;
    int line_number = 1;

    while (std::getline(std::cin, line)) {
        execute_line(line_number++, line);
    }

    std::cout << sold_tickets << "\n";

    return 0;
}
