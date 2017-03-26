#include "ui.h"
#include "database.h"

int main(int argc, const char* argv[])
{
	std::string database_name = "ticket_database";
	const std::vector<std::string> tables = { "custom", "eventim" };
	Database database(database_name, tables);

	UI user_interface(database);

	auto state = 0;

	do
	{
		switch (state)
		{
			case 0: // Hauptmenü
			{
				state = user_interface.MenuMain();
				break;
			}
			case 1: // Scannen
			{
				state = user_interface.MenuTicketScan();
				break;
			}
			case 2: // Statistik
			{
				state = user_interface.MenuStatistics();
				break;
			}
			case 3: // Datenimport
			{
				state = user_interface.MenuImport();
				break;
			}
			case 8: // Impressum
			{
				state = user_interface.MenuImprint();
				break;
			}
			default: // Fehler: Falsche Eingabe!
			{
				state = user_interface.MenuFail();
				break;
			}
		}
	} while (state != 9);
}