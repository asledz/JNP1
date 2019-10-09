#include<iostream>
#include<vector>
#include<map>
#include<set>
#include<string>

using namespace std;

/* Funkcje użytkowe */

int time_to_minutes (int hour, int minutes) {
	return 60 * hour + minutes;
}

/* Funkcje obsługi rozkładu jazdy */

// Skalowanie numerów linii
map<int, int> line_id;

// Przechowywanie linii tramwajowych
// Dla każdej linii(kursu) trzymamy godzinę przyjazdu na przystanek  
vector<map<string, int>> timetable;

// Opis biletu - dla każdej nazwy trzymamy długość trwania, cenę
vector<pair<string, pair<double, int>>> tickets;

// Sprawdza, czy dany numer linii kursu tramwajowego zatrzymuje się o danej godzinie na przystanku.
bool stop_exists (int number_of_the_line, string name_of_the_stop, int hour, int minutes) {
	// nie istnieje taka linia tramwajowa
	if(line_id.find(number_of_the_line) == line_id.end() ) {
		return false;
	}
	int id = line_id[number_of_the_line];
	if(timetable[id][name_of_the_stop] == time_to_minutes(hour, minutes)) {
		return true;
	}
	else {
		return false;
	}
}

/* Funkcje obługi biletów */

string name_of_ticket(int id) {
	return tickets[id].first;
}

double prize_of_ticket (int id) {
	return tickets[id].second.first;
}

int duration_of_ticket (int id) {
	return tickets[id].second.second;
}



int main () {
	return 0;
}