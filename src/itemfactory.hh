/*
 * itemfactory.hh
 *
 *  Created on: Jun 20, 2014
 *      Author: baueran
 */

#ifndef ITEMFACTORY_HH_
#define ITEMFACTORY_HH_

#include "item.hh"

class ItemFactory {
public:
	ItemFactory();
	virtual ~ItemFactory();
	static Item* create(std::string);
	static Item* create_plain_name(std::string);
};

#endif /* ITEMFACTORY_HH_ */
