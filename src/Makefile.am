AM_CPPFLAGS = -I$(top_srcdir)/include
bin_PROGRAMS = wicher_server
wicher_server_SOURCES = ConnectionManager.cpp \
	DatabaseCreator.cpp \
	DatabaseDropper.cpp \
	DatabaseGetter.cpp \
	DatabaseHelper.cpp \
	DatabaseManager.cpp \
	DatabaseUpdater.cpp \
	Log.cpp \
	_main.cpp \
	Main.cpp \
	Makefile.am \
	MessageParser.cpp \
	Toolkit.cpp
wicher__server_CPPFLAGS = $(jansson_CFLAGS) $(AM_CPPFLAGS)
wicher_server_LDADD = $(jansson_LIBS)
if USE_WINSOCK
        wicher_server_LDADD += -lws2_32
endif
