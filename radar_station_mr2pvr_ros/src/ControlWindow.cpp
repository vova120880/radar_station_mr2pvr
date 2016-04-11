// ======================================================================
//  Copyright (c) 2016 by Vladimir Bespalov
//
// ======================================================================

#include "ControlWindow.h"

#include "./rscontroller/rsmodel/radarstationmodel.h"

namespace server{

ControlWindow::ControlWindow(int argc, char **argv, QWidget *parent)
    : QWidget(parent)
    , m_RobotThread(argc, argv)
{
    rsContr = new RadarStationController(this);
    QObject::connect(rsContr,  SIGNAL(logMessage(const QString&)),
                     this,	   SLOT(addLog(const QString)));


    /** Set up the Controls **/
    clearButton = new QPushButton("clear");
    connectButton = new QPushButton("connect");
    p_LogDisplay = new QTextEdit();
    ipAdress = new QLineEdit();
    ipAdress->setText("127.0.0.1");

    mainLayout = new QVBoxLayout();
    mainLayout->addWidget(p_LogDisplay);
    mainLayout->addWidget(clearButton);
    mainLayout->addWidget(ipAdress);
    mainLayout->addWidget(connectButton);
    setLayout(mainLayout);

    show();

    setWindowTitle(tr("Control Window"));

    connect(clearButton,  &QPushButton::clicked, this, &ControlWindow::clearLog);
    connect(connectButton,  &QPushButton::clicked, this, &ControlWindow::connect2rs);

    connect(&m_RobotThread, &RobotThread::logMsg, this, &ControlWindow::addLog);

    m_RobotThread.setRSController(rsContr);

    connect(rsContr->getRSModel(), SIGNAL(rlmStateChanged()),
                    this,      SLOT(handleRlmUdpDatagramm()));

    connect(rsContr->getRSModel(), &RadarStationModel::rpStateChanged,
                     this,      &ControlWindow::handleRpUdpDatagramm);

    connect(rsContr->getRSModel(), SIGNAL(vrliStateChanged()),
                     this,      SLOT(handleVrliUdpDatagramm()));

    connect(rsContr->getRSModel(), SIGNAL(prliStateChanged()),
                     this,      SLOT(handlePrliUdpDatagramm()));

    QObject::connect(rsContr,   SIGNAL(connected()),
                         this,      SLOT(connected()));

    m_RobotThread.init();

}//end constructor

void ControlWindow::connected()
{
    p_LogDisplay->append("Connect - OK");
}

void ControlWindow::clearLog()
{
    p_LogDisplay->clear();
}

void ControlWindow::connect2rs()
{
    m_RobotThread.connect2radar();
    // изменить модель
    //RadarStationModel* rsModel = rsContr->getRSModel();

    //rsModel->setHost(ipAdress->text());
    /*
    rsModel->setRlmConnectionParams(ui->rlmStatePort->value(), ui->rlmCtrlPort->value());
    rsModel->setRpConnectionParams(ui->rpStatePort->value(), ui->rpCtrlPort->value());
    rsModel->setVrliConnectionParams(ui->vrliStatePort->value(), ui->vrliCtrlPort->value());
    rsModel->setPrliConnectionParams(ui->prliStatePort->value());
    rsModel->setNmeaConnectionParams(ui->nmeaCtrlPort->value());
    */
    // подключиться
    //rsContr->connect2radar();

}

void ControlWindow::addLog(QString msg)
{
    p_LogDisplay->append(msg);
}

void ControlWindow::handleRlmUdpDatagramm()
{
    m_RobotThread.handleRlmUdpDatagramm();
}

void ControlWindow::handleRpUdpDatagramm()
{
    m_RobotThread.handleRpUdpDatagramm();
}

void ControlWindow::handleVrliUdpDatagramm()
{
    m_RobotThread.handleVrliUdpDatagramm();
}

void ControlWindow::handlePrliUdpDatagramm()
{
    m_RobotThread.handlePrliUdpDatagramm();
}

}//end namespace

