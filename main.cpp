#include "ypspur_gui.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    YPSpur_gui gui;
    gui.show();

    return app.exec();
}
