#ifndef TABCONTEXT_HH
#define TABCONTEXT_HH

#include <gtkmm.h>

class TabContext
{
public:
  TabContext();
  TabContext(Gtk::Box*);
  ~TabContext();

  Gtk::Box* get_box() const;
  void set_box(Gtk::Box*);
  unsigned get_xoffset() const;
  unsigned get_yoffset() const;
  void set_xoffset(unsigned);
  void set_yoffset(unsigned);
  unsigned get_icon_brush_no() const;
  void set_icon_brush_no(unsigned);

protected:
  Gtk::Box* _box;
  // This offset is counted in terms of icons, not pixel!  This is
  // necessary to not get confused when the user disables the grid in
  // between context switches.  Later we add a grid pixel respectively
  // to all icons if necessary.
  int _xoffset, _yoffset;
  int _icon_brush_number;
};

#endif
