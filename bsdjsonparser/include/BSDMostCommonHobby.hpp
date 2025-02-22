/*
 * BSDJSONParsers.hpp
 *
 *  Created on: Feb 21, 2025
 *      Author: lamikr
 */

#ifndef BSDMOSTCOMMONHOBBY_HPP_
#define BSDMOSTCOMMONHOBBY_HPP_

#include "BSDJSONParserBase.hpp"

class BSDMostCommonHobby : BSDJSONParserBase {
public:
	json json_first_name_arr;

	BSDMostCommonHobby() : BSDJSONParserBase("") {}
	json to_json();
	json get_results(json js);
};

#endif /* BSDMOSTCOMMONHOBBY_HPP_ */
