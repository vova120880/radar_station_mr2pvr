// ======================================================================
//  Copyright (c) 2016 by Vladimir Bespalov
// ======================================================================

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "./rscontroller/radarstationcontroller.h"
#include "./rscontroller/rsmodel/radarstationmodel.h"
#include "./rscontroller/rsprotocols/radarstationprotocols.h"

// rp
void MainWindow::rpSetParamsBtnClicked()
{
    setRpCmd(ui->rpRainVal, RpCtrlLst::RAIN);
    setRpCmd(ui->rpLfFilterVal, RpCtrlLst::LFFILTERVALUE);
    setRpCmd(ui->rpInterferenceVal, RpCtrlLst::SYNCINTERFERENCEMODE);
    setRpCmd(ui->rpScaleVal, RpCtrlLst::SCALE);
    setRpCmd(ui->rpGainVal, RpCtrlLst::GAIN);
    setRpCmd(ui->rpSeaVal, RpCtrlLst::SEA);
    setRpCmd(ui->rpSeaDistVal, RpCtrlLst::SEA_DIST);
}

void MainWindow::rpGetParamsClicked()
{
    RadarStationModel* rsModel = rsContr->getRSModel();

    ui->rpRainVal->setText(rsModel->getRpCmdValue(RpStateLst::RAIN));
    ui->rpLfFilterVal->setText(rsModel->getRpCmdValue(RpStateLst::LFFILTERVALUE));
    ui->rpInterferenceVal->setText(rsModel->getRpCmdValue(RpStateLst::SYNCINTERFERENCEMODE));

    // этого параметра нет в состоянии, только в управлении: передавать всегда???
    // ui->rpScaleVal->setText(rsModel->getRpCmdValue(QString(mdcplst::SCALE.c_str())));

    ui->rpGainVal->setText(rsModel->getRpCmdValue(RpStateLst::GAIN));
    ui->rpSeaVal->setText(rsModel->getRpCmdValue(RpStateLst::SEA));
    ui->rpSeaDistVal->setText(rsModel->getRpCmdValue(RpStateLst::SEA_DIST));
}

void MainWindow::rpClearLogBtnClicked()
{
    ui->rpLog->clear();
}

void MainWindow::rpSaveLogBtnClicked()
{
    saveLog("rpLog", ui->rpLog);
}
