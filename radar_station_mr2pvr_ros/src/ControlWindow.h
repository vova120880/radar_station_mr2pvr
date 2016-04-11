// ======================================================================
//  Copyright (c) 2016 by Vladimir Bespalov
//
// ======================================================================

#ifndef CONTROL_WINDOW_H
#define CONTROL_WINDOW_H

#include <QtWidgets>
#include <QPushButton>
#include <QString>
#include <QVBoxLayout>
//#include <QHBoxLayout>
//#include <QLabel>
//#include <QTextStream>
#include <QLineEdit>
//#include <QPalette>
//#include <QIcon>

#include <QTextEdit>

#include "RobotThread.h"
#include "rscontroller/radarstationcontroller.h"

namespace server{

class ControlWindow : public QWidget
{
    Q_OBJECT

public:
    ControlWindow(int argc, char** argv, QWidget * parent = 0);

private:
    Q_SLOT void clearLog();
    Q_SLOT void connect2rs();
    Q_SLOT void addLog(QString);
    Q_SLOT void connected();

    Q_SLOT void handleRlmUdpDatagramm();
    Q_SLOT void handleRpUdpDatagramm();
    Q_SLOT void handleVrliUdpDatagramm();
    Q_SLOT void handlePrliUdpDatagramm();

    QTextEdit *p_LogDisplay;
    QLineEdit* ipAdress;

    QVBoxLayout *mainLayout;
    QPushButton *clearButton;

    QPushButton *connectButton;

    RobotThread m_RobotThread;

    RadarStationController* rsContr;

};//end class ControlWindow
}//end namespace
#endif

