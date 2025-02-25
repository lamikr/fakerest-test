#include <iostream>
#include <string>
#include <bitset>
#include <nlohmann/json.hpp> // For JSON handling (install nlohmann/json)
#include <cpr/cpr.h>

#include "BSDAverageFriendCount.hpp"
#include "BSDAverageUserAge.hpp"
#include "BSDJSONParserBase.hpp"
#include "BSDMostCommonFirstName.hpp"
#include "BSDMostCommonHobby.hpp"
#include "BSDUserWithBiggestFriendCount.hpp"
#include "RestAPIHandler.hpp"

#include "config.h"

using json = nlohmann::json;

int main(int argc, char* argv[]) {
	bool verbose_log = false;
	bool fetch_and_process_data;
	
	int polling_interval = -1;
    try {
		// defines which json calculations to exec, default mask is 0b11111
		int exec_mask = 0x1f;
    	std::string m_url_base; // = CONST_DEFAULT_REST_API_URL;

		if (argc > 1) { // Check if there are any arguments beyond the program name    	
			if ((argc == 2) && (strcmp(argv[1], "--help") == 0)) {
				std::cout << "Usage: brightsign [options] <connection url>" << std::endl;
				std::cout << "Options:" << std::endl;
				std::cout << "  --verbose,        Show verbose message and create log file connection_data_<datetime>.log" << std::endl;
				std::cout << "  --poll <seconds>, Poll the data periodically. Specified poll interval must be a positive number." << std::endl;
				std::cout << "  --mask <bitmask>, Bitmask from 0 to 31 to specify which from 5 calculations to performed for the retrieved data." << std::endl;
				std::cout << "  --help            Show this help message" << std::endl;
				return 0;
			}
			int ii = 1;
			while (ii < argc) {
				std::string arg = std::string(argv[ii]);
				
				// process each argument, e.g., check for flags or extract values
				if (arg == "--mask") {
					// handle the mask flag
					ii++;
					if (ii << argc) {
						try {
							std::string str_val = argv[ii];
							exec_mask = std::stoi(str_val);
							if ((exec_mask < 0) || (exec_mask > 31)) {
								std::cerr << "Invalid argument error, --mask argument requires a number between 0 and 31 to be specified." << std::endl;
								return EINVAL;
							}
							// print the exec mask in binary representation
							const int numBits = 5;
							std::bitset<numBits> binary(exec_mask);
							if (verbose_log == true) {
								std::cout << "exec_mask " << exec_mask << ", binary: " << binary.to_string() << std::endl;	
							}
						}
						catch (const std::exception& e) {
							std::cerr << "Invalid argument error, --mask argument requires a number between 0 and 31 to be specified." << std::endl;
							return EINVAL;
						}
					}
					else {
						std::cerr << "Invalid argument error, --mask argument requires a number between 0 and 31 to be specified." << std::endl;
						return EINVAL;
					}
				}
				else if (arg == "--poll") {
					// handle the polling interval
					ii++;
					if (ii << argc) {
						try {
							std::string str_val = argv[ii];
							polling_interval = std::stoi(str_val);
							if (polling_interval <= 0) {
								std::cerr << "Invalid argument error, --poll argument requires a positive number to be specified." << std::endl;
								return EINVAL;
							}
						}
						catch (const std::exception& e) {
							std::cerr << "Invalid argument error, --poll argument requires a positive number to be specified." << std::endl;
							return EINVAL;
						}
					}
					else {
						std::cerr << "Invalid argument error, --poll argument requires a positive number to be specified." << std::endl;
						return EINVAL;
					}					
				}
				else if (arg == "--verbose") {
					// Handle the mask flag
					verbose_log = true;
				}
				else {
					if (ii == (argc - 1)) {
						m_url_base = argv[ii];
						if (verbose_log == true) {
							std::cout << "REST API Server connection URL: " << m_url_base << std::endl;
						}
					}
					else {
						std::cerr << "Invalid argument: " << argv[ii] << std::endl;
						return EINVAL;
					}
				}
				ii++;
			}
		}
		if (m_url_base.empty()) {
			std::cout << "Error, URL to REST-API server is mandatory parameter." << std::endl;
			return EINVAL;
		}
    	json js;
    	
    	// initilze class which will handle the rest-api communication
    	RestAPIHandler brightsign = RestAPIHandler(verbose_log);

		fetch_and_process_data = true;
		while(fetch_and_process_data == true) {
			try {
#if 0
				// allow testing with local file captured earlier and stored to tests directory
				if (m_url_base.rfind("file://", 0) == 0) {
					// read data from testing puroses from the file instead of from the server
					std::string fname = m_url_base.substr(7);
					js = brightsign.get_json_data_from_file(fname);
				}
				else {
					js = brightsign.get_json_data(m_url_base);
				}
#else
				// fetch from rest api server
				js = brightsign.get_json_data(m_url_base);
#endif
				if (js.empty() != true) {
					json res_json;
					json full_json;
		  
					if (exec_mask & 0b00001) {
						BSDAverageUserAge users_by_city;
						res_json = users_by_city.get_results(js);
						full_json["average_age_of_all_users_per_city"] = res_json;
						//std::cout << res_json.dump() << std::endl;
					}
					if (exec_mask & 0b00010) {
						BSDAverageFriendCount friens_by_city;
						res_json = friens_by_city.get_results(js);
						full_json["average_number_of_friends_per_city"] = res_json;
						//std::cout << res_json.dump() << std::endl;
					}
					if (exec_mask & 0b00100) {
						BSDUserWithBiggestFriendCount users_with_most_friends;
						res_json = users_with_most_friends.get_results(js);
						full_json["user_with_the_most_friends_per_city"] = res_json;
						//std::cout << res_json.dump() << std::endl;
					}
					if (exec_mask & 0b01000) {
						BSDMostCommonFirstName most_common_first_names;
						res_json = most_common_first_names.get_results(js);
						full_json["most_common_first_name_in_all cities"] = res_json;
						//std::cout << res_json.dump() << std::endl;
					}
					if (exec_mask & 0b10000) {
						BSDMostCommonHobby most_common_hobby;    		
						res_json = most_common_hobby.get_results(js);
						full_json["most_common_hobby_of_all_friends_of_users_in_all_cities"] = res_json;
						//std::cout << res_json.dump() << std::endl;
					}
					std::cout << full_json.dump() << std::endl;
				}
				else {
					std::cerr << "Failed to read JSON Data from the REST-API Server: " << m_url_base << std::endl;
				}
			}
			catch (const std::exception& e) {
				std::cerr << "Unexpected error1: " << e.what() << std::endl;
			}
			fetch_and_process_data = (polling_interval > 0);
			if (fetch_and_process_data == true) {
				std::this_thread::sleep_for(std::chrono::seconds(polling_interval));
			}
		}
    }
    catch (const std::exception& e) {
        std::cerr << "Unexpected error2: " << e.what() << std::endl;
        return EIO;
    }
    return 0;
}
