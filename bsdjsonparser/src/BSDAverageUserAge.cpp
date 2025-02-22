/*
 * BSDAverageUserAge.hpp
 *
 *  Created on: Feb 21, 2025
 *      Author: lamikr
 */

#include "BSDAverageUserAge.hpp"

using json = nlohmann::json;

/**
 * Parses the brightsign root level JSON data to find the average age of users in city.
 *
 * @param js is the rootlevel json structure returned by the brightsign REST-API
 */
json BSDAverageUserAge::get_results(json js) {
	std::unordered_map<std::string, BSDAverageUserAge> map_param;
	json ret;

	for (std::size_t ii = 0; ii < js.size(); ii++) {
		long age = js[ii]["age"];
		std::string city = js[ii]["city"];
		//std::cout << "city: " << city << std::endl;

		if(map_param.count(city) == 1) {
			BSDAverageUserAge &ubc = map_param[city];
			ubc.value_sum = ubc.value_sum + age;
			ubc.user_count = ubc.user_count + 1;
			map_param[city] = ubc;
		}
		else{
			BSDAverageUserAge ubc;
			ubc.value_sum = age;
			ubc.user_count = 1;
			ubc.city_name = city;
			map_param[city] = ubc;
		}
		//InsertOrAssign
	}
	//ret["name"] = "AverageUserAgeByCity";
	//ret["description"] = "users average age per city";
	json data_array = json::array();
	for (const auto& pair : map_param) {
		BSDAverageUserAge ubc = pair.second;
		data_array.push_back(ubc.to_json());
		//	ubc.printout_info();
		//std::cout << "city: " << pair.first << ", Value: " << pair.second << std::endl;
	}
	//ret["users_average_age_by_city"] = data_array;
	//std::string json_string = ret.dump();
	//std::cout << json_string << std::endl;
	return data_array;
}
