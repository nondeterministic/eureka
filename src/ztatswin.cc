// This source file is part of eureka
//
// Copyright (c) 2007-2018 Andreas Bauer <baueran@gmail.com>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
// USA.

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
	SDLWindow& win = SDLWindow::Instance();

	set_texture(win.get_texture_ztats());

	int ztats_w, ztats_h;
	if (SDL_QueryTexture(win.get_texture_ztats(), NULL, NULL, &ztats_w, &ztats_h) < 0) {
		std::cerr << "WARNING: ztatswin.cc: cannot set texture: " << IMG_GetError() << "\n";
		exit(EXIT_FAILURE);
	}

	_content_provider = NULL;
	_tmp_texture = nullptr;

	_bgcolour_standard.r = 0;
	_bgcolour_standard.g = 0;
	_bgcolour_standard.b = 0;

	_bgcolour_highlight.r = 50;
	_bgcolour_highlight.g = 50;
	_bgcolour_highlight.b = 250;

	SDL_Rect rect;
	rect.x = win.get_size().first - win.frame_icon_size() - ztats_w + 2;
	rect.y = win.frame_icon_size() - 2;
	rect.w = ztats_w;
	rect.h = ztats_h;
	set_dimensions(rect);
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
	if (_tmp_texture != nullptr) {
		SDL_DestroyTexture(_tmp_texture);
	}

	// Store backup of texture
	_tmp_texture = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, get_dimensions().w, get_dimensions().h);
	SDL_SetRenderTarget(_renderer, _tmp_texture);
	SDL_RenderCopy(_renderer, _texture, NULL, NULL);

	// Now change background color from black to highlight color.
	change_bg_colour(from_top, to_bottom, _bgcolour_highlight, _bgcolour_standard);
	blit();
	SDLWindow::Instance().blit_entire_window_texture();
}

//void ZtatsWin::unhighlight_lines(int from_top, int to_bottom)
//{
//	change_bg_colour(from_top, to_bottom, _bgcolour_standard, _bgcolour_highlight);
//
//	blit();
//	SDLWindow::Instance().blit_entire_window_texture();
//}

void ZtatsWin::unhighlight_lines(int from_top, int to_bottom)
{
	SDL_SetRenderTarget(_renderer, _texture);
	SDL_RenderCopy(_renderer, _tmp_texture, NULL, NULL);

	blit();
	SDLWindow::Instance().blit_entire_window_texture();
}

//void ZtatsWin::unhighlight_all()
//{
//	unhighlight_lines(-1, -1);
//	blit();
//	SDLWindow::Instance().blit_entire_window_texture();
//}

/// Swaps background colour A for B in lines from_top to to_bottom in the ztats window.
/// Negative values swap entire window's colors.

void ZtatsWin::change_bg_colour(int from_top, int to_bottom, SDL_Color to_color, SDL_Color from_color)
{
	// How many text lines are displayed in the ztats window?
	int lines = get_dimensions().h / _font->char_height();

	// Replace entire window's colors, not just single lines...
	if (from_top < 0 || to_bottom + 1 > lines || to_bottom < 0) {
		SDLTricks::Instance().replace_color(_renderer, _texture, to_color, from_color, NULL);
		// SDLTricks::Instance().replace_bg_color(_renderer, _texture, to_color, NULL);
	}
	// Replace single singles...
	else {
		SDL_Rect rect;
		rect.x = 0;
		rect.y = from_top * _font->char_height() + _inter_line_padding * from_top + _y_frame_offset;
		rect.h = (to_bottom - from_top) * _font->char_height() + _inter_line_padding;
		rect.w = get_dimensions().w;
		SDLTricks::Instance().replace_color(_renderer, _texture, to_color, from_color, &rect);
		// SDLTricks::Instance().replace_bg_color(_renderer, _texture, to_color, &rect);
	}
}

/*! Let user select a player from the party in the stats window using cursor keys.
 *
 * @return number of the selected player corresponding to its rank in the party, or -1 if selection was aborted.
 */

int ZtatsWin::select_player()
{
	int player =  0;
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
					SDLWindow::Instance().blit_arena();
					SDLWindow::Instance().blit_entire_window_texture();
					return -1;
				default:
					break;
				}

				highlight_lines(player*2, player*2 + 2);
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
					update_player_list();
					return result;
				}
				else if (event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_SPACE) {
					if (std::find(result.begin(), result.end(), line + offset) == result.end()) {
						result.push_back(line + offset);

						StringAlignmentTuple tuple = page[line + offset];
						std::string linestr = tuple.get<0>();
						page[line + offset] = StringAlignmentTuple("*" + linestr.substr(linestr.find_first_of(")")),  // String
																   tuple.get<1>());                                   // Alginment
					}
					else {
						result.erase(std::find(result.begin(), result.end(), line + offset));

						StringAlignmentTuple tuple = page[line + offset];
						std::string linestr = tuple.get<0>();
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
		std::cerr << "WARNING: ztatswin.cc: ZtatsWinContentSelection provider holds "
				  << content_provider()->get_pages().size() << " page(s), but should hold exactly one.\n";
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
				update_player_list();
				return -1;
			}
			else if (event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_SPACE) {
				update_player_list();
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

				update_player_list();
				SDLWindow::Instance().blit_arena();
				return;
			}
		}

		if (SDLWindow::Instance().blit_console() < 0)
			std::cerr << "WARNING: ztatswin.cc: blit_console in scroll() failed.\n";
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

	blit();
}
