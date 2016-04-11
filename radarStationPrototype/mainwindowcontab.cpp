// ======================================================================
//  Copyright (c) 2016 by Vladimir Bespalov
// ======================================================================

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "./rscontroller/radarstationcontroller.h"
#include "./rscontroller/rsmodel/radarstationmodel.h"

// соединение с сервером
void MainWindow::conConnectBtnClicked()
{
    // изменить модель
    RadarStationModel* rsModel = rsContr->getRSModel();
    rsModel->setHost(ui->serverIp->text());
    rsModel->setRlmConnectionParams(ui->rlmStatePort->value(), ui->rlmCtrlPort->value());
    rsModel->setRpConnectionParams(ui->rpStatePort->value(), ui->rpCtrlPort->value());
    rsModel->setVrliConnectionParams(ui->vrliStatePort->value(), ui->vrliCtrlPort->value());
    rsModel->setPrliConnectionParams(ui->prliStatePort->value());
    rsModel->setNmeaConnectionParams(ui->nmeaCtrlPort->value());

    // подключиться
    rsContr->connect2radar();

}

void MainWindow::conClearLogBtnClicked()
{
    ui->conLog->clear();
}

void MainWindow::conSaveLogBtnClicked()
{
    saveLog("conLog", ui->conLog);
}
