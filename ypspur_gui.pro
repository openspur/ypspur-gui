#-------------------------------------------------
#
# Project created by QtCreator 2014-06-08T15:22:02
#
#-------------------------------------------------

lessThan(QT_MAJOR_VERSION, 5) {
  error("ypspur-gui requires QT>=5")
}

QT       += core gui widgets

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

