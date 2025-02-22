/*
 * BSDJSONParsers.hpp
 *
 *  Created on: Feb 21, 2025
 *      Author: lamikr
 */

#include "BSDMostCommonHobby.hpp"

/**
 * Converts the city name and average value related data to JSON object for single city.
 *
 * @return json object containing city name and average age of users
 */
json BSDMostCommonHobby::to_json() {
	json ret = {{"name", json_first_name_arr}};
	return ret;
}

/**
 * Parses the brightsign root level JSON data to find the average age of users in city.
 *
 * @param js is the rootlevel json structure returned by the brightsign REST-API
 */
json BSDMostCommonHobby::get_results(json js) {
	std::map<std::string, int> map_value_cnt;
	int biggest_value_cnt;
	int cur_value_cnt;
	json ret;

	biggest_value_cnt = 0;
	for (std::size_t ii = 0; ii < js.size(); ii++) {
		std::string hobby_name;
		json friend_array = js[ii]["friends"];
		long friend_cnt = 0;
		long hobby_cnt = 0;

		if (friend_array.is_array()) {
			friend_cnt = friend_array.size();
			for (int jj = 0; jj < friend_cnt; jj++) {
				json hobby_array = friend_array[jj]["hobbies"];
				if (hobby_array.is_array()) {
					for (int kk = 0; kk < hobby_array.size(); kk++) {
						std::string hobby_name = hobby_array[kk];
						map_value_cnt[hobby_name]++;
						if (map_value_cnt[hobby_name] > biggest_value_cnt) {
							biggest_value_cnt = map_value_cnt[hobby_name];
						}
					}
				}
			}
		}
	}
	ret["count"] = std::to_string(biggest_value_cnt);
	json data_array = json::array();
	// Iterate through the map using a range-based for loop (C++11 and later)
	for (const auto& pair : map_value_cnt) {
		if (pair.second == biggest_value_cnt) {
			data_array.push_back(pair.first);
		}
		//std::cout << pair.first << ": " << std::to_string(pair.second) << std::endl;
	}
	//
	//ret["most_common_hobby"] = data_array;
	return data_array;
}
