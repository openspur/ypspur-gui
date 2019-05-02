#include "ypspur_gui.h"
#include <QApplication>

int main(int argc, char* argv[])
{
  QApplication app(argc, argv);
  YPSpurGUI gui;
  gui.show();

  return app.exec();
}
