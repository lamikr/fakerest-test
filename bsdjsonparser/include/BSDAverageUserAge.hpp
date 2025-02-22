/*
 * BSDAverageUserAge.hpp
 *
 *  Created on: Feb 21, 2025
 *      Author: lamikr
 */

#ifndef BSDAVERAGEUSERAGE_HPP_
#define BSDAVERAGEUSERAGE_HPP_

#include "BSDJSONParserBase.hpp"

class BSDAverageUserAge : public BSDJSONParserBase {
public:
	BSDAverageUserAge() : BSDJSONParserBase("age") {}
	json get_results(json js);
};

#endif /* BSDAVERAGEUSERAGE_HPP_ */
