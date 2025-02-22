#include <iostream>
#include <string>
#include <nlohmann/json.hpp> // For JSON handling (install nlohmann/json)
#include <cpr/cpr.h>

using json = nlohmann::json;

#define CONST_DECIMAL_COUNT 2

/*
 * LoggingInterceptor can be used to printout/save the rest-api request and responses to log file.
 */
class LoggingInterceptor : public cpr::Interceptor {
private:
	std::ostream&	m_out;

public:
	/**
	 * Constructor for LoggingInterceptor
	 *
	 * @param out_sream Stream where the log files are directed. Can be for example a file
	 */
	LoggingInterceptor(std::ostream &out_stream) : m_out(out_stream), cpr::Interceptor()
	{
	}

	/**
	 * intercept is callbck method in in libcpr that will be invoked automatically
	 * before the data is send to server if the session has added an
	 * interceptor by calling a function AddInterceptor.
	 *
	 * @param session, libcpr session that will handle the REST API request and response.
	 */
	cpr::Response intercept(cpr::Session& session) override {
		// Log the request URL
		m_out << "Request url: " << session.GetFullRequestUrl() << std::endl;
		m_out << "Request header: " << session.GetFullRequestUrl() << std::endl;

		// Proceed the request and save the response
		cpr::Response response = proceed(session);

		m_out << "Response Status Code: " << response.status_code << std::endl;
		m_out << "Response Header: " << std::endl;
		for (const auto& header : response.header) {
			m_out << "  " << header.first << ": " << header.second << std::endl;
		}
		m_out << "Response Body: " << response.text << std::endl;

		// Return the stored response
		return response;
	}
};

class BSDJSONParserBase {
public:
	BSDJSONParserBase(std::string json_val_field_name) {
		json_value_field_name	= json_val_field_name;
	}
protected:
	std::string json_value_field_name;
	std::string city_name;
	long user_count;
	long value_sum;

	bool is_valid_name(std::string name) {
		return !name.empty();
	}

	virtual float get_avg_value() {
		float ret = 0;

		if (user_count != 0) {
			ret = (float)value_sum / (float)user_count;
		}
		return ret;
	}

	std::string get_avg_value_as_string(int decimal_count) {
		float value;
	    std::stringstream ss;

	    value = get_avg_value();
	    ss << std::fixed << std::setprecision(decimal_count) << value;
	    return ss.str();
	}

	void printout_info() {
		std::cout << "city: " << city_name << ": " << get_avg_value() << std::endl;
	}

	/**
	 * Converts the city name and average value related data to JSON object for single city.
	 *
	 * @return json object containing city name and average age of users
	 */
	json to_json() {
		json ret = {{"city", city_name}, {json_value_field_name, get_avg_value_as_string(CONST_DECIMAL_COUNT)}};
		return ret;
	}
};

class BSDAverageUserAge : public BSDJSONParserBase {
public:
	BSDAverageUserAge() : BSDJSONParserBase("avg") {

	}
	/**
	 * Parses the brightsign root level JSON data to find the average age of users in city.
	 *
	 * @param js is the rootlevel json structure returned by the brightsign REST-API
	 */
	json get_results(json js) {
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
		ret["users_average_age_by_city"] = data_array;
		//std::string json_string = ret.dump();
		//std::cout << json_string << std::endl;
		return ret;
	}
};

class BSDAverageFriendCount : public BSDJSONParserBase {
public:
	BSDAverageFriendCount() : BSDJSONParserBase("avg") {
	}
	/**
	 * Parses the brightsign root level JSON data to find the average age of users in city.
	 *
	 * @param js is the rootlevel json structure returned by the brightsign REST-API
	 */
	json get_results(json js) {
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
		ret["users_average_friend_count_by_city"] = data_array;
		return ret;
	}
};

class BSDUserWithBiggestFriendCount : BSDJSONParserBase {
public:
	std::string city_name;
	json json_user_arr;

	BSDUserWithBiggestFriendCount() : BSDJSONParserBase("friend_count") {
	}

	/**
	 * Converts the city name and average value related data to JSON object for single city.
	 *
	 * @return json object containing city name and average age of users
	 */
	json to_json() {
		json ret = {{"city", city_name}, {json_value_field_name, std::to_string(value_sum)}, {"users", json_user_arr}};
		return ret;
	}

	json create_user_json(long id, std::string name) {
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
	json get_results(json js) {
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
		ret["most_friends_by_city"] = data_array;
		return ret;
	}
};

class BSDMostComonFirstName : BSDJSONParserBase {
public:
	json json_first_name_arr;

	BSDMostComonFirstName() : BSDJSONParserBase("") {
	}

	/**
	 * Converts the city name and average value related data to JSON object for single city.
	 *
	 * @return json object containing city name and average age of users
	 */
	json to_json() {
		json ret = {{"name", json_first_name_arr}};
		return ret;
	}

	/**
	 * Parses the brightsign root level JSON data to find the average age of users in city.
	 *
	 * @param js is the rootlevel json structure returned by the brightsign REST-API
	 */
	json get_results(json js) {
		std::map<std::string, int> map_value_cnt;
		int biggest_value_cnt;
		int cur_value_cnt;
		json ret;

		biggest_value_cnt = 0;
		for (std::size_t ii = 0; ii < js.size(); ii++) {
			std::string user_name = js[ii]["name"];
			if (is_valid_name(user_name)) {
				map_value_cnt[user_name]++;
				if (map_value_cnt[user_name] > biggest_value_cnt) {
					biggest_value_cnt = map_value_cnt[user_name];
				}
			}
			json riends_array = js[ii]["friends"];
			long friend_cnt = 0;

			if (riends_array.is_array()) {
				friend_cnt = riends_array.size();
				for (int jj = 0; jj < friend_cnt; jj++) {
					std::string user_name = riends_array[jj]["name"];
					if (is_valid_name(user_name)) {
						map_value_cnt[user_name]++;
						if (map_value_cnt[user_name] > biggest_value_cnt) {
							biggest_value_cnt = map_value_cnt[user_name];
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
		ret["most_common_first_name"] = data_array;
		return ret;
	}
};

class BSDMostCommonHobby : BSDJSONParserBase {
public:
	json json_first_name_arr;

	BSDMostCommonHobby() : BSDJSONParserBase("") {
	}

	/**
	 * Converts the city name and average value related data to JSON object for single city.
	 *
	 * @return json object containing city name and average age of users
	 */
	json to_json() {
		json ret = {{"name", json_first_name_arr}};
		return ret;
	}

	/**
	 * Parses the brightsign root level JSON data to find the average age of users in city.
	 *
	 * @param js is the rootlevel json structure returned by the brightsign REST-API
	 */
	json get_results(json js) {
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
		ret["most_common_hobby"] = data_array;
		return ret;
	}
};

class RestApiJsonFetcher {
public:
	std::string get_log_fname() {
		std::time_t t = std::time(nullptr);
		std::tm now_tm = *std::localtime(&t);

		std::stringstream ss;
		ss << std::put_time(&now_tm, "brightsign_test_%Y%m%d_%H_%M");
		return ss.str();
	}

	json get_all_data_from_file(std::string fname) {
		nlohmann::json ret;
		std::ifstream file(fname);

		if (file.is_open()) {
			file >> ret;
		}
		else {
			std::cerr << "Error opening file: " << fname << std::endl;
		}
		return ret;
	}

	json get_all_data(std::string url_full) {
		json ret;

		try {
			cpr::Session cpr_session;
			// 1. Make a GET request.  Replace with your API endpoint.
			cpr::Url cpr_url = cpr::Url{url_full};
			cpr_session.SetUrl(cpr_url);
			std::ofstream log_stream("/home/lamikr/own/src/job_applications/brightsign/json/" + get_log_fname());
			//cpr_session.AddInterceptor(std::make_shared<LoggingInterceptor>(std::cout));
			cpr_session.AddInterceptor(std::make_shared<LoggingInterceptor>(log_stream));

			cpr_session.SetOption(cpr::Verbose{true});
			cpr::Response response = cpr_session.Get(); // Example: http://jsonplaceholder.typicode.com/todos/1

			if (response.status_code == 200) {
				try {
					//std::cout << "response.text:" << std::endl;
					//std::cout << response.text << std::endl;
					ret = json::parse(response.text);
				}
				catch (json::parse_error& e) {
					std::cerr << "JSON Parse Error: " << e.what() << std::endl;
					std::cerr << "Response Text: " << response.text << std::endl; // Important for debugging
				}
			}
			else {
				std::cerr << "HTTP Error: " << response.status_code << " - " << response.text << std::endl;
			}
		}
		catch (const std::exception& e) {
			std::cerr << "Error: " << e.what() << std::endl;
		}
		return ret;
	}
};

int main() {
    try {
    	const std::string m_url_base = "http://test.brightsign.io:3000";
    	const std::string m_url_tail_items = "";
    	std::string url_full = m_url_base + m_url_tail_items;

    	RestApiJsonFetcher brightsign = RestApiJsonFetcher();
    	//json js = brightsign.get_all_data(url_full);
    	json js = brightsign.get_all_data_from_file("/home/lamikr/own/src/job_applications/brightsign/json/brightsign.json");
    	if (js.empty() != true) {
    		//std::cout << "JSON size: " << js.size() << std::endl;
    		BSDAverageUserAge users_by_city;
    		BSDAverageFriendCount friens_by_city;
    		BSDUserWithBiggestFriendCount users_with_most_friends;
    		BSDMostComonFirstName most_common_first_names;
    		BSDMostCommonHobby most_common_hobby;
    		json res_json;

    		res_json = users_by_city.get_results(js);
    		std::cout << res_json.dump() << std::endl;
    		res_json = friens_by_city.get_results(js);
    		std::cout << res_json.dump() << std::endl;
    		res_json = users_with_most_friends.get_results(js);
    		std::cout << res_json.dump() << std::endl;
    		res_json = most_common_first_names.get_results(js);
    		std::cout << res_json.dump() << std::endl;
    		res_json = most_common_hobby.get_results(js);
    		std::cout << res_json.dump() << std::endl;
    	}
    	else {
    		std::cout << "JSON is empty" << std::endl;
    	}
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return 0;
}
