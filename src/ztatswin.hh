#ifndef ZTATSWIN_HH
#define ZTATSWIN_HH

#include <SDL.h>
#include <iostream>
#include <vector>

#include "sdlwindowregion.hh"
#include "type.hh"
#include "charset.hh"
#include "ztatswincontentprovider.hh"
#include "eureka.hh"

enum class ExecutionMode
{
	DisplayOnly,
	SelectLine
};

class ZtatsWin : public SDLWindowRegion
{
	friend class ZtatsWinContentProvider;

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
	void register_content_provider(ZtatsWinContentProvider*);
	void execute(ExecutionMode execution_mode, unsigned start_page);

	// TODO: Should be protected. Move to protected, when rest of refactoring is done/attacked.
	void scroll(unsigned start_page = 0);

protected:
	SDL_Color highlight_colour, standard_bgcolour;
	ZtatsWinContentProvider* _content_provider;

	std::vector<StringAlignmentTuple> lines;

	ZtatsWin();
	void build_ztats_player(int, int = 0);
	void swap_colours(int, int, SDL_Color, SDL_Color);
	ZtatsWinContentProvider* content_provider();
	void print_single_page(unsigned page = 0, unsigned topmost_line_number = 0);
};

#endif
