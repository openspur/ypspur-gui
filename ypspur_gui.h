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

namespace Ui
{
class YPSpurGUI;
}

class YPSpurGUI : public QMainWindow
{
  Q_OBJECT

public:
  explicit YPSpurGUI(QWidget* parent = 0);
  ~YPSpurGUI();
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
  void on_portList_currentTextChanged(const QString& arg1);
  void on_interpreterCommand_returnPressed();
  void on_coordinatorDefaultParam_clicked();
  void on_coordinatorPath_textChanged(const QString& arg1);
  void on_coordinatorOptions_textChanged(const QString& arg1);
  void on_interpreterPath_textChanged(const QString& arg1);
  void on_interpreterOptions_textChanged(const QString& arg1);

private:
  bool eventFilter(QObject* obj, QEvent* event);

  Ui::YPSpurGUI* ui_;

  QProcess interpreter_;
  QProcess coordinator_;
  QMutex mutex_interpreter_output_;
  QMutex mutex_coordinator_output_;

  QString param_file_;
  QString port_;
  QString coordinator_path_;
  QString interpreter_path_;
  QString coordinator_options_;
  QString interpreter_options_;
  QString device_path_;
  QString device_name_;

  QSettings settings_;
};

#endif  // YPSPUR_GUI_H
