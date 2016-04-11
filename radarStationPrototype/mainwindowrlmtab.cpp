// ======================================================================
//  Copyright (c) 2016 by Vladimir Bespalov
// ======================================================================

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "./rscontroller/radarstationcontroller.h"
#include "./rscontroller/rsmodel/radarstationmodel.h"
#include "./rscontroller/rsprotocols/radarstationprotocols.h"

// rlm
void MainWindow::rlmSetParamsBtnClicked()
{
    // сравнить ui  с моделью записать отличающиеся
    setRlmCmd(ui->rlmPowerVal, RlmCtrlLst::POWER);
    setRlmCmd(ui->rlmAntenaVal, RlmCtrlLst::ANTENA);
    setRlmCmd(ui->rlmWorkVal, RlmCtrlLst::WORK);
    setRlmCmd(ui->rlmPulseVal, RlmCtrlLst::PULSE);
    setRlmCmd(ui->rlmGainVal, RlmCtrlLst::GAIN);
    setRlmCmd(ui->rlmSeaAmplVal, RlmCtrlLst::SEA_AMPL);
    setRlmCmd(ui->rlmSeaDistVal, RlmCtrlLst::SEA_DIST);
}

void MainWindow::rlmGetParamsClicked()
{
    RadarStationModel* rsModel = rsContr->getRSModel();
    // заполнить контролы ui актуальными из модели
    ui->rlmPowerVal->setText(rsModel->getRlmCmdValue(RlmStateLst::POWER));
    ui->rlmAntenaVal->setText(rsModel->getRlmCmdValue(RlmStateLst::ANTENA));
    ui->rlmWorkVal->setText(rsModel->getRlmCmdValue(RlmStateLst::WORK));
    ui->rlmPulseVal->setText(rsModel->getRlmCmdValue(RlmStateLst::PULSE));
    ui->rlmGainVal->setText(rsModel->getRlmCmdValue(RlmStateLst::GAIN));
    ui->rlmSeaAmplVal->setText(rsModel->getRlmCmdValue(RlmStateLst::SEA_AMPL));
    ui->rlmSeaDistVal->setText(rsModel->getRlmCmdValue(RlmStateLst::SEA_DIST));
}

void MainWindow::rlmClearLogBtnClicked()
{
    ui->rlmLog->clear();
}

void MainWindow::rlmSaveLogBtnClicked()
{
    saveLog("rlmLog", ui->rlmLog);
}
