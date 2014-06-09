#include "ypspur_gui.h"
#include "ui_ypspur_gui.h"

#include <iostream>
#include <QRegExp>
#include <QFileDialog>
#include <QScrollBar>


YPSpur_gui::YPSpur_gui(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::YPSpur_gui),
    settings(QString("ypspur"))
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
#else
    coordinatorPath = settings.value(
                "coordinator/path",
                "/usr/local/bin/ypspur-coordinator").toString();
    interpreterPath = settings.value(
                "interpreter/path",
                "/usr/local/bin/ypspur-interpreter").toString();
#endif

    port = settings.value("coordinator/port", "/dev/ttyACM0").toString();
    if(!port.isEmpty()) ui->portList->addItem(port);

    paramFile = settings.value("coordinator/param", "").toString();
    QString paramName = paramFile;
    paramName.replace(QRegExp("^.*([^/\\\\]*)$"),"\\1");
    if(!paramFile.isEmpty()) ui->parameterName->setText(paramName);
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
        ui->parameterBrowse->setDisabled(true);
        ui->portList->setDisabled(true);
        connect(&coordinator, SIGNAL(readyReadStandardError()), this, SLOT(updateCoordinatorError()));
        connect(&coordinator, SIGNAL(readyReadStandardOutput()), this, SLOT(updateCoordinatorText()));
        connect(&interpreter, SIGNAL(readyReadStandardError()), this, SLOT(updateInterpreterError()));
        connect(&interpreter, SIGNAL(readyReadStandardOutput()), this, SLOT(updateInterpreterText()));

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
        interpreter.close();
        coordinator.close();
        ui->parameterBrowse->setDisabled(false);
        ui->portList->setDisabled(false);

        mutexCoordinatorOutput.lock();
        ui->coordinatorOut->textCursor().movePosition(QTextCursor::End);
        ui->coordinatorOut->insertHtml("<hr style=\"width:100%;background-color:#CCCCCC;height:1pt;\"><br>");
        ui->coordinatorOut->verticalScrollBar()->setValue(
                    ui->coordinatorOut->verticalScrollBar()->maximum());
        mutexCoordinatorOutput.unlock();
    }
}

void YPSpur_gui::updateCoordinatorError()
{
    mutexCoordinatorOutput.lock();
    QString data(coordinator.readAllStandardError() );

    data.replace(QRegExp(" "),"&nbsp;");
    data.replace(QRegExp("\n"),"<br>");
    ui->coordinatorOut->textCursor().movePosition(QTextCursor::End);
    ui->coordinatorOut->insertHtml("<b>" + data + "</b>");
    ui->coordinatorOut->verticalScrollBar()->setValue(
                ui->coordinatorOut->verticalScrollBar()->maximum());
    mutexCoordinatorOutput.unlock();
    if(data.contains("Command&nbsp;analyser&nbsp;started."))
    {
        interpreter.start(interpreterPath);
    }
}

void YPSpur_gui::updateCoordinatorText()
{
    mutexCoordinatorOutput.lock();
    QString data(coordinator.readAllStandardOutput());

    data.replace(QRegExp(" "),"&nbsp;");
    data.replace(QRegExp("\n"),"<br>");
    ui->coordinatorOut->textCursor().movePosition(QTextCursor::End);
    ui->coordinatorOut->insertHtml(data);
    ui->coordinatorOut->verticalScrollBar()->setValue(
                ui->coordinatorOut->verticalScrollBar()->maximum());
    mutexCoordinatorOutput.unlock();
}

void YPSpur_gui::updateInterpreterError()
{
    mutexInterpreterOutput.lock();
    QString data(interpreter.readAllStandardError() );

    data.replace(QRegExp(" "),"&nbsp;");
    data.replace(QRegExp("\n"),"<br>");
    ui->interpreterOut->textCursor().movePosition(QTextCursor::End);
    ui->interpreterOut->insertHtml("<b>" + data + "</b>");
    ui->interpreterOut->verticalScrollBar()->setValue(
                ui->interpreterOut->verticalScrollBar()->maximum());
    mutexInterpreterOutput.unlock();
}

void YPSpur_gui::updateInterpreterText()
{
    mutexInterpreterOutput.lock();
    QString data(interpreter.readAllStandardOutput());

    data.replace(QRegExp(" "),"&nbsp;");
    data.replace(QRegExp("\n"),"<br>");
    ui->interpreterOut->textCursor().movePosition(QTextCursor::End);
    ui->interpreterOut->insertHtml(data);
    ui->interpreterOut->verticalScrollBar()->setValue(
                ui->interpreterOut->verticalScrollBar()->maximum());
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
