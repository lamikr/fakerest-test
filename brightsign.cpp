#include <iostream>
#include <string>
#include <nlohmann/json.hpp> // For JSON handling (install nlohmann/json)
#include <cpr/cpr.h>

using json = nlohmann::json;

class LoggingInterceptor : public cpr::Interceptor {
	private:
		std::ostream&	m_out;

	public:
		LoggingInterceptor(std::ostream &out_stream) : m_out(out_stream), cpr::Interceptor()
		{
		}

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

class UsersByCity {
	public:
		std::string city_name;
		long user_count;
		long age_sum;

		long get_average_age() {
			return (long)(age_sum / user_count);
		}

		void parse_users_by_city(json js) {
			std::unordered_map<int, UsersByCity> myMap;

			for (auto it = js.begin(); it != js.end(); ++it) {
				std::cout << "Key: " << it.key() << ", Value: " << it.value() << std::endl;
			}
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
				/*
				std::stringstream buffer;
				buffer << file.rdbuf();
				std::string json_str;
				json_str.append(buffer.str());
				ret.parse(json_str);
				*/
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

		void print_user_ages_by_city() {

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
    		std::cout << "json size: " << js.size() << std::endl;
    		UsersByCity users_by_city;

    		users_by_city.parse_users_by_city(js);
    	}
    	else {
    		std::cout << "json is empty" << std::endl;
    	}
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return 0;
}
