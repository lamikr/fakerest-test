#ifndef BSDJSONPARSERBASE_HPP_
#define BSDJSONPARSERBASE_HPP_

#include <iostream>
#include <string>
#include <nlohmann/json.hpp> // For JSON handling (install nlohmann/json)
#include "config.h"

using json = nlohmann::json;

class BSDJSONParserBase {
public:
	/*
	 * @param json_val_field_name key name of the parameter that is added to produced JSON during calculations
	 */
    BSDJSONParserBase(std::string json_val_field_name);
	/* This function calculates the results for the calculation.
	 * 
	 * @param js Input JSON data used for the calculation
	 * @return JSON data calculated as a result.
	 */
	virtual json get_results(json js) = 0;    
protected:
    std::string json_value_field_name;
	std::string city_name;
	long user_count;
	/* helper to count the sum of values before dividing it with the
	 * amount of users or cities for example depending fromthe calculation*/
	long value_sum;
	
	/* verifies that user-name or city name is not empty.
	 * Could potentially have also some other checks. */
	virtual bool is_valid_name(std::string name);
	virtual float get_avg_value();
	virtual std::string get_avg_value_as_string(int decimal_count);
	/* for debugging purposes */
	virtual void printout_info();
	
	/**
	 * Converts the city name and average value related data to JSON object for single city.
	 *
	 * @return json object containing city name and average age of users
	 */
	virtual json to_json();
};

#endif // MY_LIBRARY_MY_CLASS_HPP
