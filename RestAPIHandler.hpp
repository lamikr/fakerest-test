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

class RestAPIHandler {
private:
	bool m_verbose_log = false;
	std::string get_log_fname() {
		std::time_t t = std::time(nullptr);
		std::tm now_tm = *std::localtime(&t);

		std::stringstream ss;
		ss << std::put_time(&now_tm, "brightsign_test_%Y%m%d_%H_%M");
		return ss.str();
	}

public:
	RestAPIHandler(bool verbose_log) {
		m_verbose_log = verbose_log;
	}

	json fetch_from_file(std::string fname) {
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

	json fetch_from_internet(std::string url_full) {
		json ret;

		try {
			cpr::Session cpr_session;
			// 1. Make a GET request.  Replace with your API endpoint.
			cpr::Url cpr_url = cpr::Url{url_full};
			cpr_session.SetUrl(cpr_url);
			std::ofstream log_stream("/home/lamikr/own/src/job_applications/brightsign/json/" + get_log_fname());
			//cpr_session.AddInterceptor(std::make_shared<LoggingInterceptor>(std::cout));
			if (m_verbose_log) {
				cpr_session.AddInterceptor(std::make_shared<LoggingInterceptor>(log_stream));
			}

			cpr_session.SetOption(cpr::Verbose{true});
			cpr::Response response = cpr_session.Get();

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

#endif /* RESTAPIHANDLER_HPP_ */
