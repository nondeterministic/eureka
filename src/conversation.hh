// ----------------------------------------------------------------
// leibniz
//
// Copyright (c) 2009  Andreas Bauer <baueran@gmail.com>
//
// This is not free software.  See COPYING for further information.
// ----------------------------------------------------------------

#ifndef __CONVERSATION_HH
#define __CONVERSATION_HH

#include "mapobj.hh"
#include <string>

class Conversation
{
private:
	MapObj& obj;
	void printcon(std::string, bool = false);

public:
	Conversation(MapObj& mo);
	void initiate();
};

#endif
