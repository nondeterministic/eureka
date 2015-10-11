/*
 * shieldhelper.hh
 *
 *  Created on: Jun 22, 2014
 *      Author: baueran
 */

#ifndef SERVICESHELPER_HH_
#define SERVICESHELPER_HH_

#include <string>
#include "service.hh"

class ServicesHelper
{
public:
  ServicesHelper();
  virtual ~ServicesHelper();
  static Service* createFromLua(std::string);
  static bool exists(std::string);
  static void apply(Service*, int);
};

#endif
