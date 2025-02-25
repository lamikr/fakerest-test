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
#include "../RestAPIHandler.hpp"

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
				std::cout << "Usage: program_name json_file" << std::endl;
				std::cout << "Options:" << std::endl;
				std::cout << "  --poll <seconds>, Poll the data periodically. Specified poll interval must be a positive number." << std::endl;
				std::cout << "  --help            Show this help message" << std::endl;
				return 0;
			}
			int ii = 1;
			while (ii < argc) {
				std::string arg = std::string(argv[ii]);
				
				if (ii == (argc - 1)) {
					m_url_base = "file://";
					m_url_base.append(argv[ii]);
				}
				else {
					std::cerr << "Invalid argument: " << argv[ii] << std::endl;
					return EINVAL;
				}
				ii++;
			}
		}
		if (m_url_base.empty()) {
			std::cout << "Error, URL to REST-API server is mandatory parameter." << std::endl;
			return EINVAL;
		}
    	json js;
    	RestAPIHandler brightsign = RestAPIHandler(verbose_log);
    	    	
    	fetch_and_process_data = true;
    	while(fetch_and_process_data == true) {
			if (m_url_base.rfind("file://", 0) == 0) {
				// read data from testing puroses from the file instead of from the server
				std::string fname = m_url_base.substr(7);
				js = brightsign.get_json_data_from_file(fname);
			}
			else {
				js = brightsign.get_json_data(m_url_base);
			}
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
				return ENODATA;
			}
			fetch_and_process_data = (polling_interval > 0);
			if (fetch_and_process_data == true) {
				std::this_thread::sleep_for(std::chrono::seconds(polling_interval));
			}
		}
    }
    catch (const std::exception& e) {
        std::cerr << "Unexpected error: " << e.what() << std::endl;
        return EIO;
    }
    return 0;
}
