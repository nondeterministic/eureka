bin_PROGRAMS = eureka 

SUBDIRS = leibniz

# -std=c++0x

AM_CXXFLAGS = -fdiagnostics-color=auto -g -O0 -Wall -std=c++11  \
	$(LIBXMLPP_CFLAGS) \
	$(SDL_CFLAGS) \
	$(BOOST_CPPFLAGS) \
	$(LUA_CFLAGS) \
	-DDATADIR=\"@datadir@\" \
	-DPREFIX=\"@prefix@\"

AUTOMAKE_OPTIONS = no-dependencies

LIBS = @LIBXMLPP_LIBS@ @SDL_LIBS@ \
	@BOOST_SYSTEM_LIBS@ @BOOST_FILESYSTEM_LIBS@ @BOOST_SIGNALS_LIBS@ @BOOST_REGEX_LIBS@ \
	@LUA_LIBS@ @LIBS@

eureka_SOURCES = \
        eureka.hh eureka.cc \
	clock.hh clock.cc \
	util.hh util.cc \
        luaapi.hh luaapi.cc \
	luawrapper.hh luawrapper.cc \
	spellsbearer.hh spellsbearer.cc \
	gamecharacter.hh gamecharacter.cc \
	creature.hh creature.cc \
	sdlwindow.hh sdlwindow.cc \
	inventory.hh inventory.cc \
	party.hh party.cc \
	map.hh map.cc \
	maphelper.hh maphelper.cc \
	spell.hh spell.cc \
	spellcasthelper.hh spellcasthelper.cc \
	mischelper.hh mischelper.cc \
	world.hh world.cc \
	type.hh type.cc \
	sdltricks.hh sdltricks.cc \
	sdlwindowregion.hh \
	ztatswin.hh ztatswin.cc \
	ztatswincontentprovider.hh ztatswincontentprovider.cc \
	tinywin.hh tinywin.cc \
	miniwin.hh miniwin.cc \
	console.hh console.cc \
	charset.hh charset.cc \
	indoorsmap.hh indoorsmap.cc \
	outdoorsmap.hh outdoorsmap.cc \
	mapobj.hh mapobj.cc \
	action.hh action.cc \
	actiontake.hh actiontake.cc \
	actiononenter.hh actiononenter.cc \
	actionpullpush.hh actionpullpush.cc \
	actionopened.hh actionopened.cc \
	actonlook.hh actonlook.cc \
	playlist.hh playlist.cc \
	soundsample.hh soundsample.cc \
	eventmanager.hh eventmanager.cc \
	gameevent.hh gameevent.cc \
	eventdeleteobj.hh eventdeleteobj.cc \
	eventaddobj.hh eventaddobj.cc \
	eventermap.hh eventermap.cc \
	eventleavemap.hh eventleavemap.cc \
	eventplaysound.hh eventplaysound.cc \
	eventchangeicon.hh eventchangeicon.cc \
	eventprintcon.hh eventprintcon.cc \
	eventluascript.hh eventluascript.cc \
	gameeventhandler.hh gameeventhandler.cc \
	iconprops.hh iconprops.cc \
	icons.hh icons.cc \
	indoorsicons.hh indoorsicons.cc \
	outdoorsicons.hh outdoorsicons.cc \
	arena.hh arena.cc \
	gamerules.hh gamerules.cc \
	gamecontrol.hh gamecontrol.cc \
	attackoption.hh attackoption.cc \
	combat.hh combat.cc \
	hexarena.hh hexarena.cc \
	squarearena.hh squarearena.cc \
	race.hh profession.hh \
	playercharacter.hh playercharacter.cc \
	item.hh item.cc \
	jimmylock.hh jimmylock.cc \
	gold.hh gold.cc \
	edible.hh edible.cc \
	service.hh service.cc \
	miscitem.hh miscitem.cc \
	shield.hh shield.cc \
	weapon.hh weapon.cc \
	itemfactory.hh itemfactory.cc \
	weaponhelper.hh weaponhelper.cc \
	shieldhelper.hh shieldhelper.cc \
	edibleshelper.hh edibleshelper.cc \
	serviceshelper.hh serviceshelper.cc \
	attackers.hh attackers.cc \
	conversation.hh conversation.cc \
	pathfinding.hh pathfinding.cc \
	gamestate.hh gamestate.cc
