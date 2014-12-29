#include "simplicissimus.hh"
#include "party.hh"

#include <utility>
#include <iostream>
#include <memory>

#include <libxml++/libxml++.h>

void Party::set_coords(int x, int y)
{
  this->x = x; this->y = y;
}

void Party::set_coords(std::pair<int, int> coords)
{
  this->x = coords.first; this->y = coords.second;
}

std::pair<int, int> Party::get_coords()
{
  std::pair<int, int> coords;
  coords.first = this->x; coords.second = this->y;
  return coords;
}

Party::Party()
{
  x = 0; y = 0;
  _gold = 0;
  _food = 0;
}

Party& Party::Instance()
{
  static Party inst;
  return inst;
}

int Party::gold()
{
  return _gold;
}

void Party::set_gold(int g)
{
  _gold = g;
}

int Party::food()
{
  return _food;
}

void Party::set_food(int g)
{
  _food = g;
}

void Party::store_state()
{
  prev_x = x;
  prev_y = y;
  prev_indoors = _indoors;
  prev_map_name = _map_name;
}

void Party::restore_state()
{
  x = prev_x;
  y = prev_y;
  _indoors = prev_indoors;
  _map_name = prev_map_name;
}

bool Party::indoors()
{
  return _indoors;
}

void Party::set_indoors(bool mode)
{
  _indoors = mode;
}

void Party::set_map_name(const char* name)
{
  _map_name = name;
}

std::string Party::map_name()
{
  return _map_name;
}

void Party::add_player(PlayerCharacter player)
{
  _players.push_back(player);
  std::cout << "Added player " << player.name() << " to rooster.\n";
}

PlayerCharacter* Party::get_player(int number)
{
  int i = 0;
  for (auto player = Party::Instance().party_begin(); player != Party::Instance().party_end(); player++, i++)
    if (i == number)
      return &(*player);
  return NULL;
}

PlayerCharacter* Party::get_player(std::string name)
{
  for (auto player = Party::Instance().party_begin(); player != Party::Instance().party_end(); player++)
    if (player->name() == name)
      return &(*player);
  return NULL;
}

// Returns the number of alive party members
unsigned Party::party_alive()
{
  unsigned i = 0;
  for (auto player = Party::Instance().party_begin(); player != Party::Instance().party_end(); player++)
    if (player->hp() > 0)
      i++;
  return i;
}

/**
 * Convert party to XML document so that it can be saved to disk for save-game.
 *
 * TODO: It seems I cannot return a node as it somehow auto-deletes itself,
 * even if used within a shared_ptr. So I return a string which then needs to
 * be parsed again, alas.
 */

std::string Party::to_xml()
{
	xmlpp::Document xml_doc;
	xmlpp::Element* partyNd = xml_doc.create_root_node("party");

	// Add general party stuff
	partyNd->add_child("x")->add_child_text(std::to_string(x));
	partyNd->add_child("y")->add_child_text(std::to_string(y));
	partyNd->add_child("map")->add_child_text(map_name());
	partyNd->add_child("indoors")->add_child_text(indoors()? "1":"0");
	partyNd->add_child("gold")->add_child_text(std::to_string(gold()));
	partyNd->add_child("food")->add_child_text(std::to_string(food()));

	// Add players
	xmlpp::Element* playersEl = partyNd->add_child("players");
	for (auto &player: _players) {
		xmlpp::Element* playerEl(playersEl->add_child("player"));
		playerEl->set_attribute("name", player.name());

		playerEl->add_child("profession")->add_child_text(professionToString.at(player.profession()));
		playerEl->add_child("ep")->add_child_text(std::to_string(player.ep()));
		playerEl->add_child("hp")->add_child_text(std::to_string(player.hp()));
		playerEl->add_child("hpm")->add_child_text(std::to_string(player.hpm()));
		playerEl->add_child("sp")->add_child_text(std::to_string(player.sp()));
		playerEl->add_child("spm")->add_child_text(std::to_string(player.spm()));
		playerEl->add_child("str")->add_child_text(std::to_string(player.str()));
		playerEl->add_child("luck")->add_child_text(std::to_string(player.luck()));
		playerEl->add_child("dxt")->add_child_text(std::to_string(player.dxt()));
		playerEl->add_child("wis")->add_child_text(std::to_string(player.wis()));
		playerEl->add_child("charr")->add_child_text(std::to_string(player.charr()));
		playerEl->add_child("iq")->add_child_text(std::to_string(player.iq()));
		playerEl->add_child("end")->add_child_text(std::to_string(player.end()));
		playerEl->add_child("sex")->add_child_text(player.sex()? "1":"0");
		playerEl->add_child("race")->add_child_text(std::to_string(player.race()));
		playerEl->add_child("level")->add_child_text(std::to_string(player.level()));

		if (player.weapon() != NULL)
			playerEl->add_child("weapon")->add_child_text(player.weapon()->luaName());
		else
			playerEl->add_child("weapon");

		if (player.shield() != NULL)
			playerEl->add_child("shield")->add_child_text(player.shield()->luaName());
		else
			playerEl->add_child("shield");
	}

	// Add inventory
	xmlpp::Element* invEl = partyNd->add_child("inventory");
	for (unsigned i = 0; i < Party::Instance().inventory()->size(); i++) {
		Item* item = Party::Instance().inventory()->get_item(i);
		xmlpp::Element* itemEl(invEl->add_child("item"));

		itemEl->set_attribute("how_many", std::to_string(Party::Instance().inventory()->how_many_at(i)));
		itemEl->add_child_text(item->luaName());
	}

	return xml_doc.write_to_string_formatted().c_str();
}

std::vector<PlayerCharacter>::iterator Party::party_begin()
{
  return _players.begin();
}

std::vector<PlayerCharacter>::iterator Party::party_end()
{
  return _players.end();
}

int Party::party_size()
{
  return _players.size();
}

Inventory* Party::inventory()
{
  return &_inv;
}
