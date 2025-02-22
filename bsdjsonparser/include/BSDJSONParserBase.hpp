#ifndef BSDJSONPARSERBASE_HPP_
#define BSDJSONPARSERBASE_HPP_

#include <iostream>
#include <string>
#include <nlohmann/json.hpp> // For JSON handling (install nlohmann/json)
#include "config.h"

using json = nlohmann::json;

class BSDJSONParserBase {
public:
    BSDJSONParserBase(std::string json_val_field_name);
    int getValue() const;
protected:
    std::string json_value_field_name;
	std::string city_name;
	long user_count;
	long value_sum;
	
	virtual bool is_valid_name(std::string name);
	virtual float get_avg_value();
	virtual std::string get_avg_value_as_string(int decimal_count);
	virtual void printout_info();
	virtual json get_results(json js) = 0;

	/**
	 * Converts the city name and average value related data to JSON object for single city.
	 *
	 * @return json object containing city name and average age of users
	 */
	virtual json to_json();
};

#endif // MY_LIBRARY_MY_CLASS_HPP
