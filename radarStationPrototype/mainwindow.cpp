// ======================================================================
//  Copyright (c) 2016 by Vladimir Bespalov
// ======================================================================

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFile>
#include <QTextStream>
#include <QDataStream>
#include <QDateTime>

#include <QScrollBar>

#include "./../common/TcpSocketThread.h"

#include "./rscontroller/radarstationcontroller.h"
#include "./rscontroller/rsmodel/radarstationmodel.h"
#include "./rscontroller/rsprotocols/radarstationprotocols.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    showMaximized();
    setWindowTitle(QString::fromUtf8("ПО связи с РЛС \"МР-2ПВ-Р\" 1.10"));

    rsContr = new RadarStationController(this);

    QObject::connect(rsContr,   SIGNAL(logMessage(const QString&)),
                     this,		SLOT(logMessage(const QString&)));

    QObject::connect(rsContr,   SIGNAL(connected()),
                     this,      SLOT(connected()));

    QObject::connect(rsContr->getRSModel(), SIGNAL(rlmStateChanged()),
                     this,      SLOT(handleRlmUdpDatagramm()));

    QObject::connect(rsContr->getRSModel(), SIGNAL(rpStateChanged()),
                     this,      SLOT(handleRpUdpDatagramm()));

    QObject::connect(rsContr->getRSModel(), SIGNAL(vrliStateChanged()),
                     this,      SLOT(handleVrliUdpDatagramm()));

    QObject::connect(rsContr->getRSModel(), SIGNAL(prliStateChanged()),
                     this,      SLOT(handlePrliUdpDatagramm()));

    initUIGroups();
    connectSlots2Signals();
}

//------------------

MainWindow::~MainWindow()
{
    delete ui;
}

//---------------------------------------
// обработчики датаграмм протоколов
//---------------------------------------
void MainWindow::handleRlmUdpDatagramm()
{
    QByteArray datagram;
    datagram = rsContr->getRSModel()->getRlmRaw();

    QDateTime date = QDateTime::currentDateTime();
    ui->rlmLog->append(date.toString("hh-mm-ss:"));
    ui->rlmLog->append(datagram);
}

void MainWindow::handleRpUdpDatagramm()
{
    QByteArray datagram;
    datagram = rsContr->getRSModel()->getRpRaw();

    QDateTime date = QDateTime::currentDateTime();
    ui->rpLog->append(date.toString("hh-mm-ss:"));
    ui->rpLog->append(datagram);

    //ui->rpLog->verticalScrollBar()->setValue(
    //            ui->rpLog->verticalScrollBar()->maximum());

}

void MainWindow::handleVrliUdpDatagramm()
{
    QByteArray datagram;
    datagram = rsContr->getRSModel()->getVrliRaw();

    QDateTime date = QDateTime::currentDateTime();
    QString msg(date.toString("hh-mm-ss"));
    ui->vrliLog->append(QString("received %1 , sz: %2").arg(msg).arg(datagram.size()));

    if(ui->vrliSavePackets->isChecked())
    {
        QDateTime date = QDateTime::currentDateTime();
        QString str = date.toString("vrli_yyMMdd_hhmmss") + ".dat";

        QFile file(str);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            ui->vrliLog->append("error open file: " + str);
            return;
        }

        file.write(datagram);
        ui->vrliLog->append("binary saved in the: " + str);
    }

    if(!ui->vrliParsePackets->isChecked())
    {
        return;
    }

    QDataStream stream(datagram);
    stream.setVersion(QDataStream::Qt_5_4);
    stream.setByteOrder(QDataStream::LittleEndian);

    TargetMsgFormat header;
    TargetForm target;
    TargetMsgCrcFormat crc;

    while(!stream.atEnd())
    {
        stream >> header;
        // прочитать цели:
        int targetsCount((header.sz - 20)/23); // 20б - размер хедера, 23б - 1 цель.
        for(int i = 0; i < targetsCount; ++i)
        {
            stream >> target;

            //targetLogMessage(header, target);
            char timeStamp[7];
            memcpy(timeStamp, header.timeStamp, 6);
            timeStamp[6] = 0;

            ui->vrliLog->append(
                QString("h = %1, sz = %2, time = %3 : "
                        "num = %4 , speed = %5 , status = %6 , course = %7"
                        )
                        .arg(header.header)
                        .arg(header.sz)
                        .arg(timeStamp)
                        .arg(target.number)
                        .arg(target.speed)
                        .arg(target.status)
                        .arg(target.course));

            //QThread::msleep(5);
        }

        stream >> crc.checkSum1;
        stream >> crc.checkSum2;

        // проверить crc
        /*
        QByteArray datagram;
        QDataStream stream2(&datagram, QIODevice::WriteOnly);
        stream2 << header;
        stream2 << target;

        quint8 checkSum1 = FletchSum((const quint8*)datagram.constData(), datagram.size());
        stream2 << checkSum1;
        quint8 checkSum2 = FletchSum((const quint8*)datagram.constData(), datagram.size());
        */

    }
}

void MainWindow::handlePrliUdpDatagramm()
{
    QByteArray datagram;
    datagram = rsContr->getRSModel()->getPrliRaw();

    QDateTime date = QDateTime::currentDateTime();
    QString msg(date.toString("hh-mm-ss"));
    ui->prliLog->append(QString("received: %1 , sz: %2").arg(msg).arg(datagram.size()));

    if(ui->prliSavePackets->isChecked())
    {
        QDateTime date = QDateTime::currentDateTime();
        QString str = date.toString("prli_yyMMdd_hhmmss") + ".dat";

        QFile file(str);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            ui->prliLog->append("error open file: " + str);
            return;
        }

        file.write(datagram);
        ui->prliLog->append("binary saved in the: " + str);
    }

    if(!ui->prliParsePackets->isChecked())
    {
        return;
    }

    // разобрать прли
    QDataStream stream(datagram);
    stream.setVersion(QDataStream::Qt_5_4);
    stream.setByteOrder(QDataStream::LittleEndian);

    PrimaryRadarInformationHeader header;
    PrimaryRadarInformationData data;

    QByteArray in;

    while(!stream.atEnd())
    {
        stream >> header;
        stream >> data;

        in.resize(data.sz - 14);
        stream.readRawData(in.data(), data.sz - 14);

        QByteArray out;
        //Algorithm2PolyBitLine::decompression(in, out);

        ui->prliLog->append(
            QString("h = %1, lon = %2, lat = %3 : "
                    "sz = %4 , num = %5 , range = %6 , t1 = %7, "
                    "out.sz = %8")
                    .arg(header.header)
                    .arg(header.longitude)
                    .arg(header.latitude)
                    .arg(data.sz - 14)
                    .arg(data.number)
                    .arg(data.range)
                    .arg(data.t1)
                    .arg(out.size()));

    }

}

void MainWindow::logMessage(const QString& str)
{
    ui->conLog->append(str);
}

void MainWindow::saveLog(const QString& prefix, QTextEdit* log)
{
    QDateTime date = QDateTime::currentDateTime();
    QString str = prefix + date.toString("_yyMMdd_hhmmss") + ".dat";

    QFile file(str);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        log->append("error open file: " + str);
        return;
    }

    QTextStream out(&file);
    out << log->toPlainText();
    log->append("saved in the: " + str);
}

void MainWindow::setRlmCmd(QLineEdit* edit, const QString& val)
{
    RadarStationModel* rsModel = rsContr->getRSModel();
    rsModel->writeRlmCmd(val, edit->text());

}

void MainWindow::setRpCmd(QLineEdit* edit, const QString& val)
{
    RadarStationModel* rsModel = rsContr->getRSModel();
    rsModel->writeRpCmd(val, edit->text());
}

void MainWindow::connected()
{
    ui->conLog->append("Connect - OK");
}

// v 1.03
void MainWindow::initUIGroups()
{
    initConnectionSettingsUIGroup();
}

void MainWindow::connectSlots2Signals()
{
    connectUISlots2Signals();
}

void MainWindow::connectUISlots2Signals()
{
    // con
    QObject::connect(ui->connectBtn, SIGNAL(clicked()),
                     this, SLOT(conConnectBtnClicked()));
    QObject::connect(ui->conLogClearBtn, SIGNAL(clicked()),
                     this, SLOT(conClearLogBtnClicked()));
    QObject::connect(ui->conLogSaveBtn, SIGNAL(clicked()),
                     this, SLOT(conSaveLogBtnClicked()));

    // rlm
    QObject::connect(ui->rlmSetBtn, SIGNAL(clicked()),
                     this, SLOT(rlmSetParamsBtnClicked()));
    QObject::connect(ui->rlmGetBtn, SIGNAL(clicked()),
                     this, SLOT(rlmGetParamsClicked()));
    QObject::connect(ui->rlmLogClearBtn, SIGNAL(clicked()),
                     this, SLOT(rlmClearLogBtnClicked()));
    QObject::connect(ui->rlmLogSaveBtn, SIGNAL(clicked()),
                     this, SLOT(rlmSaveLogBtnClicked()));

    // rp
    QObject::connect(ui->rpSetBtn, SIGNAL(clicked()),
                     this, SLOT(rpSetParamsBtnClicked()));
    QObject::connect(ui->rpGetBtn, SIGNAL(clicked()),
                     this, SLOT(rpGetParamsClicked()));
    QObject::connect(ui->rpLogClearBtn, SIGNAL(clicked()),
                     this, SLOT(rpClearLogBtnClicked()));
    QObject::connect(ui->rpLogSaveBtn, SIGNAL(clicked()),
                     this, SLOT(rpSaveLogBtnClicked()));

    // vrli
    QObject::connect(ui->vrliTgtManualBtn, SIGNAL(clicked()),
                     this, SLOT(vrliTgtManualBtnClicked()));
    QObject::connect(ui->vrliTgtResetBtn, SIGNAL(clicked()),
                     this, SLOT(vrliTgtResetBtnClicked()));
    QObject::connect(ui->vrliTgtResetLostBtn, SIGNAL(clicked()),
                     this, SLOT(vrliTgtResetLostBtnClicked()));
    QObject::connect(ui->vrliAutoOnBtn, SIGNAL(clicked()),
                     this, SLOT(vrliTgtAutoCaptureOnBtnClicked()));
    QObject::connect(ui->vrliAutoOffBtn, SIGNAL(clicked()),
                     this, SLOT(vrliTgtAutoCaptureOffBtnClicked()));

    QObject::connect(ui->vrliLogClearBtn, SIGNAL(clicked()),
                     this, SLOT(vrliClearLogBtnClicked()));
    QObject::connect(ui->vrliLogSaveBtn, SIGNAL(clicked()),
                     this, SLOT(vrliSaveLogBtnClicked()));

    // prli
    QObject::connect(ui->prliLogClearBtn, SIGNAL(clicked()),
                     this, SLOT(prliClearLogBtnClicked()));
    QObject::connect(ui->prliLogSaveBtn, SIGNAL(clicked()),
                     this, SLOT(prliSaveLogBtnClicked()));
}

void MainWindow::initConnectionSettingsUIGroup()
{
    const RadarStationModel* rsModel = rsContr->getRSModel();

    ui->serverIp->setText(rsModel->getHost());

    quint16 statePort, ctrlPort;

    rsModel->getNmeaConnectionParams(ctrlPort);
    ui->nmeaCtrlPort->setValue(ctrlPort);

    rsModel->getVrliConnectionParams(statePort, ctrlPort);
    ui->vrliStatePort->setValue(statePort);
    ui->vrliCtrlPort->setValue(ctrlPort);

    rsModel->getPrliConnectionParams(statePort);
    ui->prliStatePort->setValue(statePort);

    rsModel->getRlmConnectionParams(statePort, ctrlPort);
    ui->rlmStatePort->setValue(statePort);
    ui->rlmCtrlPort->setValue(ctrlPort);

    rsModel->getRpConnectionParams(statePort, ctrlPort);
    ui->rpStatePort->setValue(statePort);
    ui->rpCtrlPort->setValue(ctrlPort);
}

