AM_CPPFLAGS = -I$(top_srcdir)/include
bin_PROGRAMS = wicher-server
wicher_server_SOURCES = ConnectionManager.cpp \
	Connection.cpp \
	DatabaseCreator.cpp \
	DatabaseDropper.cpp \
	DatabaseGetter.cpp \
	DatabaseHelper.cpp \
	DatabaseManager.cpp \
	DatabaseUpdater.cpp \
	UID.cpp \
	Log.cpp \
	_main.cpp \
	Main.cpp \
	MessageParser.cpp \
	Toolkit.cpp
wicher_server_CPPFLAGS = $(AM_CPPFLAGS) $(BOOST_CPPFLAGS) $(jansson_CFLAGS)
wicher_server_LDADD = $(BOOST_LDFLAGS) $(BOOST_THREAD_LIB) $(BOOST_SYSTEM_LIB) $(jansson_LIBS)
