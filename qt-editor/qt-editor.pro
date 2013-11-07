TEMPLATE = app
TARGET = qt-editor
DEPENDPATH += . GeneratedFiles
INCLUDEPATH += .
QT += widgets

HEADERS += mainwindow.h \
           rownumberview.h \
           trackarea.h \
           trackview.h \
    trackgroupview.h

FORMS += mainwindow.ui

SOURCES += main.cpp \
           mainwindow.cpp \
           rownumberview.cpp \
           trackarea.cpp \
           trackview.cpp \
    trackgroupview.cpp
