#ifndef __ICONS_HH
#define __ICONS_HH

#include <vector>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "iconprops.hh"

class Icons
{
protected:
	Icons();
	std::vector<IconProps> _icons_props;
	SDL_Surface* _icons_surface;
	std::vector<SDL_Surface*> _icon_surfaces;
	std::vector<SDL_Texture*> _icon_textures;
	int convert_icons_to_textures(SDL_Renderer*, int, int);

public:
	virtual ~Icons();
	void reserve(unsigned);
	SDL_Texture* get_sdl_icon(unsigned);
	unsigned number_of_icons();

	// This function adds properties about an icon to the world's
	// register.  However, if the properties for an icon already exist
	// in that register, they will simply and silently be overwritten.
	// Life's tough!
	void add_props(IconProps);
	// Get properties object for an icon, NULL if none was specified or
	// the icon does not exist.
	IconProps* get_props(int);
};

#endif
