// ======================================================================
//  Copyright (c) 2016 by Vladimir Bespalov
// ======================================================================

#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
class UdpSocketThread;
class TcpSocketThread;

class QTcpServer;
class QTcpSocket;

class RadarStationEmul;
class RSContr;
QT_END_NAMESPACE

#include "./../radarStationPrototype/rscontroller/rssockets/udpsocket.h"

#include <QtCore/QThread>
class RSContr : public QThread
{
    Q_OBJECT

public:
    RSContr(QObject *parent = 0);

    ~RSContr();

protected:
    void run();


signals:
    void logMessage(const QString& str);
    void parsePriFile(const QString& str);
    void parseTargetsFile(const QString& str);

protected:

};

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

protected slots:

    void connectUdpClicked();
    void stopBtnClicked();

    void connectTcpClicked();

    void slotNewConnectionRlm();
    void slotNewConnectionRp();
    void slotNewConnectionVrli();
    void slotNewConnectionNmea();

    void slotReadClientRlm();
    void slotReadClientRp();
    void slotReadClientVrli();
    void slotReadClientNmea();


    void handleUdpDatagramm(QByteArray*);

    void logMessage(const QString&);

    void timerEvent(QTimerEvent *event);

    //-----------------------
    void mdspButtonClicked();
    void mspButtonClicked();
    void targetsButtonClicked();
    void priButtonClicked();

    void readPriFromFile();
    void readTargetsFromFile();

    // ПРЛИ
    void compressionAlgorithm2PolyBitLine(const QByteArray& in, QByteArray& out);
    void decompressionAlgorithm2PolyBitLine(const QByteArray& in, QByteArray& out);
    void testAlgorithm2PolyBitLine();

signals:
    void writeData2Udp(QByteArray*);

    void parsePriFile(const QString& str);
    void parseTargetsFile(const QString& str);

protected:
    void connectSignals2Slots();

    void initUI();
    void connectUISignals2Slots();
    void startSockets();

    void newTcpConnection();
    void readDataFromTcpSocket(QTcpSocket* socket);

private:
    Ui::Dialog *ui;

    UdpSocketThread* udpSocketThread;
    TcpSocketThread* tcpSocketThread;

    QTcpServer* m_ptcpServerRlm;
    QTcpSocket* pClientSocketRlm;

    QTcpServer* m_ptcpServerRp;
    QTcpSocket* pClientSocketRp;

    QTcpServer* m_ptcpServerVrli;
    QTcpSocket* pClientSocketVrli;

    QTcpServer* m_ptcpServerNmea;
    QTcpSocket* pClientSocketNmea;

    RadarStationEmul* radarStation;

    RSContr* rsContr;

    UdpSocketThread* rlmSocket;
    UdpSocketThread* rpSocket;
    UdpSocketThread* prliSocket;
    UdpSocketThread* vrliSocket;

    bool isRunning;
    int timerId;

    UdpSocket nmeaSocket;
};

#endif // DIALOG_H
