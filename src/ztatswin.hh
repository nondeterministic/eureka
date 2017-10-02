#ifndef ZTATSWIN_HH
#define ZTATSWIN_HH

#include <iostream>
#include <vector>

#include <SDL2/SDL.h>

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
	SDL_Color _bgcolour_highlight, _bgcolour_standard;
	ZtatsWinContentProvider* _content_provider;
	SDL_Texture* _tmp_texture;

	                         ZtatsWin();
	                    void change_bg_colour(int, int, SDL_Color, SDL_Color);
	ZtatsWinContentProvider* content_provider();
	                    void print_single_page(unsigned = 0, unsigned = 0);
	                     int select_item();
	        std::vector<int> select_items();
	                    void scroll(unsigned start_page = 0);

public:
	static ZtatsWin& Instance();

	void update_player_list();
	void highlight_lines(int, int);
	void unhighlight_lines(int, int);
	// void unhighlight_all();
	int select_player();
	void execute(ZtatsWinContentProvider*, unsigned = 0);

	template <class T>
	std::vector<T> execute(ZtatsWinContentSelectionProvider<T>* content_selection_provider, SelectionMode selection_mode)
	{
		std::vector<T> selected_items;
		_content_provider = new ZtatsWinContentProvider();
		_content_provider->add_content_page(content_selection_provider->get_page_strings_only());

		if (_content_provider->get_pages().size() <= 0) {
			std::cerr << "WARNING: ztatswin.hh: Selection content provider is empty, so aborting the item selection process.\n";
			delete _content_provider;
			return selected_items;
		}

		if (selection_mode == SelectionMode::SingleItem) {
			int selected_item_no = select_item();

			if (selected_item_no >= 0)
				selected_items.push_back((T)(content_selection_provider->get_page()[selected_item_no].second));
		}
		else if (selection_mode == SelectionMode::MultipleItems)
			for (int selected_item_no: select_items())
				selected_items.push_back((T)(content_selection_provider->get_page()[selected_item_no].second));

		delete _content_provider;
		_content_provider = NULL;
		return selected_items;
	}
};

#endif
