/*
 * BSDMostComonFirstName.hpp
 *
 *  Created on: Feb 21, 2025
 *      Author: lamikr
 */

#ifndef BSDMOSTCOMMONFIRSTNAME_HPP_
#define BSDMOSTCOMMONFIRSTNAME_HPP_

#include "BSDJSONParserBase.hpp"

class BSDMostCommonFirstName : BSDJSONParserBase {
public:
	json json_first_name_arr;

	BSDMostCommonFirstName() : BSDJSONParserBase("") {}
	json to_json();
	json get_results(json js);
};

#endif /* BSDMOSTCOMMONFIRSTNAME_HPP_ */
