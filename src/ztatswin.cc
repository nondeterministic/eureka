#include "ztatswin.hh"
#include "sdlwindow.hh"
#include "party.hh"
#include "playercharacter.hh"
#include "sdltricks.hh"
#include "gamecontrol.hh"
#include "eventmanager.hh"
#include "ztatswincontentprovider.hh"
#include "eureka.hh"

#include <boost/bind.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/lexical_cast.hpp>

#include <iostream>
#include <list>
#include <vector>
#include <sstream>
#include <algorithm>

using namespace std;

ZtatsWin::ZtatsWin()
{
	_content_provider = NULL;

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

/// Highlights lines from to to in the ztats window.  Players in the
/// standard view, e.g., occupy two lines each.
///
/// This function will be used to let players move the cursor up and
/// down, selecting stuff from the ztats window.

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

// Swaps background colour A for B in lines from_top to to_bottom in the ztats window.

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

/*! Let user select a player from the party in the stats window using cursor keys.
 *
 * @return number of the selected player corresponding to its rank in the party, or -1 if selection was aborted.
 */

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

std::vector<int> ZtatsWin::select_items()
{
	std::vector<int> result;
	unsigned line = 0;
	SDL_Event event;
	unsigned offset = 0;
	const int dheight = 16;  // Ztats display is 16 lines tall

	vector<StringAlignmentTuple> page = content_provider()->get_pages()[0];

	print_single_page();
	highlight_lines(line, line + 1);

	while (1) {
		if (SDL_WaitEvent(&event)) {
			if (event.type == SDL_KEYDOWN) {
				if (event.key.keysym.sym == SDLK_DOWN) {
					if (line == dheight - 1 && offset < page.size() - 1) {
						offset++;
						clear();
						for (unsigned i = offset; i < page.size(); i++)
							println_noblit(i - offset, page[i].get<0>(), page[i].get<1>());
						blit();
						highlight_lines(line, line + 1);
					}
					else if (line < dheight - 1 && line < page.size() - 1) {
						line++;
						clear();
						for (unsigned i = offset; i < page.size(); i++)
							println_noblit(i - offset, page[i].get<0>(), page[i].get<1>());
						blit();
						highlight_lines(line, line + 1);
					}
				}
				else if (event.key.keysym.sym == SDLK_UP) {
					if (line > 0) {
						line--;
						clear();
						for (unsigned i = offset; i < page.size(); i++)
							println_noblit(i - offset, page[i].get<0>(), page[i].get<1>());
						blit();
						highlight_lines(line, line + 1);
					}
					else if (line == 0 && offset > 0) {
						line--;
						offset--;
						clear();
						for (int i = offset; i < page.size(); i++)
							println_noblit(i - offset, page[i].get<0>(), page[i].get<1>());
						blit();
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

						StringAlignmentTuple tuple     = page[line + offset];
						std::string linestr  = tuple.get<0>();
						page[line + offset] = StringAlignmentTuple("*" + linestr.substr(linestr.find_first_of(")")),  // String
																   tuple.get<1>());                                   // Alginment
					}
					else {
						result.erase(std::find(result.begin(), result.end(), line + offset));

						StringAlignmentTuple tuple     = page[line + offset];
						std::string linestr  = tuple.get<0>();
						page[line + offset] = StringAlignmentTuple(boost::lexical_cast<string>(line + offset + 1) + linestr.substr(linestr.find_first_of(")")),
								tuple.get<1>());
					}

					for (unsigned i = offset; i < page.size(); i++)
						println_noblit(i - offset, page[i].get<0>(), page[i].get<1>());
					blit();

					highlight_lines(line, line + 1);
				}
			}
		}
	}

	return result;
}

/// Select single item.
/// Returns -1 if no item was selected, otherwise the nth line that was selected.

int ZtatsWin::select_item()
{
	int line = 0;
	SDL_Event event;
	unsigned offset = 0;
	const int dheight = 16;  // Ztats display is 16 lines tall

	if (content_provider()->get_pages().size() != 1) {
		std::cerr << "WARNING: ztatswin.cc: ZtatsWinContentSelection provider holds " << content_provider()->get_pages().size() << " page(s), but should hold exactly one.\n";
		return -1;
	}

	vector<StringAlignmentTuple> page = content_provider()->get_pages()[0];

	print_single_page();
	highlight_lines(line, line + 1);

	while (SDL_WaitEvent(&event)) {
		if (event.type == SDL_KEYDOWN) {
			if (event.key.keysym.sym == SDLK_DOWN && offset + line + 1 < page.size())  // +1 because we just pressed down again!
			{
				if (line == dheight - 1 && offset < page.size() - 1) {
					offset++;
					clear();
					for (int i = offset; i < (int)page.size(); i++)
						println_noblit(i - offset, page[i].get<0>(), page[i].get<1>());
					blit();
					highlight_lines(line, line + 1);
				}
				else if (line < dheight - 1 && line < (int)page.size() - 1) {
					line++;
					clear();
					for (int i = offset; i < (int)page.size(); i++)
						println_noblit(i - offset, page[i].get<0>(), page[i].get<1>());
					blit();
					highlight_lines(line, line + 1);
				}
			}
			else if (event.key.keysym.sym == SDLK_UP) {
				if (line > 0) {
					line--;
					clear();
					for (int i = offset; i < (int)page.size(); i++)
						println_noblit(i - offset, page[i].get<0>(), page[i].get<1>());
					blit();
					highlight_lines(line, line + 1);
				}
				else if (line == 0 && offset > 0) {
					offset--;
					clear();
					for (int i = offset; i < (int)page.size(); i++)
						println_noblit(i - offset, page[i].get<0>(), page[i].get<1>());
					blit();
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

	return -1;
}

/// Return content provider for ZtatsWin if there is one, or throw exception, if it is undefined (NULL).

ZtatsWinContentProvider* ZtatsWin::content_provider()
{
	if (_content_provider == NULL)
		throw std::runtime_error("EXCEPTION: ztatswin.cc: ZtatsWin's content provider is NULL.");
	return _content_provider;
}

void ZtatsWin::execute(ZtatsWinContentProvider* content_provider, unsigned start_page)
{
	_content_provider = content_provider;

	try {
		scroll(start_page);
		_content_provider = NULL;
	}
	catch (...) {

	}
}

void ZtatsWin::print_single_page(unsigned page, unsigned offet_from_top)
{
	if (page > content_provider()->get_pages().size() - 1) {
		std::cerr << "ERROR: ztatswin.cc: Cannot access page " << page << " of content provider, when there are only " << content_provider()->get_pages().size() << " available.\n";
		return;
	}

	clear(); // Clear before printing or we get letter-salad...
	for (unsigned curr_line = offet_from_top; curr_line < content_provider()->get_pages()[page].size(); curr_line++)
		println_noblit(curr_line - offet_from_top, content_provider()->get_pages()[page][curr_line].get<0>(), content_provider()->get_pages()[page][curr_line].get<1>());
	blit();
}

void ZtatsWin::scroll(unsigned start_page)
{
	if (start_page >= content_provider()->get_pages().size())
		throw std::invalid_argument("Cannot show page " + std::to_string(start_page) + " of " + std::to_string(content_provider()->get_pages().size()) + " pages.");

	SDL_Event event;
	unsigned topmost_line_number = 0;
	unsigned curr_page = start_page;

	print_single_page(curr_page, topmost_line_number);

	while (SDL_WaitEvent(&event)) {
		if (event.type == SDL_KEYDOWN) {
			if (event.key.keysym.sym == SDLK_LEFT) {
				if (content_provider()->get_pages().size() > 1) {
					if (curr_page == 0)
						curr_page = content_provider()->get_pages().size() - 1;
					else
						curr_page = std::max(0, (int)curr_page - 1);

					topmost_line_number = 0;
					print_single_page(curr_page, topmost_line_number);
				}
			}
			else if (event.key.keysym.sym == SDLK_RIGHT) {
				if (content_provider()->get_pages().size() > 1) {
					if (curr_page == content_provider()->get_pages().size() - 1)
						curr_page = 0;
					else
						curr_page = std::min((int)(content_provider()->get_pages().size() - 1), (int)(curr_page + 1));

					topmost_line_number = 0;
					print_single_page(curr_page, topmost_line_number);
				}
			}
			else if (event.key.keysym.sym == SDLK_UP) {
				if (topmost_line_number > 0) {
					topmost_line_number--;
					print_single_page(curr_page, topmost_line_number);
				}
			}
			else if (event.key.keysym.sym == SDLK_DOWN) {
				if (topmost_line_number < content_provider()->get_pages()[curr_page].size() - 1) {
					topmost_line_number++;
					print_single_page(curr_page, topmost_line_number);
				}
			}
			else if (event.key.keysym.sym == SDLK_ESCAPE || event.key.keysym.sym == SDLK_q) {
				topmost_line_number = 0;

				ZtatsWin::Instance().update_player_list();
				SDLWindow::Instance().blit_interior();
				return;
			}
		}
	}
}

void ZtatsWin::update_player_list()
{
	int i = 0;

	clear();

	for (std::vector<PlayerCharacter>::iterator player = Party::Instance().begin();
			player != Party::Instance().end();
			player++, i++, i++)
	{
		std::string condition = "G";
		switch (player->condition()) {
		case GOOD:
			condition = "G";
			break;
		case POISONED:
			condition = "P";
			break;
		case DEAD:
			condition = "D";
			break;
		}

		ostringstream name, name_stats;
		name << (i + 2)/2 << "-" << player->name() << " (" << condition << ")";
		println(i, name.str());

		name_stats << "  AC: " << player->armour_class();
		name_stats << " HP: " << player->hp() << "/" << player->hpm();
		name_stats << " SP: " << player->sp() << "/" << player->spm();
		println(i + 1, name_stats.str());
	}
}
