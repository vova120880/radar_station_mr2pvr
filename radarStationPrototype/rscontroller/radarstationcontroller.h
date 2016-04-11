// ======================================================================
//  Copyright (c) 2016 by Vladimir Bespalov
// контроллер РЛС
// ======================================================================

#ifndef RADARSTATIONCONTROLLER_H
#define RADARSTATIONCONTROLLER_H

#include <QObject>
#include <QtCore/qthread.h>

QT_BEGIN_NAMESPACE
class RadarStationModel;
class RsProtocolWorker;
QT_END_NAMESPACE

class RadarStationController : public QObject
{
    Q_OBJECT
public:
    explicit RadarStationController(QObject *parent = 0);
    ~RadarStationController();

    RadarStationModel* getRSModel() { return rsModel; }

public slots:

    void connect2radar();
    void disconnect();

protected slots:

signals:
    void logMessage(const QString& msg);
    void connected();

    void setRlmStatePort(quint16 inPort);
    void setRlmCtrlHost(const QString& host, quint16 port);

    void setRpStatePort(quint16 inPort);
    void setRpCtrlHost(const QString& host, quint16 port);

    void setVrliStatePort(quint16 inPort);
    void setVrliCtrlHost(const QString& host, quint16 port);

    void setPrliStatePort(quint16 inPort);

    void setNmeaCtrlHost(const QString& host, quint16 port);


protected:

    void createWorkers();

    void connectWorkers2Model();
    void connectRlmWorker2Model();
    void connectRpWorker2Model();
    void connectVrliWorker2Model();
    void connectPrliWorker2Model();
    void connectNmeaWorker2Model();

    void startWorkerThreads();
    void stopWorkerThreads();

private:

    // потоки для работы с протоколами
    QThread rlmWorkerThread;
    QThread rpWorkerThread;
    QThread vrliWorkerThread;
    QThread prliWorkerThread;
    QThread nmeaWorkerThread;


    //
    RsProtocolWorker* rlmWorker;
    RsProtocolWorker* rpWorker;
    RsProtocolWorker* vrliWorker;
    RsProtocolWorker* prliWorker;
    RsProtocolWorker* nmeaWorker;

    // модель РЛС
    RadarStationModel* rsModel;

};

#endif // RADARSTATIONCONTROLLER_H
