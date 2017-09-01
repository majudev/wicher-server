AM_CXXFLAGS = -I$(top_srcdir)/include -std=gnu++11
bin_PROGRAMS = wicher_server
wicher_server_SOURCES = ConnectionManager.cpp \
	MainOptions.cpp \
	Config.cpp \
	Toolkit.cpp \
	_main.cpp
#	DatabaseCreator.cpp \
#	DatabaseDropper.cpp \
#	DatabaseGetter.cpp \
#	DatabaseHelper.cpp \
#	DatabaseManager.cpp \
#	DatabaseUpdater.cpp \
#	Log.cpp \
#	_main.cpp \
#	Main.cpp \
#	Makefile.am \
#	MessageParser.cpp \
#	Toolkit.cpp
wicher_server_CPPFLAGS = $(jansson_CFLAGS)
wicher_server_LDADD = $(jansson_LIBS) -lpthread
