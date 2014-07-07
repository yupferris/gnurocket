TEMPLATE = app
TARGET = editor
DEPENDPATH += .
INCLUDEPATH += .

QT = core gui xml network

greaterThan(QT_MAJOR_VERSION, 4) {
    QT += widgets
}

# Input
HEADERS += clientsocket.h \
    mainwindow.h \
    syncdocument.h \
    trackview.h \
    ../lib/base.h \
    ../lib/track.h \
    ../lib/data.h \
    ../lib/sync.h

SOURCES += clientsocket.cpp \
    editor.cpp \
    mainwindow.cpp \
    syncdocument.cpp \
    trackview.cpp \
    ../lib/track.c \
    ../lib/data.c

RESOURCES += editor.qrc

RC_FILE = editor.rc

win32:package.files += ${TARGET} $${QMAKE_LIBDIR_QT}/../bin/QtCore$${QT_MAJOR_VERSION}.dll
win32:package.files += ${TARGET} $${QMAKE_LIBDIR_QT}/../bin/QtGui$${QT_MAJOR_VERSION}.dll
win32:package.files += ${TARGET} $${QMAKE_LIBDIR_QT}/../bin/QtNetwork$${QT_MAJOR_VERSION}.dll
package.path = $${DESTDIR}/
package.commands = echo $${TARGET}.$${TARGET_EXT} $${QMAKE_LIBDIR_QT}
INSTALLS += package
