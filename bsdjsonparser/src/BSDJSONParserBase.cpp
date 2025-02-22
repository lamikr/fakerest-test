#include "BSDJSONParserBase.hpp"

BSDJSONParserBase::BSDJSONParserBase(std::string json_val_field_name) {
	json_value_field_name	= json_val_field_name;
	value_sum				= 0;
	user_count				= 0;
}

bool BSDJSONParserBase::is_valid_name(std::string name) {
	return !name.empty();
}

float BSDJSONParserBase::get_avg_value() {
	float ret = 0;

	if (user_count != 0) {
		ret = (float)value_sum / (float)user_count;
	}
	return ret;
}

std::string BSDJSONParserBase::get_avg_value_as_string(int decimal_count) {
	float value;
	std::stringstream ss;

	value = get_avg_value();
	ss << std::fixed << std::setprecision(decimal_count) << value;
	return ss.str();
}

void BSDJSONParserBase::printout_info() {
	std::cout << "city: " << city_name << ": " << get_avg_value() << std::endl;
}

/**
 * Converts the city name and average value related data to JSON object for single city.
 *
 * @return json object containing city name and average age of users
 */
json BSDJSONParserBase::to_json() {
	json ret = {{"city", city_name}, {json_value_field_name, get_avg_value_as_string(CONST_DECIMAL_COUNT)}};
	return ret;
}

int BSDJSONParserBase::getValue() const {
    return value_sum;
}
