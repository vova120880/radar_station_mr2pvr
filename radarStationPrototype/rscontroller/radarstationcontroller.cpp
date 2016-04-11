// ======================================================================
//  Copyright (c) 2016 by Vladimir Bespalov
// ======================================================================

#include "radarstationcontroller.h"
#include "./rsworkers/rsprotocolworker.h"
#include "./rsmodel/radarstationmodel.h"

RadarStationController::RadarStationController(QObject *parent)
    : QObject(parent)
{
    rsModel = new RadarStationModel();

    createWorkers();
    connectWorkers2Model();
    startWorkerThreads();
}

void RadarStationController::createWorkers()
{
    rlmWorker = new RsProtocolWorker();
    rlmWorker->moveToThread(&rlmWorkerThread);

    rpWorker = new RsProtocolWorker();
    rpWorker->moveToThread(&rpWorkerThread);

    vrliWorker = new RsProtocolWorker();
    vrliWorker->moveToThread(&vrliWorkerThread);

    prliWorker = new RsProtocolWorker();
    prliWorker->moveToThread(&prliWorkerThread);

    nmeaWorker = new RsProtocolWorker();
    nmeaWorker->moveToThread(&nmeaWorkerThread);
}

void RadarStationController::connectWorkers2Model()
{
    connectRlmWorker2Model();
    connectRpWorker2Model();
    connectVrliWorker2Model();
    connectPrliWorker2Model();
}

void RadarStationController::connectRlmWorker2Model()
{
    QObject::connect(rlmWorker,	SIGNAL(statePacketReceived(QByteArray*)),
                     rsModel,	SLOT(updateRlmRaw(QByteArray*)));

    QObject::connect(rsModel,	SIGNAL(writeRlmCtrlPacket(QByteArray*)),
                     rlmWorker,	SIGNAL(writeCtrlPacket(QByteArray*)));

    QObject::connect(this,	SIGNAL(setRlmStatePort(quint16)),
                     rlmWorker,	SIGNAL(setStatePort(quint16)));
    QObject::connect(this,	SIGNAL(setRlmCtrlHost(const QString&, quint16)),
                     rlmWorker,	SIGNAL(setCtrlHost(const QString&, quint16)));

    QObject::connect(rlmWorker,	SIGNAL(logMessage(const QString&)),
                     this,		SIGNAL(logMessage(const QString&)));

    QObject::connect(rlmWorker, SIGNAL(connected()),
                     this,      SIGNAL(connected()));
}

void RadarStationController::connectRpWorker2Model()
{
    QObject::connect(rpWorker,	SIGNAL(statePacketReceived(QByteArray*)),
                     rsModel,	SLOT(updateRpRaw(QByteArray*)));

    QObject::connect(rsModel,	SIGNAL(writeRpCtrlPacket(QByteArray*)),
                     rpWorker,	SIGNAL(writeCtrlPacket(QByteArray*)));

    QObject::connect(this,	SIGNAL(setRpStatePort(quint16)),
                     rpWorker,	SIGNAL(setStatePort(quint16)));
    QObject::connect(this,	SIGNAL(setRpCtrlHost(const QString&, quint16)),
                     rpWorker,	SIGNAL(setCtrlHost(const QString&, quint16)));

    QObject::connect(rpWorker,	SIGNAL(logMessage(const QString&)),
                     this,		SIGNAL(logMessage(const QString&)));

    QObject::connect(rpWorker, SIGNAL(connected()),
                     this,      SIGNAL(connected()));
}

void RadarStationController::connectVrliWorker2Model()
{
    QObject::connect(vrliWorker,	SIGNAL(statePacketReceived(QByteArray*)),
                     rsModel,	SLOT(updateVrliRaw(QByteArray*)));

    QObject::connect(rsModel,	SIGNAL(writeVrliCtrlPacket(QByteArray*)),
                     vrliWorker,	SIGNAL(writeCtrlPacket(QByteArray*)));

    QObject::connect(this,	SIGNAL(setVrliStatePort(quint16)),
                     vrliWorker,	SIGNAL(setStatePort(quint16)));
    QObject::connect(this,	SIGNAL(setVrliCtrlHost(const QString&, quint16)),
                     vrliWorker,	SIGNAL(setCtrlHost(const QString&, quint16)));

    QObject::connect(vrliWorker,	SIGNAL(logMessage(const QString&)),
                     this,		SIGNAL(logMessage(const QString&)));

    QObject::connect(vrliWorker, SIGNAL(connected()),
                     this,      SIGNAL(connected()));
}

void RadarStationController::connectPrliWorker2Model()
{
    QObject::connect(prliWorker,	SIGNAL(statePacketReceived(QByteArray*)),
                     rsModel,	SLOT(updatePrliRaw(QByteArray*)));

    QObject::connect(this,	SIGNAL(setPrliStatePort(quint16)),
                     prliWorker,	SIGNAL(setStatePort(quint16)));

    QObject::connect(prliWorker,	SIGNAL(logMessage(const QString&)),
                     this,		SIGNAL(logMessage(const QString&)));

    QObject::connect(prliWorker, SIGNAL(connected()),
                     this,      SIGNAL(connected()));
}

void RadarStationController::connectNmeaWorker2Model()
{
    QObject::connect(rsModel,	SIGNAL(writeNmeaCtrlPacket(QByteArray*)),
                     nmeaWorker,	SIGNAL(writeCtrlPacket(QByteArray*)));

    QObject::connect(this,	SIGNAL(setNmeaCtrlHost(const QString&, quint16)),
                     nmeaWorker,	SIGNAL(setCtrlHost(const QString&, quint16)));

    QObject::connect(nmeaWorker,	SIGNAL(logMessage(const QString&)),
                     this,		SIGNAL(logMessage(const QString&)));

    QObject::connect(nmeaWorker, SIGNAL(connected()),
                     this,      SIGNAL(connected()));
}

void RadarStationController::startWorkerThreads()
{
    // запустить потоки обработки протоколов
    rlmWorkerThread.start();
    rpWorkerThread.start();
    vrliWorkerThread.start();
    prliWorkerThread.start();
    nmeaWorkerThread.start();
}

RadarStationController::~RadarStationController()
{
    stopWorkerThreads();
}

void RadarStationController::stopWorkerThreads()
{
    // остановить потоки обработки протоколов
    rlmWorkerThread.quit();
    rlmWorkerThread.wait();

    rpWorkerThread.quit();
    rpWorkerThread.wait();

    vrliWorkerThread.quit();
    vrliWorkerThread.wait();

    prliWorkerThread.quit();
    prliWorkerThread.wait();

    nmeaWorkerThread.quit();
    nmeaWorkerThread.wait();
}


void RadarStationController::connect2radar()
{
    quint16 statePort, ctrlPort;

    rsModel->getRlmConnectionParams(statePort, ctrlPort);
    emit setRlmStatePort(statePort);
    emit setRlmCtrlHost(rsModel->getHost(), ctrlPort);

    rsModel->getRpConnectionParams(statePort, ctrlPort);
    emit setRpStatePort(statePort);
    emit setRpCtrlHost(rsModel->getHost(), ctrlPort);

    rsModel->getVrliConnectionParams(statePort, ctrlPort);
    emit setVrliStatePort(statePort);
    emit setVrliCtrlHost(rsModel->getHost(), ctrlPort);

    rsModel->getPrliConnectionParams(statePort);
    emit setPrliStatePort(statePort);

    rsModel->getNmeaConnectionParams(ctrlPort);
    emit setNmeaCtrlHost(rsModel->getHost(), ctrlPort);
}

void RadarStationController::disconnect()
{

}
