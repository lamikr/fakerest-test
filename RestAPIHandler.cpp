/*
 * RestAPIHandler.cpp
 *
 *  Created on: Feb 21, 2025
 *      Author: lamikr
 */

#include "RestAPIHandler.hpp"

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
		m_out.flush();

		// Proceed the request and save the response
		cpr::Response response = proceed(session);

		m_out << "Response Status Code: " << response.status_code << std::endl;
		m_out << "Response Header: " << std::endl;
		for (const auto& header : response.header) {
			m_out << "  " << header.first << ": " << header.second << std::endl;
		}
		m_out << "Response Body: " << response.text << std::endl;
		m_out.flush();

		// Return the stored response
		return response;
	}
};

std::string RestAPIHandler::get_log_fname() {
	std::time_t t = std::time(nullptr);
	std::tm now_tm = *std::localtime(&t);

	std::stringstream ss;
	ss << std::put_time(&now_tm, "connection_data_%Y%m%d_%H%M%S.log");
	return ss.str();
}

RestAPIHandler::RestAPIHandler(bool verbose_log) {
	m_verbose_log = verbose_log;
	if (m_verbose_log) {
		log_stream.open("./" + get_log_fname());
	}
}

json RestAPIHandler::get_json_data_from_file(std::string fname) {
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

json RestAPIHandler::get_json_data(std::string url_full) {
	json ret;

	try {
		cpr::Session cpr_session;
		cpr::Url cpr_url = cpr::Url{url_full};
		cpr_session.SetUrl(cpr_url);
		if (m_verbose_log) {
			cpr_session.AddInterceptor(std::make_shared<LoggingInterceptor>(log_stream));
			cpr_session.SetOption(cpr::Verbose{true});
		}
		else {
			cpr_session.SetOption(cpr::Verbose{false});
		}
		cpr::Response response = cpr_session.Get();
		if (response.status_code == 200) {
			try {
				ret = json::parse(response.text);
			}
			catch (json::parse_error& e) {
				std::cerr << "Failed to parse JSON data." << std::endl;
				std::cerr << e.what() << std::endl;
				if (m_verbose_log == true) {
					std::cerr << response.text << std::endl; // Important for debugging
				}
			}
		}
		else {
			std::cerr << "Failed to connect to REST-API server: " << url_full;
			std::cerr << response.status_code << " - " << response.text << std::endl;
		}
	}
	catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
	}
	return ret;
}
