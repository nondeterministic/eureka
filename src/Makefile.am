bin_PROGRAMS = simplicissimus

SUBDIRS = leibniz

# -std=c++0x

AM_CXXFLAGS = -fdiagnostics-color=auto -g -O0 -Wall -std=c++11 \
	$(LIBXMLPP_CFLAGS) \
	$(SDL_CFLAGS) \
	$(BOOST_CPPFLAGS) \
	$(LUA_CFLAGS) \
	-DDATADIR=\"@datadir@\" \
	-DPREFIX=\"@prefix@\"

LIBS = @LIBXMLPP_LIBS@ @SDL_LIBS@ \
	@BOOST_SYSTEM_LIBS@ @BOOST_FILESYSTEM_LIBS@ @BOOST_SIGNALS_LIBS@ \
	@LUA_LIBS@ @LIBS@

simplicissimus_SOURCES = \
        simplicissimus.hh simplicissimus.cc \
	clock.hh clock.cc \
	util.hh util.cc \
        luaapi.hh luaapi.cc \
	luawrapper.hh luawrapper.cc \
	gamecharacter.hh gamecharacter.cc \
	creature.hh creature.cc \
	sdlwindow.hh sdlwindow.cc \
	inventory.hh inventory.cc \
	party.hh party.cc \
	map.hh map.cc world.hh world.cc \
	type.hh type.cc \
	sdltricks.hh sdltricks.cc \
	sdlwindowregion.hh \
	ztatswin.hh ztatswin.cc \
	tinywin.hh tinywin.cc \
	miniwin.hh miniwin.cc \
	console.hh console.cc \
	charset.hh charset.cc \
	indoorsmap.hh indoorsmap.cc \
	outdoorsmap.hh outdoorsmap.cc \
	mapobj.hh mapobj.cc \
	action.hh action.cc \
	actiononenter.hh actiononenter.cc \
	actionpullpush.hh actionpullpush.cc \
	playlist.hh playlist.cc \
	soundsample.hh soundsample.cc \
	eventmanager.hh eventmanager.cc \
	gameevent.hh gameevent.cc \
	eventermap.hh eventermap.cc \
	eventchangeicon.hh eventchangeicon.cc \
	eventprintcon.hh eventprintcon.cc \
	gameeventhandler.hh gameeventhandler.cc \
	iconprops.hh iconprops.cc \
	icons.hh icons.cc \
	indoorsicons.hh indoorsicons.cc \
	outdoorsicons.hh outdoorsicons.cc \
	arena.hh arena.cc \
	gamerules.hh gamerules.cc \
	gamecontrol.hh gamecontrol.cc \
	combat.hh combat.cc \
	hexarena.hh hexarena.cc \
	squarearena.hh squarearena.cc \
	race.hh profession.hh \
	playercharacter.hh playercharacter.cc \
	item.hh item.cc \
	gold.hh gold.cc \
	shield.hh shield.cc \
	weapon.hh weapon.cc \
	itemfactory.hh itemfactory.cc \
	weaponhelper.hh weaponhelper.cc \
	shieldhelper.hh shieldhelper.cc \
	attackers.hh attackers.cc \
	conversation.hh conversation.cc \
	gamestate.hh gamestate.cc
