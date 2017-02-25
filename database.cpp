#include <iostream>
#include <vector>

#include "database.h"


Database::Database(const std::string & kDatabaseFileName, const std::string & kDatabaseTableName)
{
}


Database::~Database()
{
}


int Database::FindTicket(std::string &ticket_code)
{
	auto ret = 0;
	std::vector<std::string> v1 = {
		"105343199102261",
		"105981211726487",
		"105195948432983",
		"105709194103941"
	};

	for (const auto item : v1) {
		if (item.compare(ticket_code) == 0)
		{
			ret = 1;
			break;
		}
	}

	return ret;
}


bool Database::OpenDatabase(const std::string & kDatabaseTableName)
{
	return true;
}


void Database::CloseDatabase()
{

}


bool Database::CheckDatabaseValidity()
{
	return true;
}


Database::DatabaseError
Database::DataImport(const std::string & kImportDataPath)
{
	return DatabaseError::SUCCESS;
}


Database::DatabaseError
Database::GetDatabaseTable(DatabaseTable & a_table)
{
	return DatabaseError::SUCCESS;
}