/*
 * shieldhelper.hh
 *
 *  Created on: Jun 22, 2014
 *      Author: baueran
 */

#ifndef MISCHELPER_HH_
#define MISCHELPER_HH_

#include <string>

#include "miscitem.hh"
#include "mapobj.hh"

class MiscHelper
{
public:
  MiscHelper();
  virtual ~MiscHelper();
  static MiscItem* createFromLua(std::string, MapObj* = NULL);
  static bool exists(std::string);
  //  static void apply(MiscItem*, int);
};

#endif
