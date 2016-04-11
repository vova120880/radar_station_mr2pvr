// ======================================================================
//  Copyright (c) 2016 by Vladimir Bespalov
// ======================================================================

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
class RadarStationController;
//class RadarStationModel;
class QTextEdit;
class QLineEdit;
QT_END_NAMESPACE

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected slots:

    void logMessage(const QString&);
    void connected();

    // v 1.03

    //---------------------------------------
    // UI
    //---------------------------------------

    // соединение с сервером
    void conConnectBtnClicked();
    void conClearLogBtnClicked();
    void conSaveLogBtnClicked();

    // rlm
    void rlmSetParamsBtnClicked();
    void rlmGetParamsClicked();
    void rlmClearLogBtnClicked();
    void rlmSaveLogBtnClicked();

    // rp
    void rpSetParamsBtnClicked();
    void rpGetParamsClicked();
    void rpClearLogBtnClicked();
    void rpSaveLogBtnClicked();

    // vrli
    void vrliTgtManualBtnClicked();
    void vrliTgtResetBtnClicked();
    void vrliTgtResetLostBtnClicked();
    void vrliTgtAutoCaptureOnBtnClicked();
    void vrliTgtAutoCaptureOffBtnClicked();

    void vrliClearLogBtnClicked();
    void vrliSaveLogBtnClicked();

    // prli
    void prliClearLogBtnClicked();
    void prliSaveLogBtnClicked();

    //---------------------------------------
    // обработчики датаграмм протоколов
    //---------------------------------------
    void handleRlmUdpDatagramm();
    void handleRpUdpDatagramm();
    void handleVrliUdpDatagramm();
    void handlePrliUdpDatagramm();

    // slots 2 signals
    void connectSlots2Signals();
    void connectUISlots2Signals();

protected:

    // v 1.03

    void initUIGroups();
    void initConnectionSettingsUIGroup();

    //
    //--------------------------------
    void saveLog(const QString& prefix, QTextEdit* log);

    void setRlmCmd(QLineEdit* edit, const QString& val);
    void setRpCmd(QLineEdit* edit, const QString& val);


signals:
    void connect2Host(const QString& host, quint16 port);
    void writePacket(QByteArray packet);

    // v 1.03
    void writeRlmCmd(QByteArray* packet);
    void writeRpCmd(QByteArray* packet);
    void writeVrliCmd(QByteArray* packet);


private:
    Ui::MainWindow *ui;

    RadarStationController* rsContr;

};

#endif // MAINWINDOW_H
