#include "ztatswin.hh"
#include "sdlwindow.hh"
#include "party.hh"
#include "playercharacter.hh"
#include "sdltricks.hh"
#include "gamecontrol.hh"
#include "eventmanager.hh"
#include "simplicissimus.hh"

#include <boost/bind.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/lexical_cast.hpp>

#include <iostream>
#include <list>
#include <vector>
#include <sstream>

using namespace std;

ZtatsWin::ZtatsWin()
{
  standard_bgcolour.r = 0;
  standard_bgcolour.g = 0;
  standard_bgcolour.b = 0;

  highlight_colour.r = 50;
  highlight_colour.g = 50;
  highlight_colour.b = 250;

  set_surface(SDLWindow::Instance().get_ztats_SDL_surface());
  
  SDL_Rect rect;
  rect.x = SDLWindow::Instance().get_drawing_area_SDL_surface()->w + 2 * SDLWindow::Instance().frame_icon_size() - 6;
  rect.y = SDLWindow::Instance().frame_icon_size() - 2;
  rect.w = get_surface()->w;
  rect.h = get_surface()->h;
  set_position(rect);  
}

ZtatsWin& ZtatsWin::Instance()
{
  static ZtatsWin inst;
  return inst;
}

// Highlights lines from to to in the ztats window.  Players in the
// standard view, e.g., occupy two lines each.
//
// This function will be used to let players move the cursor up and
// down, selecting stuff from the ztats window.

void ZtatsWin::highlight_lines(int from_top, int to_bottom)
{
  swap_colours(from_top, to_bottom, standard_bgcolour, highlight_colour);
}

void ZtatsWin::unhighlight_lines(int from_top, int to_bottom)
{
  swap_colours(from_top, to_bottom, highlight_colour, standard_bgcolour);
}

void ZtatsWin::unhighlight_all()
{
  unhighlight_lines(-1, -1);
}

// Swaps background colour A for B in lines from_top to to_bottom in
// the ztats window.

void ZtatsWin::swap_colours(int from_top, int to_bottom, SDL_Color a, SDL_Colour b)
{
  SDL_Surface* surf = get_surface();

  // How many lines are displayed in the ztats window?
  int lines = surf->h / font.char_height();

  if (from_top < 0 || to_bottom + 1 > lines || to_bottom < 0)
    SDLTricks::Instance().replace_col(surf, a, b, NULL);
  else {
    SDL_Rect rect;
    rect.x = 0;
    rect.y = from_top * font.char_height() + _inter_line_padding * from_top + _y_frame_offset;
    rect.h = (to_bottom - from_top) * font.char_height() + _inter_line_padding;
    rect.w = surf->w;
    SDLTricks::Instance().replace_col(surf, a, b, &rect);
  }

  SDLWindow::Instance().blit_ztats();
}

int ZtatsWin::select_player()
{
  int player  =  0;
  SDL_Event event;

  highlight_lines(player*2, player*2 + 2);

  while (1) {
    if ( SDL_WaitEvent(&event) ) {
      if (event.type == SDL_KEYDOWN) {
        unhighlight_lines(player*2, player*2 + 2);

        switch(event.key.keysym.sym) {
        case SDLK_UP:
          if (player > 0)
            player--;
          break;
        case SDLK_DOWN:
          if (player < Party::Instance().party_size() - 1)
            player++;
          break;
        case SDLK_RETURN:
          return player;
        case SDLK_ESCAPE:
        case SDLK_q:
          // Reset UI flags to default
          update_player_list();
          SDLWindow::Instance().blit_interior();
          return -1;
        default:
          break;
        }
        highlight_lines(player*2, player*2 + 2);
        SDLWindow::Instance().blit_interior();
      }
    }
  }

  return -1;
}

// Helps select multiple items, see Combat::victory for an example.

std::vector<int> ZtatsWin::select_items()
{
	std::vector<int> result;
	int line = 0;
	SDL_Event event;
	unsigned offset = 0;
	const int dheight = 16;  // Ztats display is 16 lines tall

	for (int i = offset; i < (int)lines.size(); i++)
		println(i - offset, lines[i].get<0>(), lines[i].get<1>());

	highlight_lines(line, line + 1);

	while (1) {
		if (SDL_WaitEvent(&event)) {
			if (event.type == SDL_KEYDOWN) {
				if (event.key.keysym.sym == SDLK_DOWN) {
					if (line == dheight - 1 && offset < lines.size() - 1) {
						offset++;
						clear();
						for (int i = offset; i < (int)lines.size(); i++)
							println(i - offset, lines[i].get<0>(), lines[i].get<1>());
						highlight_lines(line, line + 1);
					}
					else if (line < dheight - 1 && line < (int)lines.size() - 1) {
						line++;
						clear();
						for (int i = offset; i < (int)lines.size(); i++)
							println(i - offset, lines[i].get<0>(), lines[i].get<1>());
						highlight_lines(line, line + 1);
					}
				}
				else if (event.key.keysym.sym == SDLK_UP) {
					if (line > 0) {
						line--;
						clear();
						for (int i = offset; i < (int)lines.size(); i++)
							println(i - offset, lines[i].get<0>(), lines[i].get<1>());
						highlight_lines(line, line + 1);
					}
					else if (line == 0 && offset > 0) {
						line--;
						offset--;
						clear();
						for (int i = offset; i < (int)lines.size(); i++)
							println(i - offset, lines[i].get<0>(), lines[i].get<1>());
						highlight_lines(line, line + 1);
					}
				}
				else if (event.key.keysym.sym == SDLK_ESCAPE || event.key.keysym.sym == SDLK_q) {
					ZtatsWin::Instance().update_player_list();
					return result;
				}
				else if (event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_SPACE) {
					if (std::find(result.begin(), result.end(), line + offset) == result.end()) {
						result.push_back(line + offset);

						line_tuple tuple     = lines[line + offset];
						std::string linestr  = tuple.get<0>();
						lines[line + offset] = line_tuple("*" + linestr.substr(linestr.find_first_of(")")),  // String
								tuple.get<1>());                                   // Alginment
					}
					else {
						result.erase(std::find(result.begin(), result.end(), line + offset));

						line_tuple tuple     = lines[line + offset];
						std::string linestr  = tuple.get<0>();
						lines[line + offset] = line_tuple(boost::lexical_cast<string>(line + offset + 1) + linestr.substr(linestr.find_first_of(")")),
								tuple.get<1>());
					}

					for (int i = offset; i < (int)lines.size(); i++)
						println(i - offset, lines[i].get<0>(), lines[i].get<1>());

					highlight_lines(line, line + 1);
				}
			}
		}
	}

	return result;
}

// Select single item.
// Returns -1 if no item was selected, otherwise the nth line that was selected.

int ZtatsWin::select_item()
{
	int line = 0;
	SDL_Event event;
	unsigned offset = 0;
	const int dheight = 16;  // Ztats display is 16 lines tall

	for (int i = offset; i < (int)lines.size(); i++)
		println(i - offset, lines[i].get<0>(), lines[i].get<1>());

	highlight_lines(line, line + 1);

	while (1)
	{
		if (SDL_WaitEvent(&event))
		{
			if (event.type == SDL_KEYDOWN)
			{
				if (event.key.keysym.sym == SDLK_DOWN
						&& offset + line + 1 < lines.size())  // +1 because we just pressed down again!
				{
					if (line == dheight - 1 && offset < lines.size() - 1) {
						offset++;
						clear();
						for (int i = offset; i < (int)lines.size(); i++)
							println(i - offset, lines[i].get<0>(), lines[i].get<1>());
						highlight_lines(line, line + 1);
					}
					else if (line < dheight - 1 && line < (int)lines.size() - 1) {
						line++;
						clear();
						for (int i = offset; i < (int)lines.size(); i++)
							println(i - offset, lines[i].get<0>(), lines[i].get<1>());
						highlight_lines(line, line + 1);
					}
				}
				else if (event.key.keysym.sym == SDLK_UP) {
					if (line > 0) {
						line--;
						clear();
						for (int i = offset; i < (int)lines.size(); i++)
							println(i - offset, lines[i].get<0>(), lines[i].get<1>());
						highlight_lines(line, line + 1);
					}
					else if (line == 0 && offset > 0) {
						offset--;
						clear();
						for (int i = offset; i < (int)lines.size(); i++)
							println(i - offset, lines[i].get<0>(), lines[i].get<1>());
						highlight_lines(line, line + 1);
					}
				}
				else if (event.key.keysym.sym == SDLK_ESCAPE || event.key.keysym.sym == SDLK_q) {
					ZtatsWin::Instance().update_player_list();
					return -1;
				}
				else if (event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_SPACE) {
					ZtatsWin::Instance().update_player_list();
					return line + offset;
				}
			}
		}
	}

	return -1;
}

// By default, player = -1, if only up/down scrolling is required.

void ZtatsWin::scroll(int player)
{
  SDL_Event event;
  unsigned offset = 0;

  for (int i = offset; i < (int)lines.size(); i++)
    println(i - offset, lines[i].get<0>(), lines[i].get<1>());

  while (1) {
    if (SDL_WaitEvent(&event)) {
      if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_LEFT && player >= 0) {
          if (player > 0) {
            player--;
            build_ztats_player(player);
            scroll(player);
            return;
          }
        }
        else if (event.key.keysym.sym == SDLK_RIGHT && player >= 0) {
          if (player < Party::Instance().party_size() - 1) {
            player++;
            build_ztats_player(player);
            scroll(player);
            return;
          }
        }
        else if (event.key.keysym.sym == SDLK_UP) {
          if (offset > 0) {
            offset--;
            clear();
            for (int i = offset; i < (int)lines.size(); i++)
              println(i - offset, lines[i].get<0>(), lines[i].get<1>());
          }
        }
        else if (event.key.keysym.sym == SDLK_DOWN) {
          if (offset < lines.size() - 1) {
            offset++;
            clear();
            for (int i = offset; i < (int)lines.size(); i++)
              println(i - offset, lines[i].get<0>(), lines[i].get<1>());
          }
        }
        else if (event.key.keysym.sym == SDLK_ESCAPE || event.key.keysym.sym == SDLK_q) {
          offset = 0;

          ZtatsWin::Instance().update_player_list();
          SDLWindow::Instance().blit_interior();
          return;
        }
      }
    }
  }
}

void ZtatsWin::ztats_player(int p)
{
  build_ztats_player(p);
  scroll(p);
}

void ZtatsWin::set_lines(std::vector<line_tuple> new_lines)
{
  // lines.clear();
  lines = new_lines;
}

void ZtatsWin::build_ztats_player(int p, int lines_hidden)
{
  // Clear the vector first.
  lines.clear();

  PlayerCharacter* player = Party::Instance().get_player(p);
  int second_col = 29;

  if (player == NULL)
    return;

  clear();
  ostringstream temps;

  // Name
  // lines.push_back(boost::tuple<player->name(), CENTERALIGN>);
  lines.push_back(line_tuple(player->name(), CENTERALIGN));

  // Race, Profession
  switch (player->race()) {
  case HUMAN:
    temps << "Human ";
    break;
  case ELF:
    temps << "Elf ";
    break;
  case HOBBIT:
    temps << "Hobbit ";
    break;
  case HALF_ELF:
    temps << "Half-Elf ";
    break;
  case DWARF:
    temps << "Dwarf ";
    break;
  }
  switch (player->profession()) {
  case FIGHTER:
    temps << "Fighter";
    break;
  case PALADIN:
    temps << "Paladin";
    break;
  case THIEF:
    temps << "Thief";
    break;
  case BARD:
    temps << "Bard";
    break;
  case MAGE:
    temps << "Mage";
    break;
  case CLERIC:
    temps << "Cleric";
    break;
  case DRUID:
    temps << "Druid";
    break;
  case NECROMANCER:
    temps << "Necromancer";
    break;
  case ARCHMAGE:
    temps << "Archmage";
    break;
  case GEOMANCER:
    temps << "Geomancer";
    break;
  case SHEPHERD:
    temps << "Shepherd";
    break;
  case TINKER:
    temps << "Tinker";
    break;
  }
  temps << " (";
  if (player->sex())
    temps << (char)16;
  else
    temps << (char)17;
  temps << ")";
  lines.push_back(line_tuple(temps.str(), CENTERALIGN));
  lines.push_back(line_tuple(" ", LEFTALIGN));

  temps.str(""); temps.clear();
  temps << "   Condition: ";
  switch (player->condition()) {
  case POISONED:
    temps << "Poisoned";
    break;
  case DEAD:
    temps << "Dead";
    break;
  default:
    temps << "Good";
    break;
  }
  for (int i = temps.str().length(); i < second_col; i++)
    temps << " ";
  temps << " Strength: " << player->str();
  lines.push_back(line_tuple(temps.str(), LEFTALIGN));

  temps.str(""); temps.clear();
  temps << "       Level: "; // TODO: Compute level via EP points, do not explicitly store Level value!
  for (int i = temps.str().length(); i < second_col; i++)
    temps << " ";
  temps << "Dexterity: " << player->dxt();
  lines.push_back(line_tuple(temps.str(), LEFTALIGN));

  temps.str(""); temps.clear();
  temps << "  Experience: " << player->ep();
  for (int i = temps.str().length(); i < second_col; i++)
    temps << " ";
  temps << "  Stamina: " << player->end();
  lines.push_back(line_tuple(temps.str(), LEFTALIGN));

  temps.str(""); temps.clear();
  temps << "  Hit Points: " << player->hp() << "/" << player->hpm();
  for (int i = temps.str().length(); i < second_col; i++)
    temps << " ";
  temps << "     Luck: " << player->luck();
  lines.push_back(line_tuple(temps.str(), LEFTALIGN));

  temps.str(""); temps.clear();
  temps << "Spell Points: " << player->sp() << "/" << player->spm();
  for (int i = temps.str().length(); i < second_col; i++)
    temps << " ";
  temps << "   Wisdom: " << player->wis();
  lines.push_back(line_tuple(temps.str(), LEFTALIGN));

  temps.str(""); temps.clear();
  temps << "Armour class: ";
  temps << player->armour_class();
  for (int i = temps.str().length(); i < second_col; i++)
    temps << " ";
  temps << " Charisma: " << player->charr();
  lines.push_back(line_tuple(temps.str(), LEFTALIGN));

  temps.str(""); temps.clear();
  for (int i = temps.str().length(); i < second_col; i++)
    temps << " ";
  temps << "Intellig.: " << player->iq();
  lines.push_back(line_tuple(temps.str(), LEFTALIGN));

  temps.str(""); temps.clear();
  temps << " Hands: ";
  if (player->weapon() != NULL && player->weapon()->hands() > 1)
    temps << player->weapon()->name();
  else if (player->shield() != NULL)
    temps << player->shield()->name() << " (l), ";
  else
    temps << "empty (l), ";
  if (player->weapon() != NULL && player->weapon()->hands() == 1)
    temps << player->weapon()->name() << " (r)";
  else
    temps << "empty (r)";

  lines.push_back(line_tuple(temps.str(), LEFTALIGN));
  lines.push_back(line_tuple("  Head: ", LEFTALIGN));
  lines.push_back(line_tuple("Armour: ", LEFTALIGN));
  lines.push_back(line_tuple("  Feet: ", LEFTALIGN));

  lines.push_back(line_tuple("Skills: ", LEFTALIGN));
}

void ZtatsWin::update_player_list()
{
	int i = 0;

	clear();

	for (std::vector<PlayerCharacter>::iterator player = Party::Instance().party_begin();
			player != Party::Instance().party_end();
			player++, i++, i++)
	{
		ostringstream name, name_stats;
		name << (i + 2)/2 << "-" << player->name();
		println(i, name.str());

		name_stats << "  AC: " << player->armour_class();
		name_stats << " HP: " << player->hp() << "/" << player->hpm();
		name_stats << " SP: " << player->sp() << "/" << player->spm();
		println(i + 1, name_stats.str());
	}
}
