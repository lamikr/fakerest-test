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
	bool polling_enabled = false;
    try {
		// defines which json calculations to exec, default mask is 0b11111
		int exec_mask = 0x1f;
    	std::string m_url_base; // = CONST_DEFAULT_REST_API_URL;

		if (argc > 1) { // Check if there are any arguments beyond the program name    	
			if ((argc == 2) && (strcmp(argv[1], "--help") == 0)) {
				std::cout << "Usage: program_name [options] <connection url>" << std::endl;
				std::cout << "Options:" << std::endl;
				std::cout << "  --verbose,     Show verbose log and create debug file" << std::endl;
				std::cout << "  --poll,        poll data every 60 seconds" << std::endl;
				std::cout << "  --mask,        1 - 31, bitmask which tasks to run" << std::endl;
				std::cout << "  --help         Show this help message" << std::endl;
				return 0;
			}
			int ii = 1;
			while (ii < argc) {
				std::string arg = std::string(argv[ii]);
				
				// Process each argument, e.g., check for flags or extract values
				if (arg == "--mask") {
					// Handle the mask flag
					ii++;
					if (ii << argc) {
						try {
							std::string str_val = argv[ii];
							exec_mask = std::stoi(str_val);
							// print the exec mask in binary representation
							const int numBits = 5;
							std::bitset<numBits> binary(exec_mask);
							if (verbose_log == true) {
								std::cout << "exec_mask " << exec_mask << ", binary: " << binary.to_string() << std::endl;	
							}
						}
						catch (const std::invalid_argument& e) {
							std::cerr << "Invalid exec_mask argument, must be a number between 0-0x1f: " << e.what() << std::endl;
							return 2;
						}
						catch (const std::out_of_range& e) {
							std::cerr << "Invalid exec_mask argument, must be a number between 0-0x1f: " << e.what() << std::endl;
							return 3;
						}
					}
					else {
						std::cerr << "Error: Missing value for --mask argument." << std::endl;
					}
				}
				else if (arg == "--poll") {
					// Handle the mask flag
					polling_enabled = true;
				}
				else if (arg == "--verbose") {
					// Handle the mask flag
					verbose_log = true;
				}
				else {
					if (ii == (argc - 1)) {
						m_url_base = argv[ii];
						if (verbose_log == true) {
							std::cout << "connect url: " << m_url_base << std::endl;
						}
					}
					else {
						std::cerr << "Error: Missing value for -name argument." << std::endl;
					}
				}
				ii++;
			}
		}
		else {
			std::cout << "Error, no arguments provided." << std::endl;
			return 1;
		}
    	json js;
    	RestAPIHandler brightsign = RestAPIHandler(verbose_log);
    	
    	bool show_data;
    	
    	show_data = true;
    	while(show_data == true) {
			if (m_url_base.rfind("file://", 0) == 0) {
				// read data from testing puroses from the file instead of from the server
				std::string fname = m_url_base.substr(7);
				js = brightsign.fetch_from_file(fname);
			}
			else {
				js = brightsign.fetch_from_internet(m_url_base);
			}
			if (js.empty() != true) {
				json res_json;
				json full_json;
	  
				if (exec_mask & 0b00001) {
					BSDAverageUserAge users_by_city;
					res_json = users_by_city.get_results(js);
					full_json["user_avg_age_by_city"] = res_json;
					//std::cout << res_json.dump() << std::endl;
				}
				if (exec_mask & 0b00010) {
					BSDAverageFriendCount friens_by_city;
					res_json = friens_by_city.get_results(js);
					full_json["user_avg_friend_count_by_city"] = res_json;
					//std::cout << res_json.dump() << std::endl;
				}
				if (exec_mask & 0b00100) {
					BSDUserWithBiggestFriendCount users_with_most_friends;
					res_json = users_with_most_friends.get_results(js);
					full_json["most_friends_by_city"] = res_json;
					//std::cout << res_json.dump() << std::endl;
				}
				if (exec_mask & 0b01000) {
					BSDMostCommonFirstName most_common_first_names;
					res_json = most_common_first_names.get_results(js);
					full_json["most_common_first_name"] = res_json;
					//std::cout << res_json.dump() << std::endl;
				}
				if (exec_mask & 0b10000) {
					BSDMostCommonHobby most_common_hobby;    		
					res_json = most_common_hobby.get_results(js);
					full_json["most_common_hobby"] = res_json;
					//std::cout << res_json.dump() << std::endl;
				}
				std::cout << full_json.dump() << std::endl;
			}
			else {
				std::cerr << "Error, empty JSON received: " << m_url_base << std::endl;
				return 4;
			}
			show_data = polling_enabled;
			if (show_data == true) {
				std::this_thread::sleep_for(std::chrono::seconds(60));
			}
		}
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 5;
    }
    return 0;
}
