#include "ypspur_gui.h"
#include "ui_ypspur_gui.h"

#include <iostream>
#include <QRegExp>
#include <QFileDialog>
#include <QScrollBar>
#include <QDir>
#include <QEvent>

#ifdef _WIN32
#define _UNICODE
#include <windows.h>
#include <locale.h>
#include <setupapi.h>
#include <initguid.h>
#include <basetyps.h>
#include <tchar.h>
#include <ntdef.h>
#include <ntddser.h>
#endif

void printTextEdit(QTextEdit* out, QString str)
{
  out->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
  out->insertHtml(str);
  out->verticalScrollBar()->setValue(
      out->verticalScrollBar()->maximum());
}

YPSpurGUI::YPSpurGUI(QWidget* parent)
  : QMainWindow(parent)
  , ui_(new Ui::YPSpurGUI)
  , settings_(QString("ypspur-gui"))
{
  ui_->setupUi(this);
  Qt::WindowFlags flags = Qt::Window | Qt::WindowMaximizeButtonHint |
                          Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint |
                          Qt::CustomizeWindowHint;
  setWindowFlags(flags);

#ifdef _WIN32
  _tsetlocale(LC_ALL, _T(""));

  coordinator_path_ = settings_.value("coordinator/path", "./ypspur-coordinator").toString();
  interpreter_path_ = settings_.value("interpreter/path", "./ypspur-interpreter").toString();
  device_path_ = settings_.value("coordinator/devicePath", "").toString();
  device_name_ = settings_.value("coordinator/deviceName", "").toString();
#else
  coordinator_path_ = settings_.value("coordinator/path", "/usr/local/bin/ypspur-coordinator").toString();
  interpreter_path_ = settings_.value("interpreter/path", "/usr/local/bin/ypspur-interpreter").toString();
  device_path_ = settings_.value("coordinator/devicePath", "/dev/").toString();
  device_name_ = settings_.value("coordinator/deviceName", "ttyACM*").toString();
#endif
  ui_->coordinatorPath->setText(coordinator_path_);
  ui_->interpreterPath->setText(interpreter_path_);

  coordinator_options_ = settings_.value("coordinator/options", "").toString();
  interpreter_options_ = settings_.value("interpreter/options", "").toString();
  ui_->coordinatorOptions->setText(coordinator_options_);
  ui_->interpreterOptions->setText(interpreter_options_);

  port_ = settings_.value("coordinator/port", "/dev/ttyACM0").toString();
  if (!port_.isEmpty())
    ui_->portList->addItem(port_);

  param_file_ = settings_.value("coordinator/param", "").toString();
  QString paramName = param_file_;
  paramName.replace(QRegExp("^.*([^/\\\\]*)$"), "\\1");
  if (!param_file_.isEmpty())
    ui_->parameterName->setText(paramName);

  connect(&coordinator_, SIGNAL(readyReadStandardError()), this, SLOT(updateCoordinatorError()));
  connect(&coordinator_, SIGNAL(readyReadStandardOutput()), this, SLOT(updateCoordinatorText()));
  connect(&coordinator_, SIGNAL(finished(int)), this, SLOT(coordinatorQuit(int)));
  connect(&interpreter_, SIGNAL(readyReadStandardError()), this, SLOT(updateInterpreterError()));
  connect(&interpreter_, SIGNAL(readyReadStandardOutput()), this, SLOT(updateInterpreterText()));
  connect(&interpreter_, SIGNAL(finished(int)), this, SLOT(interpreterQuit(int)));

  ui_->portList->installEventFilter(this);
}

YPSpurGUI::~YPSpurGUI()
{
  settings_.sync();
  delete ui_;
}

void YPSpurGUI::on_coordinatorStart_toggled(bool checked)
{
  if (checked)
  {
    mutex_coordinator_output_.lock();
    printTextEdit(ui_->coordinatorOut, "<hr style=\"width:100%;background-color:#CCCCCC;height:1pt;\">");
    mutex_coordinator_output_.unlock();

    ui_->parameterBrowse->setDisabled(true);
    ui_->portList->setDisabled(true);

    QStringList args;
    args.append("--device");
    args.append(port_);
    if (!param_file_.isEmpty() && param_file_ != "embedded")
    {
      args.append("--param");
      args.append(param_file_);
    }
    args.append("--update-param");
    QStringList options = coordinator_options_.split(QRegExp("\\s+"), QString::SkipEmptyParts);
    if (!options.isEmpty())
      args.append(options);

    coordinator_.start(coordinator_path_, args);
  }
  else
  {
    if (interpreter_.state() != QProcess::NotRunning)
      interpreter_.close();
    if (coordinator_.state() != QProcess::NotRunning)
      coordinator_.close();
    ui_->parameterBrowse->setDisabled(false);
    ui_->portList->setDisabled(false);
  }
}

void YPSpurGUI::coordinatorQuit(int exitCode)
{
  printTextEdit(ui_->coordinatorOut, "<br><i>Exit code: " + QString().number(exitCode) + "</i><br>");
  ui_->coordinatorStart->setChecked(false);
}

void YPSpurGUI::interpreterQuit(int exitCode)
{
  printTextEdit(ui_->interpreterOut, "<br><i>Exit code: " + QString().number(exitCode) + "</i><br>");
}

void YPSpurGUI::updateCoordinatorError()
{
  mutex_coordinator_output_.lock();
  QString data(coordinator_.readAllStandardError());

  data.replace(QRegExp(" "), "&nbsp;");
  data.replace(QRegExp("\n"), "<br>");
  printTextEdit(ui_->coordinatorOut, "<b>" + data + "</b>");
  mutex_coordinator_output_.unlock();
  if (data.contains(QRegExp("Command&nbsp;analy[zs]er&nbsp;started\\.")))
  {
    mutex_interpreter_output_.lock();
    printTextEdit(ui_->interpreterOut, "<hr style=\"width:100%;background-color:#CCCCCC;height:1pt;\">");
    mutex_interpreter_output_.unlock();

    QStringList options = interpreter_options_.split(QRegExp("\\s+"), QString::SkipEmptyParts);
    if (!options.isEmpty())
      interpreter_.start(interpreter_path_, options);
    else
      interpreter_.start(interpreter_path_);
  }
}

void YPSpurGUI::updateCoordinatorText()
{
  mutex_coordinator_output_.lock();
  QString data(coordinator_.readAllStandardOutput());

  data.replace(QRegExp(" "), "&nbsp;");
  data.replace(QRegExp("\n"), "<br>");
  printTextEdit(ui_->coordinatorOut, data);
  mutex_coordinator_output_.unlock();
}

void YPSpurGUI::updateInterpreterError()
{
  mutex_interpreter_output_.lock();
  QString data(interpreter_.readAllStandardError());

  data.replace(QRegExp(" "), "&nbsp;");
  data.replace(QRegExp("\n"), "<br>");
  printTextEdit(ui_->interpreterOut, "<b>" + data + "</b>");
  mutex_interpreter_output_.unlock();
}

void YPSpurGUI::updateInterpreterText()
{
  mutex_interpreter_output_.lock();
  QString data(interpreter_.readAllStandardOutput());

  data.replace(QRegExp(" "), "&nbsp;");
  data.replace(QRegExp("\n"), "<br>");
  printTextEdit(ui_->interpreterOut, data);
  mutex_interpreter_output_.unlock();
}

void YPSpurGUI::on_parameterBrowse_clicked()
{
  QString defaultPath("./");
  if (!param_file_.isEmpty())
  {
    defaultPath = param_file_;
  }
  QString fileName = QFileDialog::getOpenFileName(
      this,
      QString("Open Image"),
      defaultPath,
      QString("Parameter files (*.param)"));
  if (!fileName.isEmpty())
  {
    setParamFile(fileName);
  }
}

void YPSpurGUI::setParamFile(QString fileName)
{
  param_file_ = fileName;

  QString paramName = fileName;
  paramName.replace(QRegExp("^.*([^/\\\\]*)$"), "\\1");
  ui_->parameterName->setText(paramName);

  settings_.setValue("coordinator/param", param_file_);
}

#if QT_VERSION >= 0x050000
void YPSpurGUI::on_portList_currentTextChanged(const QString& arg1)
#else
void YPSpurGUI::on_portList_textChanged(const QString& arg1)
#endif
{
  if (!arg1.isNull())
  {
    QStringList options = arg1.split(" ", QString::SkipEmptyParts);
    if (options.size() > 0)
    {
      port_ = options[0];
      settings_.setValue("coordinator/port", port_);
    }
  }
}

void YPSpurGUI::on_interpreterCommand_returnPressed()
{
  QString str = ui_->interpreterCommand->text();
  str.append("\n");
  interpreter_.write(str.toLocal8Bit());
  ui_->interpreterCommand->setText("");
}

bool YPSpurGUI::eventFilter(QObject* obj, QEvent* event)
{
  if (event->type() == QEvent::MouseButtonPress)
  {
#ifdef _WIN32
    HDEVINFO devInfo = SetupDiGetClassDevs(
        NULL, NULL, NULL, DIGCF_PRESENT | DIGCF_ALLCLASSES);
    TCHAR name[128];
    TCHAR className[128];
    TCHAR com[128];
    SP_DEVINFO_DATA devData;
    devData.cbSize = sizeof(SP_DEVINFO_DATA);

    ui_->portList->clear();
    for (int i = 0; SetupDiEnumDeviceInfo(devInfo, i, &devData); i++)
    {
      SetupDiGetDeviceRegistryProperty(
          devInfo, &devData, SPDRP_CLASS, NULL, (BYTE*)className, sizeof(className), NULL);
      if (_tcscmp(className, _T("Ports")) != 0)
        continue;

      SetupDiGetDeviceRegistryProperty(
          devInfo, &devData, SPDRP_FRIENDLYNAME, NULL, (BYTE*)name, sizeof(name), NULL);
      HKEY key = SetupDiOpenDevRegKey(devInfo, &devData, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_QUERY_VALUE);
      if (key)
      {
        DWORD len = sizeof(com);
        RegQueryValueEx(key, _T("PortName"), NULL, NULL, (BYTE*)com, &len);
        QString qname = QString::fromWCharArray(name);
        qname.replace(QRegExp("\\s*\\(COM[0-9]+\\)\\s*$"), "");
        ui_->portList->addItem("\\\\.\\" + QString::fromWCharArray(com) + " " + qname);
      }
    }
#else
    QDir dir(device_path_, device_name_);
    dir.setFilter(QDir::AllEntries | QDir::System);
    dir.makeAbsolute();

    QFileInfoList list = dir.entryInfoList();
    ui_->portList->clear();
    for (int i = 0; i < list.count(); i++)
    {
      ui_->portList->addItem(list[i].absoluteFilePath());
    }
#endif
  }
  fflush(stderr);
  return QObject::eventFilter(obj, event);
}

void YPSpurGUI::on_coordinatorDefaultParam_clicked()
{
  setParamFile("embedded");
}

void YPSpurGUI::on_coordinatorPath_textChanged(const QString& arg1)
{
  coordinator_path_ = arg1;
  settings_.setValue("coordinator/path", arg1);
}

void YPSpurGUI::on_coordinatorOptions_textChanged(const QString& arg1)
{
  coordinator_options_ = arg1;
  settings_.setValue("coordinator/options", arg1);
}

void YPSpurGUI::on_interpreterPath_textChanged(const QString& arg1)
{
  interpreter_path_ = arg1;
  settings_.setValue("interpreter/path", arg1);
}

void YPSpurGUI::on_interpreterOptions_textChanged(const QString& arg1)
{
  interpreter_options_ = arg1;
  settings_.setValue("interpreter/options", arg1);
}
