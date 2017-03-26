#pragma once

#include <list>
#include <string>
#include <vector>

#include <sqlite3.h>

using DatabaseRow = std::tuple <int, std::string, double, double>;
using DatabaseTable = std::list <DatabaseRow>;

class Database
{
public:
	enum DatabaseError
	{
		SUCCESS = 0,
		COULD_NOT_GET_TABLE,
		COULD_NOT_READ_CSV_FILE,
		NO_TICKET_FOUND,
		MORE_THEN_ONE_TICKET_FOUND,
		TICKET_INVALID,
		UNKNOWN_TICKET_STATUS,
	};

	const std::vector<std::string> & kDatabaseTables_ = { "", "" };

	struct db_row {
		std::string barcode;
		std::string status;
		std::string ticketArea;
		std::string ticketType;
		std::string price;
	};

	Database(const std::string & kDatabaseFileName, const std::vector<std::string> & kDatabaseTables);

	~Database();

	void ImportData();

	bool Database::CheckTablesForData();

	DatabaseError FindTicket(std::string &ticket_code, db_row & result_row);

	int Database::GetNumberOfTicketsWhereColumnIsValue(
		const std::vector <std::string> & tables_to_search,
		const std::string & a_column,
		const std::string & a_value);

	void WriteStatisticEntry(std::string a_barcode, std::string a_returncode);

private:

	const std::string & kDatabaseFileName_ = "";

	bool db_available_and_valid_ = false;
	sqlite3 * database_connection_ = nullptr;

	bool OpenDatabase(const std::string & kDatabaseFileName);

	void CloseDatabase();

	bool CheckDatabaseValidity();

	DatabaseError GetDataFromCsvFile(const std::string & kImportDataPath, const std::string & a_table);

	void Split(const std::string& str, const std::string& delim, std::vector<std::string>& parts);

	void InsertLine(db_row &a_db_raw, const std::string & a_db_table_name);
};

