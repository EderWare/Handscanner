#include <iostream>

#include "common.h"
#include "database.h"
#include "ui.h"


UI::UI(const Database & database)
: hStdout_ (GetStdHandle(STD_OUTPUT_HANDLE))
, database_ (database)
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

	system("cls");

	std::cout << "1. Scannen" << std::endl;
	std::cout << "2. Statistik" << std::endl;
	std::cout << "3. Datenimport" << std::endl;
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

	if (!database_.CheckTablesForData())
	{
		std::cout << "Keine Daten vorhanden. Bitte Daten importieren!\n" << std::endl;
		system("pause");
		return 0;
	}

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


int UI::MenuImport()
{
	int rc = 0;
	int choice;

	system("cls");
	std::cout << "Folgende 2 Dateien m\x81 \bssen im Programmordner vorhanden sein:" << std::endl;
	std::cout << "    eventim_access_list.csv" << std::endl;
	std::cout << "    custom_access_list.csv" << std::endl;
	std::cout << "Sind Sie nicht vorhanden, folgt eine Fehlermeldung. Diese ist mit Enter zu best\x84 \btigen." << std::endl;
	
	std::cout << "\n1. Importiere Dateien" << std::endl;
	std::cout << "9. Zur\x81 \bck zum Hauptmen\x81" << std::endl;

	std::cout << "\nBitte durch Eingabe der Zahl w\x84 \bhlen und mit Enter best\x84 \btigen: " << std::endl;

	Common::get_value_from_cin<int>(choice);

	if (choice == 1)
	{
		system("cls");
		database_.ImportData();
		rc = 0;
	}
	else if (choice == 9)
	{
		rc = 0;
	}

	system("cls");
	return rc;
}


int UI::MenuStatistics()
{
	auto number_of_valid_tickets = database_.GetNumberOfTicketsWhereColumnIsValue(
		database_.kDatabaseTables_, "status", "valid");
	auto number_of_invalid_tickets = database_.GetNumberOfTicketsWhereColumnIsValue(
		database_.kDatabaseTables_, "status", "invalid");
	auto number_of_tickets = number_of_valid_tickets + number_of_invalid_tickets;

	std::cout << "Statistik:\n" << std::endl;
	std::cout << "Anzahl Tickets:\t\t" << number_of_tickets << std::endl;
	std::cout << "\tG\x81 \bltig:\t\t" << number_of_valid_tickets << std::endl;
	std::cout << "      Ung\x81 \bltig:\t\t" << number_of_invalid_tickets << "\n" << std::endl;

	const std::vector <std::string> statistic_table = {"statistic"};
	auto number_of_used_tickets = database_.GetNumberOfTicketsWhereColumnIsValue(
		statistic_table, "returncode", "SUCCESS");
	auto number_of_unused_tickets = number_of_valid_tickets;

	std::cout << "Abgerufene Tickets:\t" << number_of_invalid_tickets << std::endl;
	std::cout << "Offene Tickets:\t\t" << number_of_unused_tickets << "\n" << std::endl;

	system("pause");
	system("cls");
	return 0;
}


int UI::MenuImprint()
{
	std::cout << "Diese Software dient zum Erfassen von eigenen Tickets und Tickets der Firma Eventim," << std::endl;
	std::cout << "auf Basis der bereitgestellten Access-Dateien.\n" << std::endl;
	std::cout << "Lizensierter Verein:\t\tZiegenzuchtfreunde Wei\xe1 \be Wolke Bergheim 1979 e.V." << std::endl;
	std::cout << "Entwickler:\t\t\tLars Wagner (larswagner@hotmail.de)" << std::endl;
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
	auto scan_state = 9; // Default: Zurück zum Hauptmenü

	std::cout << "\nCode einscannen (Zur\x81 \bck zum Hauptmen\x81 mit 9 + ENTER):\t";

	Common::get_value_from_cin(ticket_id);

	if (ticket_id.compare("9999") && ticket_id.compare("9"))
	{
		scan_state = 1; // Zum Gültigkeitscheck
	}

	system("cls");
	return scan_state;
}


int UI::TicketState(std::string &ticket_id)
{
	auto scan_state = 0; // Default: Ticket OK --> Zurück zum Scannen
	Database::db_row result;
	Database::DatabaseError error_flag;

	error_flag = database_.FindTicket(ticket_id, result);

	if (error_flag == Database::DatabaseError::SUCCESS)
	{
		database_.WriteStatisticEntry(ticket_id, "SUCCESS");
		system("cls");
		// Konsole: Schriftfarbe == GRÜN
		SetConsoleTextAttribute(hStdout_, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		std::cout << result.ticketArea << " " << result.barcode << " zum " << result.ticketType << " von " << result.price << " Euro ist " << result.status << "!\n" << std::endl;
		// Konsole: Schriftfarbe == STANDARD
		SetConsoleTextAttribute(hStdout_, wOldColorAttrs_);
	}
	else
	{
		// Set console collor to red
		SetConsoleTextAttribute(hStdout_, FOREGROUND_RED | FOREGROUND_INTENSITY);

		switch (error_flag)
		{
		case Database::DatabaseError::NO_TICKET_FOUND:
			database_.WriteStatisticEntry(ticket_id, "NO_TICKET_FOUND");
			std::cout << "FEHLER: Kein Ticket gefunden!" << std::endl;
			break;
		case Database::DatabaseError::MORE_THEN_ONE_TICKET_FOUND:
			database_.WriteStatisticEntry(ticket_id, "MORE_THEN_ONE_TICKET_FOUND");
			std::cout << "FEHLER: Mehr als ein Ticket gefunden!" << std::endl;
			break;
		case Database::DatabaseError::TICKET_INVALID:
			database_.WriteStatisticEntry(ticket_id, "TICKET_INVALID");
			std::cout << "FEHLER: Ticket ung\x81 \bltig!" << std::endl;
			break;
		case Database::DatabaseError::UNKNOWN_TICKET_STATUS:
			database_.WriteStatisticEntry(ticket_id, "UNKNOWN_TICKET_STATUS");
			std::cout << "FEHLER: Unbekannter Ticketstatus!" << std::endl;
			break;
		default:
			std::cout << "FEHLER: Unzul\x84 \bssige Eingabe!" << std::endl;
			break;
		}

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

	return 0; // Zurück zum Scannen
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
	return 9; // Zurück zum Hauptmenü
}