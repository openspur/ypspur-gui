#-------------------------------------------------
#
# Project created by QtCreator 2014-06-08T15:22:02
#
#-------------------------------------------------

lessThan(QT_MAJOR_VERSION, 5) {
  error("ypspur-gui requires QT>=5")
}

QT += core gui widgets

TARGET = ypspur-gui
TEMPLATE = app

SOURCES += \
  main.cpp \
  ypspur_gui.cpp
HEADERS  += \
  ypspur_gui.h
FORMS += \
  ypspur_gui.ui

win32:QTPLUGIN.platforms = qwindows
win32:LIBS += \
  -lsetupapi \
  -lqwindows \
  -ldwmapi \
  -lQt5EventDispatcherSupport \
  -lQt5FontDatabaseSupport \
  -lQt5ThemeSupport \
  -lQt5VulkanSupport \
  -lQt5WindowsUIAutomationSupport

win32:QMAKE_CXXFLAGS_RELEASE *= -Os

win32:CONFIG += static

INSTALLS += target
target.path = /usr/local/bin
