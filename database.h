#pragma once

#include <sqlite3.h>

class database
{
public:
	database();
	~database();

private:
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
};

