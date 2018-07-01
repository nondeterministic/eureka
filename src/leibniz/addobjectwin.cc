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

#include <gtkmm.h>
#include <sstream>

#include "../mapobj.hh"
#include "../indoorsicons.hh"
#include "addobjectwin.hh"

AddObjectWin::AddObjectWin(Glib::RefPtr<Gdk::Pixbuf>& icon, int icon_no) :
	id_lbl("ID:"),
	object_lbl("Select type of object:"),
	icon_lbl("undef icon no."),
	action_lbl("Action:"),
	lua_name_lbl("default_lua_name:"),
	// cb_removable("removable?"),
	rb_type_frame("Object type"),
	rb_type_item("Item"),
	rb_type_person("Person"),
	rb_type_monster("Monster"),
	rb_type_animal("Animal")
{
	// removable = false;
	_object_no = 0;
	_has_action = false;
	_action_no = 0;
	_icon_no = icon_no;

	set_title("Add an object to the icon");
	show_all_children();
	set_modal(true);

	align.set_padding(10, 10, 10, 10);
	get_vbox()->add(align);

	_icon = new Gtk::Image(icon);
	vbox.add(*_icon);
	std::stringstream ss;
	ss << icon_no;
	icon_lbl.set_text(ss.str());
	vbox.add(icon_lbl);

	hbox_id.set_spacing(10);
	hbox_id.add(id_lbl);
	hbox_id.pack_start(id_entry, Gtk::PACK_SHRINK);
	vbox.add(hbox_id);

	if (IndoorsIcons::Instance().get_props(_icon_no)->default_lua_name().length() > 0)
		lua_name_entry.set_text(IndoorsIcons::Instance().get_props(_icon_no)->default_lua_name());

	hbox_lua_name.set_spacing(10);
	hbox_lua_name.add(lua_name_lbl);
	hbox_lua_name.pack_start(lua_name_entry, Gtk::PACK_SHRINK);
	vbox.add(hbox_lua_name);

	//  hbox.set_spacing(10);
	//  hbox.add(object_lbl);
	//  hbox.pack_start(, Gtk::PACK_SHRINK);
	//  vbox.add(hbox);
	//  objects_combo.append("Item");
	//  objects_combo.append("Person");
	//  objects_combo.append("Monster");
	//  objects_combo.append("Animal");

	Gtk::RadioButton::Group group = rb_type_item.get_group();
	rb_type_item.set_group(group);
	rb_type_item.set_active();
	rb_type_person.set_group(group);
	rb_type_monster.set_group(group);
	rb_type_animal.set_group(group);

	// https://developer.gnome.org/gtkmm-tutorial/stable/sec-radio-buttons.html.en
	//
	rb_type_item.signal_clicked().connect(sigc::mem_fun(*this, &AddObjectWin::handle_rb_type_item_toggled));
	rb_type_person.signal_clicked().connect(sigc::mem_fun(*this, &AddObjectWin::handle_rb_type_person_toggled));
	rb_type_monster.signal_clicked().connect(sigc::mem_fun(*this, &AddObjectWin::handle_rb_type_monster_toggled));
	rb_type_animal.signal_clicked().connect(sigc::mem_fun(*this, &AddObjectWin::handle_rb_type_animal_toggled));

	// http://stackoverflow.com/questions/13385024/read-gtk-radio-button-signal-only-when-selected
	// but then rb_type_item needs to be a local variable, which somehow I couldn't get to work
	// with gtkmm...
	//
	//  rb_type_item.signal_toggled().connect([this,&rb_type_item]{
	//    handle_rb_type_toggled(rb_type_item);
	//  });

	rb_type_vbox.add(rb_type_item);
	rb_type_vbox.add(rb_type_person);
	rb_type_vbox.add(rb_type_monster);
	rb_type_vbox.add(rb_type_animal);
	rb_type_vbox.set_border_width(5);
	rb_type_frame.add(rb_type_vbox);

	vbox.add(rb_type_frame);

	hbox_actions.set_spacing(10);
	hbox_actions.add(action_lbl);
	hbox_actions.pack_start(actions_combo, Gtk::PACK_SHRINK);
	vbox.add(hbox_actions);

	actions_combo.append("None");
	actions_combo.append("Action");

	// vbox.add(cb_removable);

	align.add(vbox);

	bcancel = manage(add_button(Gtk::Stock::CANCEL, 0));
	bok = manage(add_button(Gtk::Stock::OK, 1));

	bcancel->signal_clicked().connect(sigc::mem_fun(*this, &AddObjectWin::on_button_cancel));
	bok->signal_clicked().connect(sigc::mem_fun(*this, &AddObjectWin::on_button_ok));

	vbox.set_spacing(10);

	bok->set_can_default();
	bok->grab_default();

	objects_combo.set_active(_object_no);
	actions_combo.set_active(_action_no);

	show_all_children();
}

void AddObjectWin::handle_rb_type_item_toggled() { _object_no = 0; }
void AddObjectWin::handle_rb_type_person_toggled() { _object_no = 1; }
void AddObjectWin::handle_rb_type_monster_toggled() { _object_no = 2; }
void AddObjectWin::handle_rb_type_animal_toggled() { _object_no = 3; }

AddObjectWin::~AddObjectWin(void)
{
}

void AddObjectWin::on_button_cancel(void)
{
	hide();
}

void AddObjectWin::on_button_ok(void)
{
	// _object_no = objects_combo.get_active_row_number();
	_has_action = actions_combo.get_active_row_number() > 0;
	id = id_entry.get_text();
	default_lua_name = lua_name_entry.get_text();
	// removable = cb_removable.get_active();

	hide();
}

bool AddObjectWin::has_action()
{
	return _has_action;
}

MAPOBJ_TYPES AddObjectWin::get_object_type(void)
{
	switch (_object_no) {
	case 0:
		return MAPOBJ_ITEM;
	case 1:
		return MAPOBJ_PERSON;
	case 2:
		return MAPOBJ_MONSTER;
	default:
		return MAPOBJ_ANIMAL;
	}
}

int AddObjectWin::get_icon_no()
{
	return _icon_no;
}
