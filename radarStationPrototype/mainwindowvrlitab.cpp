// ======================================================================
//  Copyright (c) 2016 by Vladimir Bespalov
// ======================================================================

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "./rscontroller/radarstationcontroller.h"
#include "./rscontroller/rsmodel/radarstationmodel.h"
#include "./rscontroller/rsprotocols/radarstationprotocols.h"

// vrli
void MainWindow::vrliTgtManualBtnClicked()
{
    RadarStationModel* rsModel = rsContr->getRSModel();
    rsModel->writeVrliCmd(VrliCtrlLst::TGT_MANUAL_ALL, ui->vrliTgtManualVal->text());
}

void MainWindow::vrliTgtResetBtnClicked()
{
    RadarStationModel* rsModel = rsContr->getRSModel();
    rsModel->writeVrliCmd(VrliCtrlLst::TGT_RESET, ui->vrliTgtResetVal->text());
}

void MainWindow::vrliTgtResetLostBtnClicked()
{
    RadarStationModel* rsModel = rsContr->getRSModel();
    rsModel->writeVrliCmd(VrliCtrlLst::TGT_RESET_LOST, "");
}

void MainWindow::vrliTgtAutoCaptureOnBtnClicked()
{
    RadarStationModel* rsModel = rsContr->getRSModel();
    rsModel->writeVrliCmd(VrliCtrlLst::AUTO_CAPTURE_ON, ui->vrliAutoOnVal->text());
}

void MainWindow::vrliTgtAutoCaptureOffBtnClicked()
{
    RadarStationModel* rsModel = rsContr->getRSModel();
    rsModel->writeVrliCmd(VrliCtrlLst::AUTO_CAPTURE_OFF, "");
}

void MainWindow::vrliClearLogBtnClicked()
{
    ui->vrliLog->clear();
}

void MainWindow::vrliSaveLogBtnClicked()
{
    saveLog("vrliLog", ui->vrliLog);
}
