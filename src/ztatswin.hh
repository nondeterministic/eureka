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

	// Note, there is some unsafe casting going on in this function, however, since it's local to this function, it should be safe.
	template <class T> std::vector<T> execute(ZtatsWinContentSelectionProvider<T>* content_selection_provider, SelectionMode selection_mode)
	{
		std::vector<T> selected_items;
		// Forward casting into a void-pointer.
		_content_selection_provider = (ZtatsWinContentSelectionProvider<void*>*) content_selection_provider;

		if (selection_mode == SelectionMode::SingleItem) {
			int selected_item_no = select_item();

			if (selected_item_no >= 0)
				// Backward casting from void-pointer to T.
				selected_items.push_back((T)(_content_selection_provider->get_page()[selected_item_no].second));
		}

		_content_selection_provider = NULL;
		return selected_items;
	}

	// TODO: Should be protected. Move to protected, when rest of refactoring is done/attacked.
	void scroll(unsigned start_page = 0);
};

#endif
