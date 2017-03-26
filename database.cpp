#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#include "common.h"

#include "database.h"


Database::Database(const std::string & kDatabaseFileName, const std::vector<std::string> & kDatabaseTables)
	: kDatabaseFileName_(kDatabaseFileName)
	, kDatabaseTables_(kDatabaseTables)
	, db_available_and_valid_(false)
{
	db_available_and_valid_ = OpenDatabase(kDatabaseFileName_ + ".db3");
	assert(db_available_and_valid_ && "Can not open database!");

	std::cout << "Open database successfully." << std::endl;
}


Database::~Database()
{
	CloseDatabase();
}


void Database::ImportData()
{
	auto rc = DatabaseError::SUCCESS;

	// Clean tables if data exists
	std::vector <std::string> kTablesToClean = kDatabaseTables_;
	kTablesToClean.push_back("statistic");

	for (auto db_table : kTablesToClean)
	{
		char ** result = nullptr;

		const std::string kCheckTable(
			"SELECT COUNT(*) FROM " + db_table
		);

		sqlite3_get_table(database_connection_, kCheckTable.c_str(), &result, nullptr, nullptr, nullptr);

		if (atoi(result[1]) != 0)
		{
			const std::string kCleanTable(
				"DELETE FROM " + db_table
			);

			sqlite3_exec(database_connection_, kCleanTable.c_str(), nullptr, nullptr, nullptr);
		}
	}

	// Import from CSV files
	for (auto table : kDatabaseTables_)
	{
		if (table.compare("eventim") == 0)
		{
			rc = GetDataFromCsvFile("eventim_access_list.csv", table);
		}
		else if (table.compare("custom") == 0)
		{
			rc = GetDataFromCsvFile("custom_access_list.csv", table);
		}
		else
		{
			std::cerr << "Unknown database table " << table << " !" << std::endl;
		}
	}

	if (rc != DatabaseError::SUCCESS)
	{
		system("pause");
	}
}


bool Database::CheckTablesForData()
{
	auto rc = true;

	for (auto db_table : kDatabaseTables_)
	{
		char ** result = nullptr;

		const std::string kCheckTable(
			"SELECT COUNT(*) FROM " + db_table
		);

		sqlite3_get_table(database_connection_, kCheckTable.c_str(), &result, nullptr, nullptr, nullptr);

		if (atoi(result[1]) == 0)
		{
			rc = false;
		}
	}

	return rc;
}


Database::DatabaseError
Database::FindTicket(std::string & ticket_code, db_row & result_row)
{
	DatabaseError rc = DatabaseError::SUCCESS;
	char ** number_of_matches = nullptr;
	std::vector <std::string> ticket_find_tables;

	for (auto db_table : kDatabaseTables_)
	{
		const std::string kCountRows(
			"SELECT COUNT(*) FROM " + db_table + " WHERE barcode = \"" + ticket_code + "\""
		);

		if (SQLITE_OK == sqlite3_get_table(database_connection_, kCountRows.c_str(), &number_of_matches, nullptr, nullptr, nullptr))
		{
			if (atoi(number_of_matches[1]) > 0)
			{
				ticket_find_tables.push_back(db_table);
			}
		}
	}

	if (ticket_find_tables.size() == 0)
	{
		rc = DatabaseError::NO_TICKET_FOUND;
	}
	else if (ticket_find_tables.size() == 1)
	{
		for (auto db_table : ticket_find_tables)
		{
			char ** a_row = nullptr;
			int sum_rows = 0;
			int sum_columns = 0;

			const std::string kGetRow(
				"SELECT * FROM " + db_table + " WHERE barcode = \"" + ticket_code + "\""
			);

			const std::string kSetStatusToInvalid(
				"UPDATE " + db_table + "\n"
				"SET status = 'invalid'\n"
				"WHERE barcode = \"" + ticket_code + "\";"
			);

			if (SQLITE_OK == sqlite3_get_table(database_connection_, kGetRow.c_str(), &a_row, &sum_rows, &sum_columns, nullptr))
			{
				// Find Ticket in database:
				if (sum_rows == 0)
				{
					rc = DatabaseError::NO_TICKET_FOUND;
				}
				else if (sum_rows == 1)
				{
					// Check if ticket is valid:
					std::string status = std::string(a_row[sum_columns + (2 - 1)]);
					if (status.compare("valid") == 0)
					{
						while (SQLITE_OK != sqlite3_exec(database_connection_, kSetStatusToInvalid.c_str(), nullptr, nullptr, nullptr));

						result_row.barcode = std::string(a_row[sum_columns + (1 - 1)]);
						result_row.status = std::string(a_row[sum_columns + (2 - 1)]);
						result_row.ticketArea = std::string(a_row[sum_columns + (3 - 1)]);
						result_row.ticketType = std::string(a_row[sum_columns + (4 - 1)]);
						result_row.price = std::string(a_row[sum_columns + (5 - 1)]);
					}
					else if (status.compare("invalid") == 0)
					{
						rc = DatabaseError::TICKET_INVALID;
					}
					else
					{
						rc = DatabaseError::UNKNOWN_TICKET_STATUS;
					}
				}
				else
				{
					rc = DatabaseError::MORE_THEN_ONE_TICKET_FOUND;
				}
			}
			else
			{
				rc = DatabaseError::COULD_NOT_GET_TABLE;
			}
		}
	}
	else if (ticket_find_tables.size() > 1)
	{
		rc = DatabaseError::MORE_THEN_ONE_TICKET_FOUND;
	}
	else
	{
		rc = DatabaseError::COULD_NOT_GET_TABLE;
	}

	return rc;
}


int Database::GetNumberOfTicketsWhereColumnIsValue(
	const std::vector <std::string> & tables_to_search,
	const std::string & a_column,
	const std::string & a_value)
{
	int number_of_tickets = 0;

	for (auto db_table : tables_to_search)
	{
		char ** result = nullptr;

		// SELECT COUNT(*) FROM eventim WHERE status="invalid"
		const std::string kCheckTable(
			"SELECT COUNT(*) FROM " + db_table + " WHERE " + a_column + "=\"" + a_value + "\""
		);

		sqlite3_get_table(database_connection_, kCheckTable.c_str(), &result, nullptr, nullptr, nullptr);

		number_of_tickets = number_of_tickets + atoi(result[1]);
	}

	return number_of_tickets;
}

void Database::WriteStatisticEntry(std::string a_barcode, std::string a_returncode)
{
	const std::string kInsertStatisticRow(
		"INSERT INTO statistic ( \n"
		"  barcode,\n"
		"  returncode,\n"
		"  timestamp )\n"
		"VALUES ( \n"
		"  \"" + a_barcode + "\",\n"
		"  \"" + a_returncode + "\",\n"
		"  \"" + Common::getTimestamp() + "\" );"
	);

	while (SQLITE_OK != sqlite3_exec(database_connection_, kInsertStatisticRow.c_str(), nullptr, nullptr, nullptr));
}


bool Database::OpenDatabase(const std::string & kDatabaseFileName)
{
	auto rc = sqlite3_open(kDatabaseFileName.c_str(), &database_connection_);

	if (rc)
	{
		db_available_and_valid_ = false;
	}
	else
	{
		db_available_and_valid_ = CheckDatabaseValidity();
	}

	return db_available_and_valid_;
}


void Database::CloseDatabase()
{
	if (SQLITE_OK == sqlite3_close_v2(database_connection_))
	{
		std::cout << "Can't close database: " << sqlite3_errmsg(database_connection_) << std::endl;
	}
	else
	{
		database_connection_ = nullptr;
		std::cout << "Closed database successfully." << std::endl;
	}
}


bool Database::CheckDatabaseValidity()
{
	bool rc;
	char * errmsg = nullptr;

	for (auto db_table : kDatabaseTables_)
	{
		const std::string kCheckTable(
			"SELECT barcode, status, ticketArea, ticketType, price"
			" FROM " + db_table +
			" LIMIT 1"
		);

		const std::string kCreateTable(
			"CREATE TABLE " + db_table + " (\n"
			"barcode TEXT NOT NULL,\n"
			"status TEXT NOT NULL,\n"
			"ticketArea TEXT NOT NULL,\n"
			"ticketType TEXT NOT NULL,\n"
			"price TEXT NOT NULL );\n"
		);

		rc = (SQLITE_OK == sqlite3_exec(database_connection_, kCheckTable.c_str(), nullptr, nullptr, &errmsg));

		if (nullptr != errmsg)
		{
			sqlite3_free(errmsg);
		}

		if (!rc)
		{
			rc = (SQLITE_OK == sqlite3_exec(database_connection_, kCreateTable.c_str(), nullptr, nullptr, &errmsg));

			if (nullptr != errmsg)
			{
				sqlite3_free(errmsg);
				break;
			}
		}
		else
		{
			break;
		}
	}

	// Statistic Table
	const std::string kCheckStatisticTable(
		"SELECT barcode, returncode, timestamp"
		" FROM statistic LIMIT 1"
	);

	const std::string kCreateStatisticTable(
		"CREATE TABLE statistic (\n"
		"barcode TEXT NOT NULL,\n"
		"returncode TEXT NOT NULL,\n"
		"timestamp TEXT NOT NULL );\n"
	);

	rc = (SQLITE_OK == sqlite3_exec(database_connection_, kCheckStatisticTable.c_str(), nullptr, nullptr, &errmsg));

	if (nullptr != errmsg)
	{
		sqlite3_free(errmsg);
	}

	if (!rc)
	{
		rc = (SQLITE_OK == sqlite3_exec(database_connection_, kCreateStatisticTable.c_str(), nullptr, nullptr, &errmsg));

		if (nullptr != errmsg)
		{
			sqlite3_free(errmsg);
		}
	}

	return rc;
}


Database::DatabaseError
Database::GetDataFromCsvFile(const std::string & kImportDataPath, const std::string & a_table)
{
	auto rc = DatabaseError::SUCCESS;
	std::ifstream csvread;
	csvread.open(kImportDataPath, std::ios::in);
	if (csvread)
	{
		std::string line = "";
		db_row row_to_insert = {"", "", "", "", ""};
		std::vector<std::string> parts;

		std::cout << "Importiere Daten. Warten..." << std::endl;

		//Erste Reihe verwerfen:
		std::getline(csvread, line);

		while (std::getline(csvread, line))
		{
			line.erase(std::remove(line.begin(), line.end(), '\r'),
				line.end());
			Split(line, ";", parts);

			line.clear();

			auto counter = 0;
			for (auto &item : parts)
			{
				switch (counter)
				{
				case 0:
					row_to_insert.barcode = item;
					break;
				case 1:
					row_to_insert.status = item;
					break;
				case 2:
					row_to_insert.ticketArea = item;
					break;
				case 3:
					row_to_insert.ticketType = item;
					break;
				case 4:
					row_to_insert.price = item;
					break;
				}

				counter++;
				if (counter == 5)
				{
					counter = 0;
				}
			}
			parts.clear();

			InsertLine(row_to_insert, a_table);
		}
		csvread.close();
	}
	else
	{
		std::cerr << "Error by reading CSV file " << kImportDataPath << "!" << std::endl;
		rc = DatabaseError::COULD_NOT_READ_CSV_FILE;
	}

	return rc;
}


void Database::Split(const std::string& str, const std::string& delim, std::vector<std::string>& parts)
{
	size_t start, end = 0;
	while (end < str.size()) {
		start = end;
		while (start < str.size() && (delim.find(str[start]) != std::string::npos)) {
			start++;  // Skip initial whitespace
		}
		end = start;
		while (end < str.size() && (delim.find(str[end]) == std::string::npos)) {
			end++; // Skip to end of word
		}
		if (end - start != 0) {  // Just ignore zero-length strings.
			parts.push_back(std::string(str, start, end - start));
		}
	}
}

void Database::InsertLine (db_row &a_db_raw, const std::string & a_db_table_name)
{
	const std::string kInsertLine(
		"INSERT INTO " + a_db_table_name + " ( "
		"barcode, status, ticketArea, ticketType, price )\n"
		"VALUES ( "
		+ "\"" + a_db_raw.barcode + "\", "
		+ "\"" + a_db_raw.status + "\", "
		+ "\"" + a_db_raw.ticketArea + "\", "
		+ "\"" + a_db_raw.ticketType + "\", "
		+ "\"" + a_db_raw.price + "\" );"
	);

	if (SQLITE_OK != sqlite3_exec(database_connection_, kInsertLine.c_str(), nullptr, nullptr, nullptr))
	{
		std::cerr << "Can't insert data into database!" << std::endl;
	}
}