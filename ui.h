#pragma once

#include <iostream>

#include <Windows.h>

#include "database.h"

class UI
{
public:
	UI(const Database & database);
	~UI();

	int MenuMain();
	int MenuImport();
	int MenuTicketScan();
	int MenuStatistics();
	int MenuImprint();
	int MenuFail();

	Database database_;

	// Init console preferences:
	CONSOLE_SCREEN_BUFFER_INFO csbiInfo_;
	WORD wOldColorAttrs_;
	HANDLE hStdout_;

private:
	int Scanning(std::string &ticket_id);
	int TicketState(std::string &ticket_id);
	int Acknowledge();
	int ScanFail();
};