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

class BrightsignCityData {
public:
	BrightsignCityData(std::string json_val_field_name) {
		json_value_field_name	= json_val_field_name;
	}
protected:
	std::string json_value_field_name;
	std::string city_name;
	long user_count;
	long value_sum;

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
	 * This method converts the city name and average age related data to json object for single city.
	 *
	 * @return json object containing city name and average age of users
	 */
	json to_json() {
		json ret = {{"city", city_name}, {json_value_field_name, get_avg_value_as_string(CONST_DECIMAL_COUNT)}};
		return ret;
	}

};

class BrightsignCityAvgUserAge : public BrightsignCityData {
public:
	BrightsignCityAvgUserAge() : BrightsignCityData("avg") {

	}
	/**
	 * Parses the brightsign root level JSON data to find the average age of users in city.
	 *
	 * @param js is the rootlevel json structure returned by the brightsign REST-API
	 */
	json get_results(json js) {
		std::unordered_map<std::string, BrightsignCityAvgUserAge> map_param;
		json ret;

		for (std::size_t ii = 0; ii < js.size(); ii++) {
			long age = js[ii]["age"];
			std::string city = js[ii]["city"];
			//std::cout << "city: " << city << std::endl;

			if(map_param.count(city) == 1) {
				BrightsignCityAvgUserAge &ubc = map_param[city];
				ubc.value_sum = ubc.value_sum + age;
				ubc.user_count = ubc.user_count + 1;
				map_param[city] = ubc;
			}
			else{
				BrightsignCityAvgUserAge ubc;
				ubc.value_sum = age;
				ubc.user_count = 1;
				ubc.city_name = city;
				map_param[city] = ubc;
			}
			//InsertOrAssign
		}
		ret["name"] = "AverageUserAgeByCity";
		ret["description"] = "users average age per city";
		json data_array = json::array();
		for (const auto& pair : map_param) {
			BrightsignCityAvgUserAge ubc = pair.second;
			data_array.push_back(ubc.to_json());
			//	ubc.printout_info();
			//std::cout << "city: " << pair.first << ", Value: " << pair.second << std::endl;
		}
		ret["users_age_by_city"] = data_array;
		//std::string json_string = ret.dump();
		//std::cout << json_string << std::endl;
		return ret;
	}
};

class BrightsignCityAvgUserFriendCount : public BrightsignCityData {
public:
	BrightsignCityAvgUserFriendCount() : BrightsignCityData("avg") {
	}
	/**
	 * Parses the brightsign root level JSON data to find the average age of users in city.
	 *
	 * @param js is the rootlevel json structure returned by the brightsign REST-API
	 */
	json get_results(json js) {
		std::unordered_map<std::string, BrightsignCityAvgUserFriendCount> map_param;
		json ret;

		for (std::size_t ii = 0; ii < js.size(); ii++) {
			std::string city = js[ii]["city"];
			json child_array = js[ii]["friends"];
			long friend_cnt = 0;

			if (child_array.is_array()) {
				friend_cnt = child_array.size();
			}
			if(map_param.count(city) == 1) {
				BrightsignCityAvgUserFriendCount &ubc = map_param[city];
				ubc.value_sum = ubc.value_sum + friend_cnt;
				ubc.user_count = ubc.user_count + 1;
				map_param[city] = ubc;
			}
			else{
				BrightsignCityAvgUserFriendCount ubc;
				ubc.value_sum = friend_cnt;
				ubc.user_count = 1;
				ubc.city_name = city;
				map_param[city] = ubc;
			}
		}
		ret["name"] = "AverageFriendCountByCity";
		ret["description"] = "users average friend count per city";
		json data_array = json::array();
		for (const auto& pair : map_param) {
			BrightsignCityAvgUserFriendCount ubc = pair.second;
			data_array.push_back(ubc.to_json());
			//	ubc.printout_info();
			//std::cout << "city: " << pair.first << ", Value: " << pair.second << std::endl;
		}
		ret["users_friend_count_by_city"] = data_array;
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
			std::cerr << "Error opening file" << std::endl;
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

			// Example POST request (comment out GET if you want to test POST):
			/*
			json data;
			data["userId"] = 1;
			data["title"] = "My Todo";
			data["completed"] = false;

			cpr::Response response = cpr::Post(cpr::Url{"http://your-api-endpoint/todos"},
											cpr::Body{data.dump()},
											cpr::Header{{"Content-Type", "application/json"}});
			*/


			// 2. Check the response status code.
			if (response.status_code == 200) {
				// 3. Parse the JSON response.
				try {
					//std::cout << "response.text:" << std::endl;
					//std::cout << response.text << std::endl;
					ret = json::parse(response.text);

					// 4. Process the JSON data.
					//std::cout << j.dump(4) << std::endl;  // Pretty-print the JSON

					// Example accessing data (adapt to your JSON structure):
					/*
					if (j.contains("title")) {
						std::cout << "Title: " << j["title"] << std::endl;
					}
					*/

				} catch (json::parse_error& e) {
					std::cerr << "JSON Parse Error: " << e.what() << std::endl;
					std::cerr << "Response Text: " << response.text << std::endl; // Important for debugging
				}

			} else {
				std::cerr << "HTTP Error: " << response.status_code << " - " << response.text << std::endl;
			}

		} catch (const std::exception& e) {
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
    		std::cout << "JSON size: " << js.size() << std::endl;
    		BrightsignCityAvgUserAge users_by_city;
    		BrightsignCityAvgUserFriendCount friens_by_city;
    		json res_json;

    		res_json = users_by_city.get_results(js);
    		std::cout << res_json.dump() << std::endl;
    		res_json = friens_by_city.get_results(js);
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
