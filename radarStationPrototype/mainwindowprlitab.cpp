// ======================================================================
//  Copyright (c) 2016 by Vladimir Bespalov
// ======================================================================

#include "mainwindow.h"
#include "ui_mainwindow.h"

// prli
void MainWindow::prliClearLogBtnClicked()
{
    ui->prliLog->clear();
}

void MainWindow::prliSaveLogBtnClicked()
{
    saveLog("prliLog", ui->prliLog);
}
