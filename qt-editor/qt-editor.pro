TEMPLATE = app
TARGET = qt-editor
DEPENDPATH += . GeneratedFiles
INCLUDEPATH += .
QT += widgets

HEADERS += \
    mainwindow.h \
    rownumberview.h \
    track.h \
    trackarea.h \
    trackgroup.h \
    trackgroupview.h \
    trackview.h

FORMS += mainwindow.ui

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    rownumberview.cpp \
    track.cpp \
    trackarea.cpp \
    trackgroup.cpp \
    trackgroupview.cpp \
    trackview.cpp
