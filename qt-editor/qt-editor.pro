TEMPLATE = app
TARGET = qt-editor
DEPENDPATH += . GeneratedFiles
INCLUDEPATH += .

HEADERS += mainwindow.h \
           multitrackview.h \
           rownumberview.h \
           trackarea.h \
           trackview.h

FORMS += mainwindow.ui

SOURCES += main.cpp \
           mainwindow.cpp \
           multitrackview.cpp \
           rownumberview.cpp \
           trackarea.cpp \
           trackview.cpp
