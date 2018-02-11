AC_PREREQ([2.69])
AC_INIT([eureka], [0.0.1], [baueran@gmail.com]) 

AC_CONFIG_SRCDIR([src/eureka.cc])
AC_CONFIG_HEADERS([src/config.h])
AC_CONFIG_MACRO_DIR([m4])

# The boost stuff wants us to use libtool, oh well...
LT_INIT
AC_PROG_LIBTOOL

AM_INIT_AUTOMAKE([foreign subdir-objects])
AM_MAINTAINER_MODE

# Checks for programs:
AC_PROG_CC
AC_PROG_CXX

# Check for SDL and SDL features, i.e. -lSDL2
SDL_VERSION=2.0.5
AM_PATH_SDL2($SDL_VERSION, 
            [], 
            [AC_MSG_ERROR([*** SDL version $SDL_VERSION not found!])]) 
AC_CHECK_LIB(SDL2_image, IMG_ReadXPMFromArray)
AC_CHECK_HEADER([SDL2/SDL_image.h],
                [],
                [AC_MSG_ERROR([*** SDL_image include files not found!
		You should install the development package.])])
AC_CHECK_LIB(SDL2_mixer, Mix_OpenAudio)
AC_CHECK_HEADERS([SDL2/SDL_mixer.h], 
                [],
                [AC_MSG_ERROR([*** SDL_mixer include files not found!
		You should install the development package.])])

AC_LANG([C++]) 

# Only for the editor:
PKG_CHECK_MODULES(GTKMM,
        [gtkmm-3.0 >= 3.0])
AC_SUBST(GTKMM_CFLAGS)
AC_SUBST(GTKMM_LIBS)

PKG_CHECK_MODULES(LIBXMLPP,
	[libxml++-2.6])
AC_SUBST(LIBXMLPP_CFLAGS)
AC_SUBST(LIBXMLPP_LIBS) 

BOOST_REQUIRE([1.54.0])
BOOST_FILESYSTEM
BOOST_REGEX
BOOST_STRING_ALGO
BOOST_SIGNALS
BOOST_BIND
BOOST_SYSTEM
AC_SUBST(BOOST_CPPFLAGS)
AC_SUBST(BOOST_CXXFLAGS)

## Checking for Lua 5.1
PKG_CHECK_MODULES(
	[LUA],
	[lua5.1 >= 5.1.3],
	[], ## do nothing special
	[PKG_CHECK_MODULES([LUA], [lua5.1 >= 5.1.3], [])])
AC_MSG_CHECKING(for lua libraries)
AC_MSG_RESULT($LUA_LIBS)
AC_SUBST([LUA_CFLAGS])
AC_SUBST([LUA_LIBS])

AC_CONFIG_FILES([Makefile \
	         src/Makefile \
		 src/Doxyfile \
		 src/leibniz/Makefile \
		 m4/Makefile \
		 data/Makefile \
		 data/leibniz/Makefile \
		 data/Demoworld/Makefile \
		 data/Demoworld/images/Makefile \
		 data/Demoworld/maps/Makefile \
		 data/Demoworld/sound/Makefile \
		 data/Demoworld/services/Makefile \
		 data/Demoworld/weapons/Makefile \
		 data/Demoworld/armour/Makefile \
		 data/Demoworld/shields/Makefile \
		 data/Demoworld/bestiary/Makefile \
		 data/Demoworld/miscitems/Makefile \
		 data/Demoworld/edibles/Makefile \
		 data/Demoworld/people/Makefile \
		 data/Demoworld/spells/Makefile \
		 data/Demoworld/potions/Makefile \
		 data/Demoworld/spells/mage/Makefile])
AC_OUTPUT

echo "
  ($PACKAGE_NAME) version $PACKAGE_VERSION

  Prefix.........:        $prefix
  C++ Compiler...:        $CXX $CXXFLAGS $CPPFLAGS 
      		     SDL: $SDL_CFLAGS
		   Boost: $BOOST_CPPFLAGS
		     Lua: $LUA_CFLAGS
		   XML++: $LIBXMLPP_CFLAGS
  Linker.........:        $LD $LDFLAGS
  		     SDL: $SDL_LIBS 
  		   Boost: $BOOST_SIGNALS_LIBS
  		          $BOOST_FILESYSTEM_LIBS 
		          $BOOST_SYSTEM_LIBS
			  $BOOST_REGEX_LIBS
		     Lua: $LUA_LIBS
		    Libs: $LIBS"
