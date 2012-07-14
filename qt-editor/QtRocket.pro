QT       += core gui network

TARGET = QtRocket
TEMPLATE = app

SOURCES += main.cpp\
        MainWindow.cpp \
    TrackView.cpp \
    TrackModel.cpp \
    ServerWrapper.cpp \
    clientsocket.cpp \
    SyncTrack.cpp

HEADERS  += MainWindow.h \
    TrackView.h \
    TrackModel.h \
    ServerWrapper.h \
    clientsocket.h \
    SyncTrack.h

FORMS    += MainWindow.ui

#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../lib/sprocket/release/ -lsprocket
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../lib/sprocket/debug/ -lsprocket
#else:symbian: LIBS += -lsprocket
#else:unix: LIBS += -L$$PWD/../libsprocket/build -lsprocket

win32 {
    LIBS += -lwsock32
}

INCLUDEPATH += $$PWD/../libsprocket
DEPENDPATH += $$PWD/../libsprocket



