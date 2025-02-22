/*
 * BSDUserWithBiggestFriendCount.hpp
 *
 *  Created on: Feb 21, 2025
 *      Author: lamikr
 */

#ifndef BSDUSERWITHBIGGESTFRIENDCOUNT_HPP_
#define BSDUSERWITHBIGGESTFRIENDCOUNT_HPP_

#include "BSDJSONParserBase.hpp"

class BSDUserWithBiggestFriendCount : BSDJSONParserBase {
public:
	json json_user_arr;

	BSDUserWithBiggestFriendCount() : BSDJSONParserBase("friend_count") {}

	/**
	 * Converts the city name and average value related data to JSON object for single city.
	 *
	 * @return json object containing city name and average age of users
	 */
	json to_json();
	json get_results(json js);
private:
	json create_user_json(long id, std::string name);
};

#endif /* BSDUSERWITHBIGGESTFRIENDCOUNT_HPP_ */
