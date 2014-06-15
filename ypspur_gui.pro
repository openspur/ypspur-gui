#-------------------------------------------------
#
# Project created by QtCreator 2014-06-08T15:22:02
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ypspur-gui
TEMPLATE = app


SOURCES += main.cpp \
    ypspur_gui.cpp

HEADERS  += \
    ypspur_gui.h

win32:LIBS += -lsetupapi
win32:INCLUDEPATH += /mingw/include/ddk

win32:QMAKE_CXXFLAGS_RELEASE -= -O
win32:QMAKE_CXXFLAGS_RELEASE -= -O1
win32:QMAKE_CXXFLAGS_RELEASE -= -O2

win32:QMAKE_CXXFLAGS_RELEASE *= -Os


FORMS += \
    ypspur_gui.ui

CONFIG += static


INSTALLS += target
target.path = /usr/local/bin

