#ifndef YPSPUR_GUI_H
#define YPSPUR_GUI_H

#include <stdio.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <sys/types.h>

#include <QMainWindow>
#include <QFile>
#include <QProcess>
#include <QTextEdit>
#include <QMetaObject>
#include <QMutex>
#include <QSettings>

namespace Ui {
class YPSpur_gui;
}


class YPSpur_gui : public QMainWindow
{
    Q_OBJECT

public:
    explicit YPSpur_gui(QWidget *parent = 0);
    ~YPSpur_gui();

private slots:
    void on_coordinatorStart_toggled(bool checked);
    void updateCoordinatorError();
    void updateCoordinatorText();
    void updateInterpreterError();
    void updateInterpreterText();

    void on_parameterBrowse_clicked();

    void on_portList_textChanged(const QString &arg1);

    void on_interpreterCommand_returnPressed();

private:
    Ui::YPSpur_gui *ui;

    QProcess interpreter;
    QProcess coordinator;
    QMutex mutexInterpreterOutput;
    QMutex mutexCoordinatorOutput;

    QString paramFile;
    QString port;

    QSettings settings;
};

#endif // YPSPUR_GUI_H
