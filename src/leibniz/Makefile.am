bin_PROGRAMS = leibniz

AM_CXXFLAGS = -g -Wall -O0 -fdiagnostics-color=auto -std=c++14 \
	    $(GTKMM_CFLAGS) \
	    $(LIBXMLPP_CFLAGS) \
	    $(SDL_CFLAGS) \
	    $(BOOST_CPPFLAGS) \
	    $(LUA_CFLAGS) \
	    -DDATADIR=\"@datadir@\" -DPREFIX=\"@prefix@\" -DEDITOR_COMPILE=\"YES\"

AUTOMAKE_OPTIONS = no-dependencies

leibniz_SOURCES = leibniz.hh leibniz.cc \
		../sdl_shared.hh \
		../util.hh ../util.cc \
		../item.hh ../item.cc \
		../weapon.hh ../weapon.cc \
		../shield.hh ../shield.cc \
		../armour.hh ../armour.cc \
		../edible.hh ../edible.cc \
		../spellsbearer.hh ../spellsbearer.cc \
		../gamecharacter.hh ../gamecharacter.cc \
		../attackers.hh ../attackers.cc \
		../creature.hh ../creature.cc \
		../map.cc ../map.hh \
		obj.cc obj.hh \
		../action.cc ../action.hh \
		../actonlook.cc ../actonlook.hh \
		../actiontake.cc ../actiontake.hh \
		../actiononenter.cc ../actiononenter.hh \
		../actionpullpush.cc ../actionpullpush.hh \
		../actionopened.cc ../actionopened.hh \
		addactionwin.cc addactionwin.hh \
		addobjectwin.cc addobjectwin.hh \
		../mapobj.cc ../mapobj.hh \
		../gameevent.cc ../gameevent.hh \
		../eventermap.cc ../eventermap.hh \
		../eventleavemap.cc ../eventleavemap.hh \
		../eventaddobj.cc ../eventaddobj.hh \
		../eventdeleteobj.cc ../eventdeleteobj.hh \
		../eventplaysound.cc ../eventplaysound.hh \
		../eventprintcon.cc ../eventprintcon.hh \
		../eventchangeicon.cc ../eventchangeicon.hh \
		../eventluascript.cc ../eventluascript.hh \
		eventwin.cc eventwin.hh \
		../luawrapper.hh ../luawrapper.cc \
		../outdoorsmap.cc ../outdoorsmap.hh \
		../indoorsmap.cc ../indoorsmap.hh \
		../outdoorsicons.cc ../outdoorsicons.hh \
		../iconprops.cc ../iconprops.hh \
		../icons.cc ../icons.hh \
		../indoorsicons.cc ../indoorsicons.hh \
		../spell.hh ../spell.cc \
		../world.cc ../world.hh \
		../gamerules.cc ../gamerules.hh \
		iconpropswin.cc iconpropswin.hh \
		newworldwin.cc newworldwin.hh  \
		startupwin.cc startupwin.hh \
		newmapwin.cc newmapwin.hh \
		editorwin.cc editorwin.hh \
		sdleditor.cc sdleditor.hh \
		indoorssdleditor.cc indoorssdleditor.hh \
		outdoorssdleditor.cc outdoorssdleditor.hh \
		tabcontext.cc tabcontext.hh \
		selectmapwin.cc selectmapwin.hh \
		expandmapwin.cc expandmapwin.hh 

LIBS = @GTKMM_LIBS@ @LIBXMLPP_LIBS@ @SDL_LIBS@ \
	@BOOST_SYSTEM_LIBS@ @BOOST_FILESYSTEM_LIBS@ @BOOST_REGEX_LIBS@ \
	@LUA_LIBS@ @LIBS@
