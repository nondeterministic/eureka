#ifndef ZTATSWIN_HH
#define ZTATSWIN_HH

#include <SDL.h>
#include <iostream>
#include <vector>

#include "eureka.hh"
#include "sdlwindowregion.hh"
#include "type.hh"
#include "charset.hh"
#include "item.hh"
#include "ztatswincontentprovider.hh"

class ZtatsWin : public SDLWindowRegion
{
protected:
	SDL_Color highlight_colour, standard_bgcolour;
	ZtatsWinContentProvider* _content_provider;
	ZtatsWinContentSelectionProvider<void*>* _content_selection_provider;

	std::vector<StringAlignmentTuple> lines; // TODO: Remove me when refactoring is done!

	ZtatsWin();
	void build_ztats_player(int, int = 0);
	void swap_colours(int, int, SDL_Color, SDL_Color);
	ZtatsWinContentProvider* content_provider();
	void print_single_page(unsigned = 0, unsigned = 0);
	void print_selection_page(unsigned = 0);

public:
	static ZtatsWin& Instance();

	void update_player_list();
	void highlight_lines(int, int);
	void unhighlight_lines(int, int);
	void unhighlight_all();
	void ztats_player(int);
	int select_player();
	void set_lines(std::vector<StringAlignmentTuple>);
	int select_item();
	std::vector<int> select_items();
	void execute(ZtatsWinContentProvider*, unsigned = 0);

	template <class T>
	T execute(ZtatsWinContentSelectionProvider<T>* content_selection_provider)
	{
		_content_selection_provider = (ZtatsWinContentSelectionProvider<void*>*) content_selection_provider;

		Item* selected_item = NULL;

		SDL_Event event;
		unsigned topmost_line_number = 0;

		print_selection_page();

		while (SDL_WaitEvent(&event)) {
			if (event.type == SDL_KEYDOWN) {
				if (event.key.keysym.sym == SDLK_UP) {
					if (topmost_line_number > 0) {
						topmost_line_number--;
						print_selection_page(topmost_line_number);
					}
				}
				else if (event.key.keysym.sym == SDLK_DOWN) {
					if (topmost_line_number < _content_selection_provider->get_page().size() - 1) {
						topmost_line_number++;
						print_selection_page(topmost_line_number);
					}
				}
				else if (event.key.keysym.sym == SDLK_ESCAPE || event.key.keysym.sym == SDLK_q) {
					topmost_line_number = 0;

					ZtatsWin::Instance().update_player_list();
					SDLWindow::Instance().blit_interior();
					return NULL;
				}
			}
		}

		_content_selection_provider = NULL;
		return NULL;
	}

	// TODO: Should be protected. Move to protected, when rest of refactoring is done/attacked.
	void scroll(unsigned start_page = 0);
};

#endif
