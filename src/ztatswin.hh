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

enum class SelectionMode {
	MultipleItems,
	SingleItem
};

class ZtatsWin : public SDLWindowRegion
{
protected:
	SDL_Color highlight_colour, standard_bgcolour;
	ZtatsWinContentProvider* _content_provider;

	std::vector<StringAlignmentTuple> lines; // TODO: Remove me when refactoring is done!

	ZtatsWin();
	void build_ztats_player(int, int = 0);
	void swap_colours(int, int, SDL_Color, SDL_Color);
	ZtatsWinContentProvider* content_provider();
	void print_single_page(unsigned = 0, unsigned = 0);

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
	std::vector<T> execute(ZtatsWinContentSelectionProvider<T>* content_selection_provider, SelectionMode selection_mode)
	{
		std::vector<T> selected_items;
		_content_provider = new ZtatsWinContentProvider();
		_content_provider->add_content_page(content_selection_provider->get_page_strings_only());

		if (selection_mode == SelectionMode::SingleItem) {
			int selected_item_no = select_item();

			if (selected_item_no >= 0)
				selected_items.push_back((T)(content_selection_provider->get_page()[selected_item_no].second));
		}
		else if (selection_mode == SelectionMode::MultipleItems) {
			; // TODO
		}

		delete _content_provider;
		_content_provider = NULL;
		return selected_items;
	}

	// TODO: Should be protected. Move to protected, when rest of refactoring is done/attacked.
	void scroll(unsigned start_page = 0);
};

#endif
