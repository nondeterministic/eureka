bin_PROGRAMS = leibniz

AM_CXXFLAGS = -g -Wall -std=c++11 \
	    $(GTKMM_CFLAGS) \
	    $(LIBXMLPP_CFLAGS) \
	    $(SDL_CFLAGS) \
	    $(BOOST_CPPFLAGS) \
	    $(LUA_CFLAGS) \
	    -DDATADIR=\"@datadir@\" -DPREFIX=\"@prefix@\"

AUTOMAKE_OPTIONS = subdir-objects

leibniz_SOURCES = leibniz.cc \
		../util.hh ../util.cc \
		../item.hh ../item.cc \
		../weapon.hh ../weapon.cc \
		../shield.hh ../shield.cc \
		../gamecharacter.hh ../gamecharacter.cc \
		../creature.hh ../creature.cc \
		../map.cc ../map.hh \
		obj.cc obj.hh \
		../action.cc ../action.hh \
		../actiononenter.cc ../actiononenter.hh \
		addactionwin.cc addactionwin.hh \
		addobjectwin.cc addobjectwin.hh \
		../mapobj.cc ../mapobj.hh \
		../gameevent.cc ../gameevent.hh \
		../eventermap.cc ../eventermap.hh \
		eventwin.cc eventwin.hh \
		../outdoorsmap.cc ../outdoorsmap.hh \
		../indoorsmap.cc ../indoorsmap.hh \
		../outdoorsicons.cc ../outdoorsicons.hh \
		../iconprops.cc ../iconprops.hh \
		../icons.cc ../icons.hh \
		../indoorsicons.cc ../indoorsicons.hh \
		../world.cc ../world.hh \
		../gamerules.cc ../gamerules.hh \
		iconpropswin.cc iconpropswin.hh \
		newworldwin.cc newworldwin.hh  \
		startupwin.cc startupwin.hh \
		newmapwin.cc newmapwin.hh \
		sdleditor.cc sdleditor.hh \
		indoorssdleditor.cc indoorssdleditor.hh \
		outdoorssdleditor.cc outdoorssdleditor.hh \
		tabcontext.cc tabcontext.hh \
		selectmapwin.cc selectmapwin.hh \
		expandmapwin.cc expandmapwin.hh \
		editorwin.cc editorwin.hh

LIBS = @GTKMM_LIBS@ @LIBXMLPP_LIBS@ @SDL_LIBS@ \
	@BOOST_SYSTEM_LIBS@ @BOOST_FILESYSTEM_LIBS@ \
	@LUA_LIBS@ @LIBS@
