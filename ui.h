#pragma once

#include <iostream>

#include <Windows.h>

class UI
{
public:
	UI();
	~UI();

	int MenuMain();
	int MenuTicketScan();
	int MenuStatistics();
	int MenuImprint();
	int MenuFail();

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