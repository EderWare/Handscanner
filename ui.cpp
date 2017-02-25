#include <iostream>

#include "common.h"
#include "database.h"
#include "ui.h"


UI::UI()
: hStdout_ (GetStdHandle(STD_OUTPUT_HANDLE))
{
	// Save standard console settings
	GetConsoleScreenBufferInfo(hStdout_, &csbiInfo_);
	wOldColorAttrs_ = csbiInfo_.wAttributes;
}


UI::~UI()
{
}


// Menu

int UI::MenuMain()
{
	int choice;

	std::cout << "1. Scannen" << std::endl;
	std::cout << "2. Statistik" << std::endl;
	std::cout << "\n8. Impressum" << std::endl;
	std::cout << "9. Anwendung beenden" << std::endl;
	std::cout << "\nBitte durch Eingabe der Zahl w\x84 \bhlen und mit Enter best\x84 \btigen: " << std::endl;

	Common::get_value_from_cin<int>(choice);

	system("cls");
	return choice;
}

int UI::MenuTicketScan()
{
	auto scan_state = 0;
	std::string ticket_id = "";

	system("cls");

	do
	{
		switch (scan_state)
		{
		case 0: // Scannen
		{
			scan_state = Scanning(ticket_id);
			break;
		}
		case 1: // Ticketstatus
		{
			scan_state = TicketState(ticket_id);
			break;
		}
		case 2: // Quittieren
		{
			scan_state = Acknowledge();
			break;
		}
		default: // Fehler: Falsche Eingabe!
		{
			scan_state = ScanFail();
			break;
		}
		}
	} while (scan_state != 9);

	return 0;
}

int UI::MenuStatistics()
{
	std::cout << "Noch nicht Implementiert! Zur\x81 \bck zum Hauptmen\x81..." << std::endl;

	system("pause");
	system("cls");
	return 0;
}

int UI::MenuImprint()
{
	std::cout << "Diese Software dient zum erfassen von Tickets der Firma Eventim," << std::endl;
	std::cout << "auf Basis der bereitgestellten Access-Datei.\n" << std::endl;
	std::cout << "Verein:\t\tZiegenzuchtfreunde Wei\xe1 \be Wolke Bergheim 1979 e.V." << std::endl;
	std::cout << "Entwickler:\tLars Wagner (larswagner@hotmail.de)" << std::endl;
	std::cout << "\nLizenz: GPL Version 3\n" << std::endl;

	system("pause");
	system("cls");
	return 0;
}

int UI::MenuFail()
{
	// Konsole: Schriftfarbe == ROT
	SetConsoleTextAttribute(hStdout_, FOREGROUND_RED | FOREGROUND_INTENSITY);
	std::cout << "FEHLER: Unzul\x84 \bssige Eingabe!" << std::endl;
	// Konsole: Schriftfarbe == STANDARD
	SetConsoleTextAttribute(hStdout_, wOldColorAttrs_);

	system("pause");
	system("cls");
	return 0;
}

// Scan tickets

int UI::Scanning(std::string &ticket_id)
{
	auto scan_state = 9; // Default: Zur�ck zum Hauptmen�

	std::cout << "\nCode einscannen (Zur\x81 \bck zum Hauptmen\x81 mit 9 + ENTER):\t";

	Common::get_value_from_cin(ticket_id);

	if (ticket_id.compare("9999") && ticket_id.compare("9"))
	{
		scan_state = 1; // Zum G�ltigkeitscheck
	}

	system("cls");
	return scan_state;
}

int UI::TicketState(std::string &ticket_id)
{
	auto scan_state = 0; // Default: Ticket OK --> Zur�ck zum Scannen

	if (Database::FindTicket(ticket_id))
	{
		system("cls");
		// Konsole: Schriftfarbe == GR�N
		SetConsoleTextAttribute(hStdout_, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		std::cout << "Ticket " << ticket_id << " ist g\x81 \bltig!\n" << std::endl;
		// Konsole: Schriftfarbe == STANDARD
		SetConsoleTextAttribute(hStdout_, wOldColorAttrs_);
	}
	else
	{
		// Konsole: Schriftfarbe == ROT
		SetConsoleTextAttribute(hStdout_, FOREGROUND_RED | FOREGROUND_INTENSITY);
		std::cout << "FEHLER: Unzul\x84 \bssige Eingabe!" << std::endl;
		scan_state = 2; // Zum Quittieren
	}

	return scan_state;
}

int UI::Acknowledge()
{
	auto input = 0;

	do
	{
		std::cout << "\nFehlerhaftes Ticket Quittieren (Quittieren mit 8 + ENTER):\t";
		Common::get_value_from_cin<int>(input);
		system("cls");
	} while (input != 8888 && input != 8);

	// Konsole: Schriftfarbe == STANDARD
	SetConsoleTextAttribute(hStdout_, wOldColorAttrs_);

	return 0; // Zur�ck zum Scannen
}

int UI::ScanFail()
{
	// Konsole: Schriftfarbe == ROT
	SetConsoleTextAttribute(hStdout_, FOREGROUND_RED | FOREGROUND_INTENSITY);
	std::cout << "FEHLER: Unzul\x84 \bssiger Status. Kontaktieren Sie den Entwickler." << std::endl;
	// Konsole: Schriftfarbe == STANDARD
	SetConsoleTextAttribute(hStdout_, wOldColorAttrs_);

	system("pause");
	system("cls");
	return 9; // Zur�ck zum Hauptmen�
}