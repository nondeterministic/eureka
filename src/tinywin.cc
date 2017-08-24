#include "tinywin.hh"
#include "sdlwindow.hh"
#include "miniwin.hh"
#include "ztatswin.hh"
#include "console.hh"

#include <SDL2/SDL.h>

using namespace std;

TinyWin::TinyWin()
{
	SDLWindow& win = SDLWindow::Instance();

	set_texture(SDLWindow::Instance().get_texture_tiny_win());

	int tinywin_w, tinywin_h;
	if (SDL_QueryTexture(win.get_texture_tiny_win(), NULL, NULL, &tinywin_w, &tinywin_h) < 0) {
		std::cerr << "WARNING: tinywin.cc: cannot set texture: " << IMG_GetError() << "\n";
		exit(EXIT_FAILURE);
	}

	SDL_Rect rect;
	rect.x = win.get_size().first - win.frame_icon_size() - tinywin_w + 2;
	rect.y = win.get_size().second - 2 * win.frame_icon_size() - Console::Instance().get_size().second - tinywin_h + 5;
	rect.w = tinywin_w;
	rect.h = tinywin_h;
	set_dimensions(rect);

	// Alter position of text relative to SDL surface
	_y_frame_offset = 0;
}

TinyWin& TinyWin::Instance()
{
	static TinyWin inst;
	return inst;
}
