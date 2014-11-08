#ifndef __ICONS_HH
#define __ICONS_HH

#include "iconprops.hh"
#include <vector>
#include <SDL.h>
#include <SDL_image.h>

class Icons
{
public:
  Icons();
  virtual ~Icons();

  void reserve(unsigned);
  virtual SDL_Surface* get_sdl_icon(unsigned) = 0;
  virtual unsigned number_of_icons() = 0;

  // This function adds properties about an icon to the world's
  // register.  However, if the properties for an icon already exist
  // in that register, they will simply and silently be overwritten.
  // Life's tough!
  void add_props(IconProps);
  // Get properties object for an icon, NULL if none was specified or
  // the icon does not exist.
  IconProps* get_props(unsigned);

protected:
  std::vector<IconProps> _icons_props;
};

#endif
