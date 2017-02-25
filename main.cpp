#include "ui.h"

int main(int argc, const char* argv[])
{
	UI user_interface;

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