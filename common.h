#pragma once

#include <iostream>
#include <string>

#include <time.h>

class Common
{
public:
	Common()
	{ }

	~Common()
	{ }

	template <typename T>
	static void get_value_from_cin(T & a_value)
	{
		a_value = 0;
		std::cin.clear();
		std::cin.ignore(std::cin.rdbuf()->in_avail());
		std::cin >> a_value;
	}

	template <>
	static void get_value_from_cin(std::string & a_value)
	{
		a_value = "";
		std::cin.clear();
		std::cin.ignore(std::cin.rdbuf()->in_avail());
		std::getline(std::cin, a_value);
	}

	static std::string getTimestamp()
	{
		time_t result = time(NULL);
		char timestamp[26];

		ctime_s(timestamp, sizeof timestamp, &result);

		return (std::string(timestamp));
	}
};

