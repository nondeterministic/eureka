// ----------------------------------------------------------------
// leibniz
//
// Copyright (c) 2009  Andreas Bauer <baueran@gmail.com>
//
// This is not free software.  See COPYING for further information.
// ----------------------------------------------------------------

#include <iostream>
#include "boost/lexical_cast.hpp"
#include "expandmapwin.hh"

ExpandMapWin::ExpandMapWin(const char* _subtitle, const char* _title)
  :  top_lbl("Top:"),
     bot_lbl("Bottom:"),
     right_lbl("Right:"),
     left_lbl("Left:"),
     title_lbl(_title)
{
  // Just some initialisation in case someone gets the values before
  // running the dialog - though this should not happen.
  _top = 0; _bot = 0; _right = 0; _left = 0;

  set_title(_subtitle);
  show_all_children();
  set_modal(true);

  align.set_padding(10, 10, 10, 10);
  get_vbox()->add(align);

  vbox.add(title_lbl);

  top_entry.set_text("0");
  bot_entry.set_text("0");
  right_entry.set_text("0");
  left_entry.set_text("0");

  hbox_top.set_spacing(10);
  hbox_top.add(top_lbl);
  hbox_top.pack_start(top_entry, Gtk::PACK_SHRINK);

  hbox_bot.set_spacing(10);
  hbox_bot.add(bot_lbl);
  hbox_bot.pack_start(bot_entry, Gtk::PACK_SHRINK);

  hbox_right.set_spacing(10);
  hbox_right.add(right_lbl);
  hbox_right.pack_start(right_entry, Gtk::PACK_SHRINK);

  hbox_left.set_spacing(10);
  hbox_left.add(left_lbl);
  hbox_left.pack_start(left_entry, Gtk::PACK_SHRINK);

  vbox.add(hbox_top);
  vbox.add(hbox_bot);
  vbox.add(hbox_right);
  vbox.add(hbox_left);
  align.add(vbox);
  
  bcancel = manage(add_button(Gtk::Stock::CANCEL, 0));
  bok = manage(add_button(Gtk::Stock::OK, 1));

  bcancel->
    signal_clicked().
    connect(sigc::mem_fun(*this, &ExpandMapWin::on_button_cancel));
  bok->
    signal_clicked().
    connect(sigc::mem_fun(*this, &ExpandMapWin::on_button_ok));

  vbox.set_spacing(10);

  bok->set_can_default();
  bok->grab_default();

  show_all_children();
}

ExpandMapWin::~ExpandMapWin(void)
{
}

void ExpandMapWin::on_button_cancel(void)
{
  hide();
}

void ExpandMapWin::on_button_ok(void)
{
  try
    {
      _top = boost::lexical_cast<int>(top_entry.get_text().c_str());
      _bot = boost::lexical_cast<int>(bot_entry.get_text().c_str());
      _right = boost::lexical_cast<int>(right_entry.get_text().c_str());
      _left = boost::lexical_cast<int>(left_entry.get_text().c_str());
    }
  catch (...)
    {
      std::cerr << "Bad dimensions?" << std::endl;
    }
  hide();
}

void ExpandMapWin::get_values(int& top, int& bot, int& right, int& left)
{
  top = _top;
  bot = _bot;
  right = _right;
  left = _left;
}
