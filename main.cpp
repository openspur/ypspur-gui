#include "ypspur_gui.h"
#include <QApplication>

#ifdef _WIN32
#include <QtCore/QtPlugin>
Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin)
#endif  // _WIN32

int main(int argc, char* argv[])
{
  QApplication app(argc, argv);
  YPSpurGUI gui;
  gui.show();

  return app.exec();
}
