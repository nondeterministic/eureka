// leibniz
//
// Copyright (c) 2009  Andreas Bauer <baueran@gmail.com>
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

#include <iostream>
#include <gtkmm.h>
#include "startupwin.hh"
#include "config.h"

// #include <X11/Xlib.h>
// static int (*X_handler)(Display*, XErrorEvent*) = NULL;
// static int shm_errhandler(Display *d, XErrorEvent *e)
// {
//   std::cout << "ERROR HANDLER" << std::endl;
//   return(X_handler(d,e));
// }

int main(int argc, char *argv[])
{
  Gtk::Main kit(argc, argv);
  StartupWin startupwin;

  // XSetErrorHandler(shm_errhandler);

  kit.run(startupwin);
  return 0;
}
