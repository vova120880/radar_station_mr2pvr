// ======================================================================
//  Copyright (c) 2016 by Vladimir Bespalov
//
// ======================================================================

#ifndef ___ROBOTTHREAD_H___
#define ___ROBOTTHREAD_H___

#include <QtCore>
#include <QThread>
#include <QString>
#include <QStringList>
#include <stdlib.h>
#include <QMutex>
#include <iostream>
#include "assert.h"

#include <ros/ros.h>
#include <ros/network.h>

#include <diagnostic_msgs/KeyValue.h>
#include <diagnostic_msgs/DiagnosticStatus.h>
#include <diagnostic_updater/DiagnosticStatusWrapper.h>
#include <diagnostic_updater/diagnostic_updater.h>

//----------------------------------
#include "rscontroller/radarstationcontroller.h"

#include "radar_station_mr2pvr/RadarTarget.h"
#include "radar_station_mr2pvr/RadarRawLine.h"
#include "radar_station_mr2pvr/NavINS.h"

#include "radar_station_mr2pvr/RlmCtrlCmd.h"
#include "radar_station_mr2pvr/RpCtrlCmd.h"
#include "radar_station_mr2pvr/VrliCtrlCmd.h"

class RobotThread : public QObject {
	Q_OBJECT
public:
    RobotThread(int argc, char **pArgv, const char * topic  = "/odom");
    virtual ~RobotThread();

    bool init();

    void setRSController(RadarStationController* contr) { rsContr = contr; }

    void diagnosticRlmCallBack(const diagnostic_msgs::KeyValue& msg);
    void diagnosticRpCallBack(const diagnostic_msgs::KeyValue& msg);
    void diagnosticVrliCallBack(const diagnostic_msgs::KeyValue& msg);

    void diagnosticVrliCallBack(const radar_station_mr2pvr::NavINS& msg);

    Q_SLOT void run();

    //---------------------------------------
    // обработчики датаграмм протоколов
    //---------------------------------------
    Q_SLOT void handleRlmUdpDatagramm();
    Q_SLOT void handleRpUdpDatagramm();
    Q_SLOT void handleVrliUdpDatagramm();
    Q_SLOT void handlePrliUdpDatagramm();

    Q_SLOT void connect2radar();

    Q_SIGNAL void logMsg(QString);

    void produce_diagnostics(diagnostic_updater::DiagnosticStatusWrapper &stat);

protected:
    bool rlmCtrlCmdHandler(radar_station_mr2pvr::RlmCtrlCmd::Request  &req,
                 radar_station_mr2pvr::RlmCtrlCmd::Response &res);
    bool rpCtrlCmdHandler(radar_station_mr2pvr::RpCtrlCmd::Request  &req,
                 radar_station_mr2pvr::RpCtrlCmd::Response &res);
    bool vrliCtrlCmdHandler(radar_station_mr2pvr::VrliCtrlCmd::Request  &req,
                 radar_station_mr2pvr::VrliCtrlCmd::Response &res);

private:
    int m_Init_argc;
    char** m_pInit_argv;
    const char * m_topic;

    QThread * m_pThread;

    //-------------------
    // топики управления
    //-------------------
    ros::Subscriber rlm_listener;
    ros::Subscriber rp_listener;
    ros::Subscriber vrli_listener;
    ros::Subscriber nmea_listener;

    ros::Publisher diagnostic_pub;

    ros::Publisher rlm_pub;
    ros::Publisher rp_pub;

    ros::Publisher rlm_ctrl_pub;
    ros::Publisher rp_ctrl_pub;
    ros::Publisher vrli_ctrl_pub;

    ros::Publisher vrli_pub;
    ros::Publisher prli_pub;

    ros::ServiceServer rlm_service;
    ros::ServiceServer rp_service;
    ros::ServiceServer vrli_service;

    //------------------
    diagnostic_updater::Updater* updater;

    RadarStationController* rsContr;
};
#endif

