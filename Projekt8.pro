QT        += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET     = Projekt8
TEMPLATE   = app


SOURCES   += main.cpp \
             mainwindow.cpp \
             canvas.cpp \
             sudoku.cpp \
    undosolve.cpp \
    undofield.cpp

HEADERS   += mainwindow.h \
             canvas.h \
             sudoku.h \
    undosolve.h \
    undofield.h

FORMS     += mainwindow.ui

RESOURCES += res/res.qrc

RC_FILE    = res.rc
