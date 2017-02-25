#pragma once

#include <list>
#include <sqlite3.h>

constexpr char DATABASE_FILENAME[] = "/data/AccessData.db3";

using DatabaseRow = std::tuple <int, std::string, double, double>;
using DatabaseTable = std::list <DatabaseRow>;

class Database
{
public:
	enum DatabaseError
	{
		SUCCESS = 0,
		COULD_NOT_GET_TABLE,
	};

	Database(const std::string & kDatabaseFileName, const std::string & kDatabaseTableName);

	~Database();

	static int FindTicket(std::string &ticket_code);

private:
	bool OpenDatabase(const std::string & kDatabaseTableName);

	void CloseDatabase();

	bool CheckDatabaseValidity();

	DatabaseError DataImport(const std::string & kImportDataPath);

	DatabaseError GetDatabaseTable(DatabaseTable & a_table);

	sqlite3 *db_;
	char *zErrMsg_ = 0;
	int rc_;
	const std::string & kDatabaseFileName_ = "";
	const std::string & kDatabaseTableName_ = "";
};

