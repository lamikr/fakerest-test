/*
 * RestAPIHandler.hpp
 *
 *  Created on: Feb 21, 2025
 *      Author: lamikr
 */

#ifndef RESTAPIHANDLER_HPP_
#define RESTAPIHANDLER_HPP_

#include <iostream>
#include <string>
#include <nlohmann/json.hpp> // For JSON handling (install nlohmann/json)
#include <cpr/cpr.h>

using json = nlohmann::json;

class RestAPIHandler {
private:
	bool m_verbose_log = false;
	std::string get_log_fname();
	std::ofstream log_stream;
public:
    /**
     * @param verbose_log specifies whether requests and responses are saved to log file.
     */
	RestAPIHandler(bool verbose_log);
	/**
	 * test method to allow reading the json data from local file
	 * 
	 * @param fname for JSON file
	 */
	json get_json_data_from_file(std::string fname);
	/**
	 * Connect to REST-API endpoint to receive data in JSON format
	 * by using the GET method.
	 * 
	 * @param url_full REST-API server endpoint url.
	 */
	json get_json_data(std::string url_full);
};

#endif /* RESTAPIHANDLER_HPP_ */
