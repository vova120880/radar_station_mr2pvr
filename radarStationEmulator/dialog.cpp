// ======================================================================
//  Copyright (c) 2016 by Vladimir Bespalov
// ======================================================================

#include "dialog.h"
#include "ui_dialog.h"

#include <QByteArray>
#include <QFile>

#include <QtNetwork/qtcpserver.h>
#include <QtNetwork/qtcpsocket.h>

#include "./../common/UdpSocketThread.h"
//#include "./../common/TcpSocketThread.h"

//#include "./../radarStationPrototype/radarstationprotocols.h"

#include "radarstationemul.h"

RSContr::RSContr(QObject *parent/* = 0*/)
        : QThread(parent)
    {
    }

RSContr::~RSContr()
{
}

void RSContr::run()
{
    RadarStationEmul* rs = new RadarStationEmul();

    connect(rs, SIGNAL(logMessage(const QString&)),
            this, SIGNAL(logMessage(const QString&)));

    connect(this, SIGNAL(parsePriFile(const QString&)),
            rs, SLOT(parsePriFile(const QString&)));

    connect(this, SIGNAL(parseTargetsFile(const QString&)),
            rs, SLOT(parseTargetsFile(const QString&)));

    exec();
    delete rs;
}

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
    , m_ptcpServerRlm(0)
    , m_ptcpServerRp(0)
    , m_ptcpServerVrli(0)
    , m_ptcpServerNmea(0)
    , isRunning(false)
    , timerId(0)
{
    ui->setupUi(this);
    setWindowTitle(QString::fromUtf8("ПО \"эмулятор РЛС МР-2ПВ-Р\" 1.11"));

    ui->PathPriEdit->setText("e:\\Nelax\\ros_sln\\downloads\\data\\"
                             "real_dat\\prli_160310_135039.dat");
                          //"radar_protocol_data_25Nov15\\prli_test_circle.txt");

    //e:\Nelax\ros_sln\downloads\data\real_dat\prli_160310_135044.dat

    ui->PathTargetsEdit->setText("e:\\Nelax\\ros_sln\\downloads\\data\\"
                          "radar_protocol_data_25Nov15\\goals2.txt");

    ui->IpTcpEdit->setText("127.000.000.001");
    ui->PortTcpEdit->setText("2000");

    QObject::connect(ui->ConnectUdpButton, SIGNAL(clicked()),
                     this, SLOT(connectUdpClicked()));

    QObject::connect(ui->stopButton, SIGNAL(clicked()),
                     this, SLOT(stopBtnClicked()));

    QObject::connect(ui->ConnectTcpButton, SIGNAL(clicked()),
                     this, SLOT(connectTcpClicked()));

    //---------------------------------------------------------------------------
    //udpSocketThread = new UdpSocketThread(this);
    //udpSocketThread->start();
/*
    QObject::connect(udpSocketThread,	SIGNAL(datagramSended()),
                    this,				SLOT(commandPacketSended()));

    QObject::connect(udpSocketThread,	SIGNAL(datagramReceived(QByteArray*)),
                    this,				SLOT(handleUdpDatagramm(QByteArray*)));

    QObject::connect(udpSocketThread,	SIGNAL(logMessage(const QString&)),
                    this,				SLOT(logMessage(const QString&)));

    QObject::connect(
                this,	SIGNAL(writeData2Udp(QByteArray*)),
                udpSocketThread,	SIGNAL(writeData(QByteArray*)));
*/
    //tcpSocketThread = new TcpSocketThread(this);
    //tcpSocketThread->start();

    connectSignals2Slots();

    radarStation = new RadarStationEmul(this);
    QObject::connect(radarStation,	SIGNAL(logMessage(const QString&)),
                    this,			SLOT(logMessage(const QString&)));


    RSContr* rsContr = new RSContr(this);
    rsContr->start();
    QObject::connect(rsContr,	SIGNAL(logMessage(const QString&)),
                    this,   	SLOT(logMessage(const QString&)));

    QObject::connect(this,      SIGNAL(parsePriFile(const QString&)),
                    rsContr,  	SIGNAL(parsePriFile(const QString&)));
    QObject::connect(this,      SIGNAL(parseTargetsFile(const QString&)),
                    rsContr,  	SIGNAL(parseTargetsFile(const QString&)));

    initUI();
    connectUISignals2Slots();
    startSockets();

    //-----------------

}

void Dialog::initUI()
{
    ui->RlmStatePortEdit->setText("10002");
    ui->RlmCtrlPortEdit->setText("10003");

    ui->RpStatePortEdit->setText("10004");
    ui->RpCtrlPortEdit->setText("10005");

    ui->VrliStatePortEdit->setText("10000");
    ui->VrliCtrlPortEdit->setText("10000");

    ui->PrliStatePortEdit->setText("10001");

    ui->NmeaCtrlPortEdit->setText("10007");
}

void Dialog::connectUISignals2Slots()
{

}

void Dialog::startSockets()
{
    rlmSocket = new UdpSocketThread(this);
    rlmSocket->start();
    rpSocket = new UdpSocketThread(this);
    rpSocket->start();
    prliSocket = new UdpSocketThread(this);
    prliSocket->start();
    vrliSocket = new UdpSocketThread(this);
    vrliSocket->start();
}

void Dialog::connectSignals2Slots()
{

    QObject::connect(ui->mdspButton, SIGNAL(clicked()),
                     this, SLOT(mdspButtonClicked()));
    QObject::connect(ui->mspButton, SIGNAL(clicked()),
                     this, SLOT(mspButtonClicked()));
    QObject::connect(ui->targetsButton, SIGNAL(clicked()),
                     this, SLOT(targetsButtonClicked()));
    QObject::connect(ui->priButton, SIGNAL(clicked()),
                     this, SLOT(priButtonClicked()));

    QObject::connect(ui->ReadFromFileButton, SIGNAL(clicked()),
                     this, SLOT(readPriFromFile()));

    QObject::connect(ui->ReadTargetsFromFileButton, SIGNAL(clicked()),
                     this, SLOT(readTargetsFromFile()));
}


void Dialog::timerEvent(QTimerEvent* /*event*/)
{
    //ui->LogEdit->append("tick");

    QByteArray* rpdata = radarStation->buildModeDataStateProtocolDatagram();
    rpSocket->sendDatagram(rpdata);

    QByteArray* rlmdata = radarStation->buildModeStateProtocolDatagram();
    rlmSocket->sendDatagram(rlmdata);

    QByteArray* vrlidata = radarStation->buildTargetsProtocolDatagram();
    vrliSocket->sendDatagram(vrlidata);

    QByteArray* prlidata = radarStation->buildPRIProtocolDatagram();
    prliSocket->sendDatagram(prlidata);
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::logMessage(const QString& str)
{
    ui->LogEdit->append(str);
}

void Dialog::connectUdpClicked()
{
    rlmSocket->slotBind(0,//ui->RlmStatePortEdit->text().toInt(),
                        ui->RlmStatePortEdit->text().toInt());
    rpSocket->slotBind(0,//ui->RpStatePortEdit->text().toInt(),
                       ui->RpStatePortEdit->text().toInt());
    prliSocket->slotBind(0,//ui->PrliStatePortEdit->text().toInt(),
                         ui->PrliStatePortEdit->text().toInt());
    vrliSocket->slotBind(0,//ui->VrliStatePortEdit->text().toInt(),
                         ui->VrliStatePortEdit->text().toInt());

    isRunning = true;
    timerId = startTimer(1000);
}

void Dialog::stopBtnClicked()
{
    isRunning = false;
    killTimer(timerId);
    timerId = 0;
}

void Dialog::connectTcpClicked()
{
    // rlm
    if(!m_ptcpServerRlm)
    {
        m_ptcpServerRlm = new QTcpServer(this);
        connect(m_ptcpServerRlm, SIGNAL(newConnection()), this, SLOT(slotNewConnectionRlm()));
    }

    if(!m_ptcpServerRlm->listen(QHostAddress::Any, ui->RlmCtrlPortEdit->text().toInt()))
    {
        ui->LogEdit->append("Unable to start the server:" + m_ptcpServerRlm->errorString());
        m_ptcpServerRlm->close();
        //return;
    }
    else
        ui->LogEdit->append("Server started : " + ui->RlmCtrlPortEdit->text());

    // rp
    if(!m_ptcpServerRp)
    {
        m_ptcpServerRp = new QTcpServer(this);
        connect(m_ptcpServerRp, SIGNAL(newConnection()), this, SLOT(slotNewConnectionRp()));
    }

    if(!m_ptcpServerRp->listen(QHostAddress::Any, ui->RpCtrlPortEdit->text().toInt()))
    {
        ui->LogEdit->append("Unable to start the server:" + m_ptcpServerRp->errorString());
        m_ptcpServerRp->close();
        //return;
    }
    else
        ui->LogEdit->append("Server started : " + ui->RpCtrlPortEdit->text());

    // vlri
    if(!m_ptcpServerVrli)
    {
        m_ptcpServerVrli = new QTcpServer(this);
        connect(m_ptcpServerVrli, SIGNAL(newConnection()), this, SLOT(slotNewConnectionVrli()));
    }

    if(!m_ptcpServerVrli->listen(QHostAddress::Any, ui->VrliCtrlPortEdit->text().toInt()))
    {
        ui->LogEdit->append("Unable to start the server:" + m_ptcpServerVrli->errorString());
        m_ptcpServerVrli->close();
        //return;
    }
    else
        ui->LogEdit->append("Server started : " + ui->VrliCtrlPortEdit->text());

    // nmea
    // vlri
    if(!m_ptcpServerNmea)
    {
        m_ptcpServerNmea = new QTcpServer(this);
        connect(m_ptcpServerNmea, SIGNAL(newConnection()), this, SLOT(slotNewConnectionNmea()));
    }

    if(!m_ptcpServerNmea->listen(QHostAddress::Any, ui->NmeaCtrlPortEdit->text().toInt()))
    {
        ui->LogEdit->append("Unable to start the server:" + m_ptcpServerNmea->errorString());
        m_ptcpServerNmea->close();
        //return;
    }
    else
        ui->LogEdit->append("Server started : " + ui->NmeaCtrlPortEdit->text());

}

void Dialog::newTcpConnection()
{

}

void Dialog::readDataFromTcpSocket(QTcpSocket* socket)
{
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_5_4);

    if(socket->bytesAvailable() <= 0)
        return;

    int sz = socket->bytesAvailable();

    QByteArray* ba = new QByteArray();
    ba->resize(sz);
    in.readRawData(ba->data(), sz);

    QString cmd("");
    for(int i = 0; i < ba->size(); ++i)
    {
        if((*ba)[i])
            cmd += (*ba)[i];
        else
        {
            ui->LogEdit->append(cmd);
            cmd = "";
        }
    }
    delete ba;
}

void Dialog::slotNewConnectionRlm()
{
    QTcpSocket* pClientSocket = m_ptcpServerRlm->nextPendingConnection();
    connect(pClientSocket, SIGNAL(disconnected()),
            pClientSocket, SLOT(deleteLater())
           );
    connect(pClientSocket, SIGNAL(readyRead()),
            this,          SLOT(slotReadClientRlm())
           );

    ui->LogEdit->append("Server Rlm Response: Connected!");
}

void Dialog::slotReadClientRlm()
{
    ui->LogEdit->append("Rlm cmd:");
    readDataFromTcpSocket((QTcpSocket*)sender());
    /*
    pClientSocketRlm = (QTcpSocket*)sender();
    QDataStream in(pClientSocketRlm);
    in.setVersion(QDataStream::Qt_5_4);

    if(pClientSocketRlm->bytesAvailable() <= 0)
        return;

    int sz = pClientSocketRlm->bytesAvailable();

    QByteArray* ba = new QByteArray();
    ba->resize(sz);
    in.readRawData(ba->data(), sz);

    QString cmd("");
    for(int i = 0; i < ba->size(); ++i)
    {
        if((*ba)[i])
            cmd += (*ba)[i];
        else
        {
            ui->LogEdit->append(cmd);
            cmd = "";
        }
    }
    //ui->LogEdit->append(*ba);
    delete ba;
    */
}

void Dialog::slotNewConnectionRp()
{
    QTcpSocket* pClientSocket = m_ptcpServerRp->nextPendingConnection();
    connect(pClientSocket, SIGNAL(disconnected()),
            pClientSocket, SLOT(deleteLater())
           );
    connect(pClientSocket, SIGNAL(readyRead()),
            this,          SLOT(slotReadClientRp())
           );

    ui->LogEdit->append("Server Rp Response: Connected!");
}

void Dialog::slotReadClientRp()
{
    ui->LogEdit->append("Rp cmd:");
    readDataFromTcpSocket((QTcpSocket*)sender());
    /*
    QTcpSocket* pClientSocket = (QTcpSocket*)sender();
    QDataStream in(pClientSocket);
    in.setVersion(QDataStream::Qt_5_4);

    if(pClientSocket->bytesAvailable() <= 0)
        return;

    int sz = pClientSocket->bytesAvailable();

    QByteArray* ba = new QByteArray();
    ba->resize(sz);
    in.readRawData(ba->data(), sz);

    QString cmd("");
    for(int i = 0; i < ba->size(); ++i)
    {
        if((*ba)[i])
            cmd += (*ba)[i];
        else
        {
            ui->LogEdit->append(cmd);
            cmd = "";
        }
    }
    //ui->LogEdit->append(*ba);
    delete ba;
    */
}

void Dialog::slotNewConnectionVrli()
{
    QTcpSocket* pClientSocket = m_ptcpServerVrli->nextPendingConnection();
    connect(pClientSocket, SIGNAL(disconnected()),
            pClientSocket, SLOT(deleteLater())
           );
    connect(pClientSocket, SIGNAL(readyRead()),
            this,          SLOT(slotReadClientVrli())
           );

    ui->LogEdit->append("Server Rlm Response: Connected!");
}

void Dialog::slotReadClientVrli()
{
    ui->LogEdit->append("Vrli cmd:");
    readDataFromTcpSocket((QTcpSocket*)sender());
    /*
    QTcpSocket* pClientSocket = (QTcpSocket*)sender();
    QDataStream in(pClientSocket);
    in.setVersion(QDataStream::Qt_5_4);

    if(pClientSocket->bytesAvailable() <= 0)
        return;

    int sz = pClientSocket->bytesAvailable();

    QByteArray* ba = new QByteArray();
    ba->resize(sz);
    in.readRawData(ba->data(), sz);

    QString cmd("");
    for(int i = 0; i < ba->size(); ++i)
    {
        if((*ba)[i])
            cmd += (*ba)[i];
        else
        {
            ui->LogEdit->append(cmd);
            cmd = "";
        }
    }
    //ui->LogEdit->append(*ba);
    delete ba;
    */
}

void Dialog::slotNewConnectionNmea()
{
    QTcpSocket* pClientSocket = m_ptcpServerNmea->nextPendingConnection();
    connect(pClientSocket, SIGNAL(disconnected()),
            pClientSocket, SLOT(deleteLater())
           );
    connect(pClientSocket, SIGNAL(readyRead()),
            this,          SLOT(slotReadClientNmea())
           );

    ui->LogEdit->append("Server Nmea Response: Connected!");
}

void Dialog::slotReadClientNmea()
{
    ui->LogEdit->append("Nmea cmd:");

    QTcpSocket* pClientSocket = (QTcpSocket*)sender();
    QDataStream in(pClientSocket);
    in.setVersion(QDataStream::Qt_5_4);

    if(pClientSocket->bytesAvailable() <= 0)
        return;

    int sz = pClientSocket->bytesAvailable();

    QByteArray* ba = new QByteArray();
    ba->resize(sz);
    in.readRawData(ba->data(), sz);

    QString cmd("");
    for(int i = 0; i < ba->size(); ++i)
    {
        if((*ba)[i])
            cmd += (*ba)[i];
        else
        {
            ui->LogEdit->append(cmd);
            cmd = "";
        }
    }
    //ui->LogEdit->append(*ba);
    delete ba;
}

void Dialog::handleUdpDatagramm(QByteArray* datagram)
{

}

// Протокол передачи состояния режимов обработки данных от РЛС
void Dialog::mdspButtonClicked()
{
    QByteArray* data = radarStation->buildModeDataStateProtocolDatagram();
    emit writeData2Udp(data);
}

// Протокол передачи состояния РЛС
void Dialog::mspButtonClicked()
{
    QByteArray* data = radarStation->buildModeStateProtocolDatagram();
    emit writeData2Udp(data);
}

void Dialog::targetsButtonClicked()
{
    QByteArray* datagram = radarStation->buildTargetsProtocolDatagram();

    emit writeData2Udp(datagram);

    ui->LogEdit->append(QString("targets datagram: %1")
                        .arg(datagram->size()));
}

void Dialog::priButtonClicked()
{
    //testAlgorithm2PolyBitLine();

    QByteArray* datagram = radarStation->buildPRIProtocolDatagram();

    emit writeData2Udp(datagram);

    ui->LogEdit->append(QString("datagram: %1 : in: %2 : out: %3")
                        .arg(datagram->size())
                        .arg(radarStation->szIn)
                        .arg(radarStation->szOut));

    // РЛС выполняет 20 оборотов в минуту и на каждом обороте выдает 4095 линеек,
    // т.е. 4095*20/60 = 1365 линеек в секунду.


}

void Dialog::testAlgorithm2PolyBitLine()
{
    size_t szz(10000);
    QByteArray in, out1, out2;
    Algorithm2PolyBitLine::generatePRID(in, szz);
    compressionAlgorithm2PolyBitLine(in, out1);
    decompressionAlgorithm2PolyBitLine(out1, out2);

    ui->LogEdit->append(
                QString("in: %1 , out1: %2 , out2: %3")
                .arg(in.size())
                .arg(out1.size())
                .arg(out2.size()));

/*
    ui->LogEdit->append("=================");
    for(size_t i = 0; i < in.size(); ++i)
    {
        ui->LogEdit->append( QString("[%1]  in: %2")
                             .arg(i, 3, 10)
                             .arg(quint8(in[i]), 0, 16)
                    );
    }
    ui->LogEdit->append("=================");
*/

    for(size_t i = 0; i < szz; ++i)
    {
        if(in[i] != out2[i])
        {
            ui->LogEdit->append(
                        QString("[%3]  in: %1 , out2: %2 :    out1: %4 ")
                        .arg(quint8(in[i]), 0, 16)
                        .arg(quint8(out2[i]), 0, 16)
                        .arg(i, 3, 10)
                        .arg(quint8(out1[i]), 0, 16));
        }
    }
}

void Dialog::compressionAlgorithm2PolyBitLine(const QByteArray& in, QByteArray& out)
{
    Algorithm2PolyBitLine::compression(in, out);
}

void Dialog::decompressionAlgorithm2PolyBitLine(const QByteArray& in, QByteArray& out)
{
    Algorithm2PolyBitLine::decompression(in, out);
}

/* e:\Nelax\ros_sln\downloads\data\radar_protocol_data_25Nov15\ */

void Dialog::readPriFromFile()
{
    QString path = ui->PathPriEdit->text();
    emit parsePriFile(path);
}

void Dialog::readTargetsFromFile()
{
    QString path = ui->PathTargetsEdit->text();
    emit parseTargetsFile(path);
}
