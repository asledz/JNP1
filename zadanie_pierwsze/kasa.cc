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
// Godzina jest podana w formacie hh:mm albo h:mm.
int string_to_time(const std::string &time) {
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
// Ignoruje napotkane kropki - liczby zmiennoprzecinkowe traktuje jak całkowite
// Argument text musi być poprawną liczbą całkowitą lub zmiennoprzecinkową
// zapisaną jako tekst
unsigned long long string_to_ull(const std::string &text) {
    unsigned long long power = 1;
    unsigned long long result = 0;
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
std::vector<std::unordered_map<std::string, unsigned>> timetable;

// Opis biletu - dla każdej nazwy trzymamy cenę i czas ważności
std::vector<std::pair<std::string, std::pair<unsigned, int>>> tickets;

long long sold_tickets;

// Sprawdza, czy dany numer linii kursu tramwajowego zatrzymuje się o danej godzinie na przystanku.
bool stop_exists(int number_of_the_line, std::string name_of_the_stop, unsigned time) {
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

// Funkcja dla podanego tekstu i wzorca znajduje pierwsze wystąpienie
// wzorca w tekscie, zwraca je oraz modfikuje tekst w taki sposób, że
// po jej wywołaniu jest on swoim sufiksem zaczynającym się za wystąpieniem wzorca
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

// Rozpatruje linię z komendą dodania kursu, w razie błądu w komendzie
// wywołuje funkcję call_error, jeśli komenda jest poprawna dodaje rozkład
// kursu do wektora i mapuje jego numer.
void execute_course_adding(std::string &line, int line_number) {
    std::regex course_number_regex("^([0-9]+)");
    std::regex time_regex("((?:2[0-3]|1[0-9]|[1-9]):[0-5][0-9])");
    std::regex stop_name_regex("([a-zA-Z_^]+)");
    std::string course_number, time_arriving, stop_name;
    std::string line_backup = line;

    course_number = first_match(line, course_number_regex);
    unsigned course = string_to_ull(course_number);

    if (line_id.count(course)) {
        call_error(line_number, line_backup);
        return;
    }

    // Mapowanie numeru kursu
    line_id[course] = number_of_courses;

    std::unordered_map<std::string, unsigned> course_timetable;
    // Set do wychwycenia powtórzeń w trasie kursu
    std::unordered_set<std::string> visited_stops;
    unsigned time_limit_bottom = string_to_time("5:55") - 1;
    unsigned minutes;

    // Kolejne czasy oraz przystanki są ucinane z początku komendy aż do jej
    // zakończenia. W przypadku powtórzenia nazwy przystanku lub wczytania
    // niepoprawnego czasu (przed poprzednim przystankiem/rozpoczęciem pracy
    // lub po zakończeniu pracy), wywołuję call_error i cofam mapowanie
    // numeru kursu.
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
    // Jeśli nie przerwano działania funkcji, to znaczy, że zapytanie jest
    // poprawne, dodaję rozkład do wektora kursów i zwiększam licznik
    // przechowywanych kursów.
    number_of_courses++;
    timetable.push_back(course_timetable);
}

// Rozpatruje linię z komendą dodania biletu, wywołuje funkcję
// call_error jeśli któryś z paremetrów jest niepoprawny, jeśli są poprawne,
// dodaje bilet do wektora biletów
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

    int price_int = string_to_ull(ticket_price);
    int valid_time = string_to_ull(ticket_valid);

    if (check_if_exists(ticket_name)) {
        call_error(line_number, line_backup);
        return;
    }
    // Jeśli nie przerwano działania funkcji, to znaczy, że parametry są
    // poprawne, dodaje bilet do wektora biletów.
    add_ticket(ticket_name, valid_time, price_int);
}

// Rozpatruje linię z komendą zapytania o trasę, wywołuje funkcję
// call_error jeśli któryś z paremetrów jest niepoprawny,
// lub jeśli nie da się przebyć tej trasy dodanymi kursami.
// Odpowiada na pytanie jakimi biletami najtaniej można przejechać zadaną trasę,
// wypisuje ":-( nazwa_przystanku", jeśli nie da się przebyć tej trasy bez
// czekania lub wypisuje ":-|" jeśli dodane bilety nie wystarzczą na
// przejechanie trasy
void execute_course_query(std::string &line, int line_number) {
    std::regex stop_name_regex("([a-zA-Z_^]+)");
    std::regex course_number_regex("([0-9]+)");
    std::string start, stop, course_number;
    std::string line_backup = line;

    start = first_match(line, stop_name_regex);

    course_number = first_match(line, course_number_regex);
    stop = first_match(line, stop_name_regex);
    unsigned course_number_int = string_to_ull(course_number);
    unsigned start_time, last_time;

    // Pierwsze 2 przystanki są sprawdzane osobno - sprawdzam czy linia istnieje,
    // czy podane przystanki należą do linii oraz czy następują po sobie w
    // dobrej kolejności.
    if(!line_id.count(course_number_int)) {
      call_error(line_number, line_backup);
      return;
    }
    if (timetable[line_id[course_number_int]].count(start)) {
        start_time = timetable[line_id[course_number_int]][start];
        if (timetable[line_id[course_number_int]].count(stop) &&
          start_time < timetable[line_id[course_number_int]][stop]) {
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

    // Każdy kolejny przystanek jest "doklejany" do poprzednich:
    // Sprawdzam czy podana linia istnieje, czy przystanek istnieje dla podanej
    // linii, czy czas dojadu na przystanek nie jest mniejszy bądź równy
    // czasowi z ostatniego przystanku oraz czy jest równy czasowi dojazdu
    // poprzedniej linii na ten przystanek.
    while (!line.empty()) {
        course_number = first_match(line, course_number_regex);
        course_number_int = string_to_ull(course_number);

        stop = first_match(line, stop_name_regex);

        if(!line_id.count(course_number_int)) {
          call_error(line_number, line_backup);
          return;
        }
        if (timetable[line_id[course_number_int]].count(start) &&
            timetable[line_id[course_number_int]].count(stop)) {
            unsigned on_start = timetable[line_id[course_number_int]][start];
            unsigned on_stop = timetable[line_id[course_number_int]][stop];
            if (last_time == on_start && on_start < on_stop) {
                last_time = on_stop;
                start = stop;
            } else {
                if (last_time >= on_start) {
                    call_error(line_number, line_backup);
                    return;
                }
                std::cout << ":-( " << start << "\n";
                return;
            }
        } else {
            call_error(line_number, line_backup);
            return;
        }
    }
    // Jeśli nie przerwano działania funkcji, to znaczy, że zapytanie jest
    // poprawne i należy wyliczyć wynik dla policzonego czasu jazdy.
    find_best_combination(last_time - start_time + 1);
}

// Wyszukuje we wczytanej linii którejś z dostępnych komend, jeśli odnajdzie
// dopasowanie, przekzuje linię odpowiedniej funkcji wywołującej.
// Jeśli linia nie pasuje do żadnego wzorca, wywołuje funkcję call_error.
// Ignoruje puste linie.
void execute_line(int line_number, std::string &line) {
    std::regex course_regex("^([0-9]+)((?: (?:(?:2[0-3]|1[0-9]|[1-9]):[0-5][0-9]) (?:[a-zA-Z_^]+)){2,})$");
    std::regex ticket_regex("^((?:[a-zA-Z ])+) ((?:[0-9])+\\.[0-9]{2}) ([1-9][0-9]*)$");
    std::regex query_regex("^\\? (?:[a-zA-Z_^]+)( (?:[0-9]+) (?:[a-zA-Z_^]+))+$");

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

// Funkcja sterująca programem.
// Wczytuje linie do napotkania końca pliku i przekazuje ich rozpatrzenie
// funkcji execute_line. Na koniec działania programu
// wypisuje liczbę kupionych biletów
int main() {
    std::string line;
    int line_number = 1;

    while (std::getline(std::cin, line)) {
        execute_line(line_number++, line);
    }

    std::cout << sold_tickets << "\n";

    return 0;
}
