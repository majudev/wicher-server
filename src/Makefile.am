AM_CXXFLAGS = -I$(top_srcdir)/include -std=gnu++11
bin_PROGRAMS = wicher-server
wicher_server_SOURCES = DBman.cpp \
	DBmanPerformer.cpp \
	AuthDB.cpp \
	MainOptions.cpp \
	Config.cpp \
	Toolkit.cpp \
	_main.cpp \
	DatabaseManager.cpp \
	JSONDatabase/DatabaseCreator.cpp \
	JSONDatabase/DatabaseDropper.cpp \
	JSONDatabase/DatabaseGetter.cpp \
	JSONDatabase/DatabaseHelper.cpp \
	JSONDatabase/DatabaseManager.cpp \
	JSONDatabase/DatabaseUpdater.cpp
#	Log.cpp \
#	_main.cpp \
#	Main.cpp \
#	Makefile.am \
#	MessageParser.cpp \
#	Toolkit.cpp
wicher_server_CPPFLAGS = $(jansson_CFLAGS)
wicher_server_LDADD = $(jansson_LIBS) -lpthread
