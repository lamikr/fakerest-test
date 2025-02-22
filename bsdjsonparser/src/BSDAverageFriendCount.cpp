/*
 * BSDAverageFriendCount.cpp
 *
 *  Created on: Feb 21, 2025
 *      Author: lamikr
 */

#include "BSDAverageFriendCount.hpp"

/**
 * Parses the brightsign root level JSON data to find the average age of users in city.
 *
 * @param js is the rootlevel json structure returned by the brightsign REST-API
 */
json BSDAverageFriendCount::get_results(json js) {
	std::unordered_map<std::string, BSDAverageFriendCount> map_param;
	json ret;

	for (std::size_t ii = 0; ii < js.size(); ii++) {
		std::string city = js[ii]["city"];
		json child_array = js[ii]["friends"];
		long friend_cnt = 0;

		if (child_array.is_array()) {
			friend_cnt = child_array.size();
		}
		if(map_param.count(city) == 1) {
			BSDAverageFriendCount &ubc = map_param[city];
			ubc.value_sum = ubc.value_sum + friend_cnt;
			ubc.user_count = ubc.user_count + 1;
			map_param[city] = ubc;
		}
		else{
			BSDAverageFriendCount ubc;
			ubc.value_sum = friend_cnt;
			ubc.user_count = 1;
			ubc.city_name = city;
			map_param[city] = ubc;
		}
	}
	//ret["name"] = "AverageFriendCountByCity";
	//ret["description"] = "users average friend count per city";
	json data_array = json::array();
	for (const auto& pair : map_param) {
		BSDAverageFriendCount ubc = pair.second;
		data_array.push_back(ubc.to_json());
		//	ubc.printout_info();
		//std::cout << "city: " << pair.first << ", Value: " << pair.second << std::endl;
	}
	//ret["average_users_friend_count_by_city"] = data_array;
	return data_array;
}
