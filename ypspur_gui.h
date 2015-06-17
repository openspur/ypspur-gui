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
    void setParamFile(QString param);

private slots:
    void on_coordinatorStart_toggled(bool checked);
    void updateCoordinatorError();
    void updateCoordinatorText();
    void updateInterpreterError();
    void updateInterpreterText();

    void interpreterQuit(int exitCode);
    void coordinatorQuit(int exitCode);

    void on_parameterBrowse_clicked();
#if QT_VERSION >= 0x050000
    void on_portList_currentTextChanged(const QString &arg1);
#else
    void on_portList_textChanged(const QString &arg1);
#endif
    void on_interpreterCommand_returnPressed();
    void on_coordinatorDefaultParam_clicked();
    void on_coordinatorPath_textChanged(const QString &arg1);
    void on_coordinatorOptions_textChanged(const QString &arg1);
    void on_interpreterPath_textChanged(const QString &arg1);
    void on_interpreterOptions_textChanged(const QString &arg1);

private:
    bool eventFilter(QObject *obj, QEvent *event);


    Ui::YPSpur_gui *ui;

    QProcess interpreter;
    QProcess coordinator;
    QMutex mutexInterpreterOutput;
    QMutex mutexCoordinatorOutput;

    QString paramFile;
    QString port;
    QString coordinatorPath;
    QString interpreterPath;
    QString coordinatorOptions;
    QString interpreterOptions;
    QString devicePath;
    QString deviceName;

    QSettings settings;
};

#endif // YPSPUR_GUI_H
