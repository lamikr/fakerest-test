/*
 * BSDAverageFriendCount.hpp
 *
 *  Created on: Feb 21, 2025
 *      Author: lamikr
 */

#ifndef BSDAVERAGEFRIENDCOUNT_HPP_
#define BSDAVERAGEFRIENDCOUNT_HPP_

#include "BSDJSONParserBase.hpp"

class BSDAverageFriendCount : public BSDJSONParserBase {
public:
	BSDAverageFriendCount() : BSDJSONParserBase("avg_friends") {}
	json get_results(json js);
};

#endif /* BSDAVERAGEFRIENDCOUNT_HPP_ */
