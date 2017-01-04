#include <iostream>
#include <string>
#include <vector>

#include <Windows.h>

#include "database.h"

using namespace std;

// Main menu
int main_menu();
int scan_tickets();
int statistics();
int imprint();
int menu_fail();

// Scan tickets
int scanning(string &ticket_id);
int ticket_state(string &ticket_id);
int acknowledge();
int scan_fail();
int find_ticket(string &string_to_compare);

// Common
template <typename T>
void get_value_from_cin(T &a_value);
template <>
void get_value_from_cin(string &a_value);

// Init console preferences:
CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
WORD wOldColorAttrs;

auto hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

int main(int argc, const char* argv[])
{
	// Save standard console settings
	GetConsoleScreenBufferInfo(hStdout, &csbiInfo);
	wOldColorAttrs = csbiInfo.wAttributes;

	auto state = 0;

	do
	{
		switch (state)
		{
			case 0: // Hauptmenü
			{
				state = main_menu();
				break;
			}
			case 1: // Scannen
			{
				state = scan_tickets();
				break;
			}
			case 2: // Statistik
			{
				state = statistics();
				break;
			}
			case 8: // Impressum
			{
				state = imprint();
				break;
			}
			default: // Fehler: Falsche Eingabe!
			{
				state = menu_fail();
				break;
			}
		}
	} while (state != 9);
}

// Main menu

int main_menu()
{
	int choice;

	cout << "1. Scannen" << endl;
	cout << "2. Statistik" << endl;
	cout << "\n8. Impressum" << endl;
	cout << "9. Anwendung beenden" << endl;
	cout << "\nBitte durch Eingabe der Zahl w\x84 \bhlen und mit Enter best\x84 \btigen: " << endl;

	get_value_from_cin<int>(choice);

	system("cls");
	return choice;
}

int scan_tickets()
{
	auto scan_state = 0;
	string ticket_id = "";

	system("cls");

	do
	{
		switch(scan_state)
		{
			case 0: // Scannen
			{
				scan_state = scanning(ticket_id);
				break;
			}
			case 1: // Ticketstatus
			{
				scan_state = ticket_state(ticket_id);
				break;
			}
			case 2: // Quittieren
			{
				scan_state = acknowledge();
				break;
			}
			default: // Fehler: Falsche Eingabe!
			{
				scan_state = scan_fail();
				break;
			}
		}
	} while (scan_state != 9);

	return 0;
}

int statistics()
{
	cout << "Noch nicht Implementiert! Zur\x81 \bck zum Hauptmen\x81..." << endl;

	system("pause");
	system("cls");
	return 0;
}

int imprint()
{
	cout << "Diese Software dient zum erfassen von Tickets der Firma Eventim," << endl;
	cout << "auf Basis der bereitgestellten Access-Datei.\n" << endl;
	cout << "Verein:\t\tZiegenzuchtfreunde Wei\xe1 \be Wolke Bergheim 1979 e.V." << endl;
	cout << "Entwickler:\tLars Wagner (larswagner@hotmail.de)" << endl;
	cout << "\nLizenz: GPL Version 3\n" << endl;
	
	system("pause");
	system("cls");
	return 0;
}

int menu_fail()
{
	// Konsole: Schriftfarbe == ROT
	SetConsoleTextAttribute(hStdout, FOREGROUND_RED | FOREGROUND_INTENSITY);
	cout << "FEHLER: Unzul\x84 \bssige Eingabe!" << endl;
	// Konsole: Schriftfarbe == STANDARD
	SetConsoleTextAttribute(hStdout, wOldColorAttrs);

	system("pause");
	system("cls");
	return 0;
}

// Scan tickets

int scanning(string &ticket_id)
{
	auto scan_state = 9; // Default: Zurück zum Hauptmenü

	cout << "\nCode einscannen (Zur\x81 \bck zum Hauptmen\x81 mit 9 + ENTER):\t";

	get_value_from_cin(ticket_id);

	if (ticket_id.compare("9999") && ticket_id.compare("9"))
	{
		scan_state = 1; // Zum Gültigkeitscheck
	}

	system("cls");
	return scan_state;
}

int ticket_state(string &ticket_id)
{
	auto scan_state = 0; // Default: Ticket OK --> Zurück zum Scannen

	if (find_ticket(ticket_id))
	{
		system("cls");
		// Konsole: Schriftfarbe == GRÜN
		SetConsoleTextAttribute(hStdout, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		cout << "Ticket " << ticket_id << " ist g\x81 \bltig!\n" << endl;
		// Konsole: Schriftfarbe == STANDARD
		SetConsoleTextAttribute(hStdout, wOldColorAttrs);
	}
	else
	{
		// Konsole: Schriftfarbe == ROT
		SetConsoleTextAttribute(hStdout, FOREGROUND_RED | FOREGROUND_INTENSITY);
		cout << "FEHLER: Unzul\x84 \bssige Eingabe!" << endl;
		scan_state = 2; // Zum Quittieren
	}

	return scan_state;
}

int acknowledge()
{
	auto input = 0;

	do
	{
		cout << "\nFehlerhaftes Ticket Quittieren (Quittieren mit 8 + ENTER):\t";
		get_value_from_cin<int>(input);
		system("cls");
	} while (input != 8888 && input != 8);

	// Konsole: Schriftfarbe == STANDARD
	SetConsoleTextAttribute(hStdout, wOldColorAttrs);

	return 0; // Zurück zum Scannen
}

int scan_fail()
{
	// Konsole: Schriftfarbe == ROT
	SetConsoleTextAttribute(hStdout, FOREGROUND_RED | FOREGROUND_INTENSITY);
	cout << "FEHLER: Unzul\x84 \bssiger Status. Kontaktieren Sie den Entwickler." << endl;
	// Konsole: Schriftfarbe == STANDARD
	SetConsoleTextAttribute(hStdout, wOldColorAttrs);
	
	system("pause");
	system("cls");
	return 9; // Zurück zum Hauptmenü
}

int find_ticket(string &string_to_compare)
{
	auto ret = 0;
	vector<string> v1 = { "12345", "23456", "34567", "1020000081631920000000" };

	for (const auto item : v1){
		if (item.compare(string_to_compare) == 0)
		{
			ret = 1;
			break;
		}
	}

	return ret;
}

// Common

template <typename T>
void get_value_from_cin(T &a_value)
{
	a_value = 0;
	std::cin.clear();
	std::cin.ignore(std::cin.rdbuf()->in_avail());
	cin >> a_value;
}

template <>
void get_value_from_cin(string &a_value)
{
	a_value = "";
	std::cin.clear();
	std::cin.ignore(std::cin.rdbuf()->in_avail());
	getline(std::cin, a_value);
}