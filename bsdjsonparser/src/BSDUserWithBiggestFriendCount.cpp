/*
 * BSDUserWithBiggestFriendCount.hpp
 *
 *  Created on: Feb 21, 2025
 *      Author: lamikr
 */

#include "BSDUserWithBiggestFriendCount.hpp"

/**
 * Converts the city name and average value related data to JSON object for single city.
 *
 * @return json object containing city name and average age of users
 */
json BSDUserWithBiggestFriendCount::to_json() {
	json ret = {{"city", city_name}, {json_value_field_name, std::to_string(value_sum)}, {"users", json_user_arr}};
	return ret;
}

json BSDUserWithBiggestFriendCount::create_user_json(long id, std::string name) {
	json ret;

	ret["id"] = id;
	ret["name"] = name;
	return ret;
}

/**
 * Parses the brightsign root level JSON data to find the average age of users in city.
 *
 * @param js is the rootlevel json structure returned by the brightsign REST-API
 */
json BSDUserWithBiggestFriendCount::get_results(json js) {
	std::unordered_map<std::string, BSDUserWithBiggestFriendCount> map_param;
	json ret;

	for (std::size_t ii = 0; ii < js.size(); ii++) {
		std::string city = js[ii]["city"];
		long user_id =  js[ii]["id"];
		std::string user_name = js[ii]["name"];

		json child_array = js[ii]["friends"];
		long friend_cnt = 0;

		if (child_array.is_array()) {
			friend_cnt = child_array.size();
		}
		if(map_param.count(city) == 1) {
			BSDUserWithBiggestFriendCount &ubc = map_param[city];
			if (friend_cnt > ubc.value_sum) {
				// reset previously stored user data becaue
				// this user has more friends than the one stored previously
				ubc.value_sum = friend_cnt;
				ubc.json_user_arr.clear();
			}
			if (friend_cnt == ubc.value_sum) {
				json user_json = create_user_json(user_id, user_name);
				ubc.json_user_arr.push_back(user_json);
			}
			map_param[city] = ubc;
		}
		else{
			// data for this city is not yet in the map
			BSDUserWithBiggestFriendCount ubc;
			ubc.value_sum = friend_cnt;
			ubc.user_count = 1;
			ubc.city_name = city;
			json user_json = create_user_json(user_id, user_name);
			ubc.json_user_arr.push_back(user_json);
			map_param[city] = ubc;
		}
	}
	//ret["name"] = "UsersWithMostFriendsByCity";
	//ret["description"] = "users having most friends by city";
	json data_array = json::array();
	for (const auto& pair : map_param) {
		BSDUserWithBiggestFriendCount ubc = pair.second;
		data_array.push_back(ubc.to_json());
		//	ubc.printout_info();
		//std::cout << "city: " << pair.first << ", Value: " << pair.second << std::endl;
	}
	//ret["most_friends_by_city"] = data_array;
	return data_array;
}
