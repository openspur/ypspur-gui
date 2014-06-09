#include "ypspur_gui.h"
#include "ui_ypspur_gui.h"

#include <iostream>
#include <QRegExp>
#include <QFileDialog>
#include <QScrollBar>
#include <QDir>
#include <QEvent>


void printTextEdit(QTextEdit *out, QString str)
{
    out->moveCursor(QTextCursor::End,QTextCursor::MoveAnchor);
    out->insertHtml(str);
    out->verticalScrollBar()->setValue(
                out->verticalScrollBar()->maximum());
}

YPSpur_gui::YPSpur_gui(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::YPSpur_gui),
    settings(QString("ypspur-gui"))
{
    ui->setupUi(this);
    Qt::WindowFlags flags = Qt::Window | Qt::WindowMaximizeButtonHint |
            Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint |
            Qt::CustomizeWindowHint;
    setWindowFlags(flags);

#ifdef _WIN32
    coordinatorPath = settings.value(
                "coordinator/path",
                "./ypspur-coordinator").toString();
    interpreterPath = settings.value(
                "interpreter/path",
                "./ypspur-interpreter").toString();
    devicePath = settings.value("coordinator/devicePath", "").toString();
    deviceName = settings.value("coordinator/deviceName", "").toString();
#else
    coordinatorPath = settings.value(
                "coordinator/path",
                "/usr/local/bin/ypspur-coordinator").toString();
    interpreterPath = settings.value(
                "interpreter/path",
                "/usr/local/bin/ypspur-interpreter").toString();
    devicePath = settings.value("coordinator/devicePath", "/dev/").toString();
    deviceName = settings.value("coordinator/deviceName", "ttyACM*").toString();
#endif

    port = settings.value("coordinator/port", "/dev/ttyACM0").toString();
    if(!port.isEmpty()) ui->portList->addItem(port);

    paramFile = settings.value("coordinator/param", "").toString();
    QString paramName = paramFile;
    paramName.replace(QRegExp("^.*([^/\\\\]*)$"),"\\1");
    if(!paramFile.isEmpty()) ui->parameterName->setText(paramName);

    connect(&coordinator, SIGNAL(readyReadStandardError()), this, SLOT(updateCoordinatorError()));
    connect(&coordinator, SIGNAL(readyReadStandardOutput()), this, SLOT(updateCoordinatorText()));
    connect(&coordinator, SIGNAL(finished(int)), this, SLOT(coordinatorQuit(int)));
    connect(&interpreter, SIGNAL(readyReadStandardError()), this, SLOT(updateInterpreterError()));
    connect(&interpreter, SIGNAL(readyReadStandardOutput()), this, SLOT(updateInterpreterText()));
    connect(&interpreter, SIGNAL(finished(int)), this, SLOT(interpreterQuit(int)));

    ui->portList->installEventFilter(this);
}

YPSpur_gui::~YPSpur_gui()
{
    settings.sync();
    delete ui;
}

void YPSpur_gui::on_coordinatorStart_toggled(bool checked)
{
    if(checked)
    {
        mutexCoordinatorOutput.lock();
        printTextEdit(ui->coordinatorOut, "<hr style=\"width:100%;background-color:#CCCCCC;height:1pt;\">");
        mutexCoordinatorOutput.unlock();

        ui->parameterBrowse->setDisabled(true);
        ui->portList->setDisabled(true);

        QStringList args;
        args.append("--device");
        args.append(port);
        args.append("--param");
        args.append(paramFile);
        args.append("--update-param");
        coordinator.start(coordinatorPath, args);
    }
    else
    {
        if(interpreter.state() != QProcess::NotRunning) interpreter.close();
        if(coordinator.state() != QProcess::NotRunning) coordinator.close();
        ui->parameterBrowse->setDisabled(false);
        ui->portList->setDisabled(false);
    }
}

void YPSpur_gui::coordinatorQuit(int exitCode)
{
    printTextEdit(ui->coordinatorOut, "<br><i>Exit code: " + QString().number(exitCode) + "</i><br>");
    ui->coordinatorStart->toggle();
}

void YPSpur_gui::interpreterQuit(int exitCode)
{
    printTextEdit(ui->interpreterOut, "<br><i>Exit code: " + QString().number(exitCode) + "</i><br>");
}

void YPSpur_gui::updateCoordinatorError()
{
    mutexCoordinatorOutput.lock();
    QString data(coordinator.readAllStandardError() );

    data.replace(QRegExp(" "),"&nbsp;");
    data.replace(QRegExp("\n"),"<br>");
    printTextEdit(ui->coordinatorOut, "<b>" + data + "</b>");
    mutexCoordinatorOutput.unlock();
    if(data.contains(QRegExp("Command&nbsp;analy[zs]er&nbsp;started\\.")))
    {
        mutexInterpreterOutput.lock();
        printTextEdit(ui->interpreterOut, "<hr style=\"width:100%;background-color:#CCCCCC;height:1pt;\">");
        mutexInterpreterOutput.unlock();
        interpreter.start(interpreterPath);
    }
}

void YPSpur_gui::updateCoordinatorText()
{
    mutexCoordinatorOutput.lock();
    QString data(coordinator.readAllStandardOutput());

    data.replace(QRegExp(" "),"&nbsp;");
    data.replace(QRegExp("\n"),"<br>");
    printTextEdit(ui->coordinatorOut, data);
    mutexCoordinatorOutput.unlock();
}

void YPSpur_gui::updateInterpreterError()
{
    mutexInterpreterOutput.lock();
    QString data(interpreter.readAllStandardError() );

    data.replace(QRegExp(" "),"&nbsp;");
    data.replace(QRegExp("\n"),"<br>");
    printTextEdit(ui->interpreterOut, "<b>" + data + "</b>");
    mutexInterpreterOutput.unlock();
}

void YPSpur_gui::updateInterpreterText()
{
    mutexInterpreterOutput.lock();
    QString data(interpreter.readAllStandardOutput());

    data.replace(QRegExp(" "),"&nbsp;");
    data.replace(QRegExp("\n"),"<br>");
    printTextEdit(ui->interpreterOut, data);
    mutexInterpreterOutput.unlock();
}

void YPSpur_gui::on_parameterBrowse_clicked()
{
    QString defaultPath("./");
    if(!paramFile.isEmpty())
    {
        defaultPath = paramFile;
    }
    QString fileName = QFileDialog::getOpenFileName(
                this,
                QString("Open Image"),
                defaultPath,
                QString("Parameter files (*.param)"));
    if(!fileName.isEmpty())
    {
        paramFile = fileName;

        QString paramName = fileName;
        paramName.replace(QRegExp("^.*([^/\\\\]*)$"),"\\1");
        ui->parameterName->setText(paramName);

        settings.setValue("coordinator/param", paramFile);
    }
}

void YPSpur_gui::on_portList_textChanged(const QString &arg1)
{
    if(!arg1.isNull())
    {
        port = arg1;
        settings.setValue("coordinator/port", port);
    }
}

void YPSpur_gui::on_interpreterCommand_returnPressed()
{
    QString str = ui->interpreterCommand->text();
    str.append("\n");
    interpreter.write(str.toLocal8Bit());
    ui->interpreterCommand->setText("");
}

bool YPSpur_gui::eventFilter(QObject *obj, QEvent *event)
{
    if(event->type() == QEvent::MouseButtonPress)
    {
#ifdef _WIN32
#else
        QDir dir(devicePath, deviceName);
        dir.setFilter(QDir::AllEntries | QDir::System);
        dir.makeAbsolute();

        QFileInfoList list = dir.entryInfoList();
        ui->portList->clear();
        fprintf(stderr,"test %d\n", list.count());
        for(int i = 0; i < list.count(); i++)
        {
            ui->portList->addItem(list[i].absoluteFilePath());
        }
#endif
    }
    return QObject::eventFilter(obj, event);
}
