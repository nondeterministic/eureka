// This source file is part of eureka
//
// Copyright (c) 2007-2018  Andreas Bauer <a@pspace.org>
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

#include <gtkmm/image.h>
#include <gtkmm/application.h>

// See http://www.boost.org/doc/libs/1_46_1/libs/filesystem/v3/doc/index.htm
#define BOOST_FILESYSTEM_VERSION 3

// See http://www.robertnitsch.de/notes/cpp/cpp11_boost_filesystem_undefined_reference_copy_file
#define BOOST_NO_SCOPED_ENUMS
#define BOOST_NO_CXX11_SCOPED_ENUMS
#include "boost/filesystem.hpp"
#include "boost/filesystem/path.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/regex.hpp>

#include <iostream>
#include <memory>
#include <string>

#ifndef EDITOR_COMPILE
#include "eureka.hh"
#else
#include "leibniz.hh"
#endif

#include "startupwin.hh"
#include "newworldwin.hh"
#include "editorwin.hh"
#include "../world.hh"
#include "../config.h"

StartupWin::StartupWin()
: logo((conf_data_path / "leibniz" / "logo.png").string()),
  bnew("_Start new world..."),
  bcancel(Gtk::Stock::CANCEL),
  breopen("_Reopen old world")
{
	set_title("Welcome to Leibniz interactive world builder!");
	set_resizable(false);

	bcancel.signal_clicked().connect(sigc::mem_fun(*this, &StartupWin::on_button_close));
	bcancel.set_use_underline();

	breopen.signal_clicked().connect(sigc::mem_fun(*this, &StartupWin::on_button_reopen));

	bnew.signal_clicked().connect(sigc::mem_fun(*this, &StartupWin::on_button_new));
	bnew.set_use_underline();

	align.set_padding(10, 10, 10, 10);
	add(align);

	vbox.set_spacing(10);
	vbox.add(logo);
	vbox.add(sep);

	hbox.pack_start(bcancel);
	hbox.pack_start(breopen, Gtk::PACK_EXPAND_WIDGET, 10);
	hbox.pack_start(bnew);

	vbox.add(hbox);
	align.add(vbox);

	breopen.set_image(*Gtk::manage(new Gtk::Image(Gtk::Stock::REDO, Gtk::ICON_SIZE_BUTTON)));

	// Grab default calls should always be late, or
	// we get "widget not within a GtkWindow errror - God knows why.
	bnew.set_can_default();
	bnew.set_image(*Gtk::manage(new Gtk::Image(Gtk::Stock::NEW, Gtk::ICON_SIZE_BUTTON)));
	bnew.grab_default();

	show_all_children();
}

void StartupWin::on_button_close(void)
{
	hide();
}

void StartupWin::on_button_reopen(void)
{
	Gtk::FileChooserDialog dialog("Please choose a world XML-file", Gtk::FILE_CHOOSER_ACTION_OPEN);
	dialog.set_transient_for(*this);

	dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	dialog.add_button(Gtk::Stock::OPEN, Gtk::RESPONSE_OK);

	Glib::RefPtr<Gtk::FileFilter> filter_xml = Gtk::FileFilter::create();
	filter_xml->set_name("XML files");
	filter_xml->add_mime_type("text/xml");
	dialog.add_filter(filter_xml);

	Glib::RefPtr<Gtk::FileFilter> filter_any = Gtk::FileFilter::create();
	filter_any->set_name("Any files");
	filter_any->add_pattern("*");
	dialog.add_filter(filter_any);

	dialog.set_show_hidden(true);

	// Show the dialog and wait for a user response
	switch (dialog.run())
	{
	case(Gtk::RESPONSE_OK): {
		std::string filename = dialog.get_filename();
		std::string world_name = "";

		// This regex should work, but somehow doesn't.  Weird!
		// boost::regex regex(".*(?:\\|\/)([^\\\/]+)\.xml", boost::regex::perl|boost::regex::icase);

		// Extract world name from filename
		// boost::regex regex("([^\\\/]+)\.xml", boost::regex::perl|boost::regex::icase);
		boost::regex regex("([^\\/]+)\\.xml", boost::regex::perl|boost::regex::icase);
		boost::match_results<std::string::const_iterator> results;

		if (boost::regex_search(filename, results, regex)) {
			world_name = results[1];
		}
		else {
			std::cerr << "ERROR: startupwin.cc: Could not extract world name of path '" << filename << "'. Did you use spaces or hidden characters?\n";
			exit(0);
		}

		// Set some global configuration variable
		conf_world_path = boost::filesystem::path(conf_data_path / world_name);

		if (World::Instance().xml_load_world_data(filename)) {
			dialog.hide();
			hide();

			if (world_name != World::Instance().get_name()) {
				std::cerr << "ERROR: startupwin.cc: The world name in the world XML file is not the same as it's file name. (Did you manually edit the world XML file?)\n";
				break;
			}

			// Now start editor
			EditorWin editor_win;
			Gtk::Main::run(editor_win);
		}
		else
			std::cerr << "ERROR: startupwin.cc: Error loading world xml data: " << filename << std::endl;

		break;
    }
	case(Gtk::RESPONSE_CANCEL):
		break;
	default:
		std::cout << "INFO: Unexpected button clicked." << std::endl;
		break;
	}
}

void StartupWin::on_button_new(void)
{
	// First ask for the basic properties of the new world
	NewWorldWin newworld_win;
	if (!newworld_win.run())
		return;

	// template only: boost::filesystem::path conf_world_path = boost::filesystem::path(conf_data_path / world_name);
	// TODO

	std::string data_dir = World::Instance().get_path().c_str();
	std::string world_dir  =  data_dir
			+ (std::string)World::Instance().get_name().c_str();
	std::string world_file = world_dir + ".xml";

	// Set up dialog window, we may need it if the user selects a path
	// that doesn't exist (see below)
	Gtk::MessageDialog
	dirrerr_dlg("Error creating directory.",
			true,
			Gtk::MESSAGE_ERROR,
			Gtk::BUTTONS_OK);
	dirrerr_dlg.set_title("Error");

	if (!(boost::filesystem::exists(data_dir)))	{
		Gtk::MessageDialog
		msgdlg("Directory does not exist.",
				true,
				Gtk::MESSAGE_QUESTION,
				Gtk::BUTTONS_YES_NO);
		msgdlg.set_secondary_text("Do you want to create it?");
		msgdlg.set_title("Directory does not exist");

		if (msgdlg.run() == Gtk::RESPONSE_YES) {
			try	{
				msgdlg.hide();
				boost::filesystem::create_directories(data_dir);
			} catch (...) {
				dirrerr_dlg.set_secondary_text(data_dir);
				dirrerr_dlg.run();
				dirrerr_dlg.hide();
			}
		}
	}

	// Try to create world dir inside data dir and other required
	// directories within.
	try {
		if (boost::filesystem::exists(data_dir)) {
			boost::filesystem::create_directories(world_dir);
			boost::filesystem::create_directories(world_dir + "/maps");
			boost::filesystem::create_directories(world_dir + "/images");
		}
		// } catch (boost::filesystem::basic_filesystem_error<boost::filesystem::path>& e) {
	} catch (boost::filesystem::filesystem_error& e) {
		dirrerr_dlg.set_secondary_text(e.path1().string());
		dirrerr_dlg.run();
		dirrerr_dlg.hide();
	}

	// Start world's main XML file
	World::Instance().xml_write_world_data();

	// If all files and required directories are created, Go!
	try {
		if (World::Instance().get_name().length() > 0
				&& boost::filesystem::exists(world_file)
				&& boost::filesystem::exists(world_dir + "/maps")
				&& boost::filesystem::exists(world_dir + "/images"))
		{
			// Copy media data into world directory
			boost::filesystem::copy_file((std::string)DATADIR +
					"/leibniz/data/icons_indoors.png",
					world_dir +
					"/images/icons_indoors.png");
			boost::filesystem::copy_file((std::string)DATADIR +
					"/leibniz/data/icons_outdoors.png",
					world_dir +
					"/images/icons_outdoors.png");

			// Remove start-up window
			hide();

			// Now start editor...
			// Is now deprecated, see:
			// https://stackoverflow.com/questions/13773846/gtkmainrun-segfaults
			// https://developer.gnome.org/gtkmm-tutorial/stable/sec-buildapp-trivial-app.html.en
			EditorWin editor_win;
			Gtk::Main::run(editor_win);
		}
	} catch (...) {
		std::cerr << "ERROR: startupwin.cc: on_button_new() failed." << std::endl;
	}
}

StartupWin::~StartupWin(void)
{
}
