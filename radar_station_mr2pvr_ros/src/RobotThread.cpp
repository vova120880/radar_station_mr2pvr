// ======================================================================
//  Copyright (c) 2016 by Vladimir Bespalov
//
// ======================================================================

#include "RobotThread.h"
#include "./rscontroller/rsmodel/radarstationmodel.h"
#include "./rscontroller/rsprotocols/radarstationprotocols.h"

RobotThread::RobotThread(int argc, char** pArgv, const char * topic)
    :	m_Init_argc(argc),
        m_pInit_argv(pArgv),
        m_topic(topic)
{/** Constructor for the robot thread **/}

RobotThread::~RobotThread()
{
    ROS_INFO("exit ROS~");
    if (ros::isStarted())
    {
        ros::shutdown();
        ros::waitForShutdown();
    }//end if

    m_pThread->quit();
    m_pThread->wait();

    ROS_INFO("exit ROS~");
}//end destructor

bool RobotThread::init()
{
    m_pThread = new QThread();
    this->moveToThread(m_pThread);

    connect(m_pThread, &QThread::started, this, &RobotThread::run);
    ros::init(m_Init_argc, m_pInit_argv, "radar_station");

    ROS_INFO("ros::init");

    if (!ros::master::check())
        return false;//do not start without ros.

    ros::start();
    ROS_INFO("ros::start");
    ros::Time::init();
    ros::NodeHandle nh;

    diagnostic_pub = nh.advertise<diagnostic_msgs::DiagnosticStatus>("radar_station/diagnostics", 1000);
    prli_pub = nh.advertise<radar_station_mr2pvr::RadarRawLine>("radar_station/radar_lines", 1000);
    vrli_pub = nh.advertise<radar_station_mr2pvr::RadarTarget>("radar_station/radar_targets", 1000);

    //rp_ctrl_pub = nh.advertise<diagnostic_msgs::KeyValue>("radar/radar_settings", 1000);
    //rlm_ctrl_pub = nh.advertise<diagnostic_msgs::KeyValue>("radar/radar_control", 1000);
    //vrli_ctrl_pub = nh.advertise<diagnostic_msgs::KeyValue>("radar/radar_targets_control", 1000);
    //rlm_pub = nh.advertise<diagnostic_msgs::DiagnosticStatus>("radar/diagnostics", 1000);
    //rp_pub = nh.advertise<diagnostic_msgs::DiagnosticStatus>("radar/diagnostics", 1000);

    rp_listener = nh.subscribe("/radar_station/radar_settings", 10, &RobotThread::diagnosticRpCallBack, this);
    rlm_listener = nh.subscribe("/radar_station/radar_control", 10, &RobotThread::diagnosticRlmCallBack, this);
    vrli_listener = nh.subscribe("/radar_station/radar_targets_control", 10, &RobotThread::diagnosticVrliCallBack, this);
    nmea_listener = nh.subscribe("/radar_station/naviINS", 10, &RobotThread::diagnosticVrliCallBack, this);


    rlm_service = nh.advertiseService("rlm_ctrl_cmd", &RobotThread::rlmCtrlCmdHandler, this);
    rp_service = nh.advertiseService("rp_ctrl_cmd", &RobotThread::rpCtrlCmdHandler, this);
    vrli_service = nh.advertiseService("vrli_ctrl_cmd", &RobotThread::vrliCtrlCmdHandler, this);

    updater = new diagnostic_updater::Updater();
    updater->setHardwareID("none");
    updater->add("B21 Status", this, &RobotThread::produce_diagnostics);

    m_pThread->start();
    return true;
}//set up the thread

void RobotThread::connect2radar()
{
   // изменить модель
    RadarStationModel* rsModel = rsContr->getRSModel();

    // чтение параметров
    ros::NodeHandle nh("/radar_station");
    std::string param_val;

    //
    if (nh.hasParam("radar_ip"))
    {
        ROS_INFO("radar_ip present");
        if(nh.getParam("radar_ip", param_val))
             rsModel->setHost(QString(param_val.c_str()));
    }
    else
    {
        ROS_INFO("radar_ip create");
        rsModel->setHost("127.0.0.1");
        nh.setParam("radar_ip", "127.0.0.1");
    }
    ROS_INFO("ip: [%s]", rsModel->getHost().toStdString().c_str());

    int param_state, param_ctrl;
    quint16 statePort, ctrlPort;

    // rlm
    rsModel->getRlmConnectionParams(statePort, ctrlPort);

    if (nh.hasParam("rlm_state_port"))
    {
        if(nh.getParam("rlm_state_port", param_state))
            statePort = param_state;
    }
    else
        nh.setParam("rlm_state_port", statePort);

    if (nh.hasParam("rlm_ctrl_port"))
    {
        if(nh.getParam("rlm_ctrl_port", param_ctrl))
            ctrlPort = param_ctrl;
    }
    else
        nh.setParam("rlm_ctrl_port", ctrlPort);

    rsModel->setRlmConnectionParams(statePort, ctrlPort);
    ROS_INFO("rlm: [%d] [%d]", statePort, ctrlPort);

    // rp
    rsModel->getRpConnectionParams(statePort, ctrlPort);

    if (nh.hasParam("rp_state_port"))
    {
        if(nh.getParam("rp_state_port", param_state))
            statePort = param_state;
    }
    else
        nh.setParam("rp_state_port", statePort);

    if (nh.hasParam("rp_ctrl_port"))
    {
        if(nh.getParam("rp_ctrl_port", param_ctrl))
            ctrlPort = param_ctrl;
    }
    else
        nh.setParam("rp_ctrl_port", ctrlPort);

    rsModel->setRpConnectionParams(statePort, ctrlPort);
    ROS_INFO("rp: [%d] [%d]", statePort, ctrlPort);

    // vrli
    rsModel->getVrliConnectionParams(statePort, ctrlPort);

    if (nh.hasParam("vrli_state_port"))
    {
        if(nh.getParam("vrli_state_port", param_state))
            statePort = param_state;
    }
    else
        nh.setParam("vrli_state_port", statePort);

    if (nh.hasParam("vrli_ctrl_port"))
    {
        if(nh.getParam("vrli_ctrl_port", param_ctrl))
            ctrlPort = param_ctrl;
    }
    else
        nh.setParam("vrli_ctrl_port", ctrlPort);

    rsModel->setVrliConnectionParams(statePort, ctrlPort);
    ROS_INFO("vrli: [%d] [%d]", statePort, ctrlPort);

    // prli
    rsModel->getPrliConnectionParams(statePort);

    if (nh.hasParam("prli_state_port"))
    {
        if(nh.getParam("prli_state_port", param_state))
            statePort = param_state;
    }
    else
        nh.setParam("prli_state_port", statePort);

    rsModel->setPrliConnectionParams(statePort);
    ROS_INFO("prli: [%d]", statePort);

    // nmea
    rsModel->getNmeaConnectionParams(ctrlPort);

    if (nh.hasParam("nmea_ctrl_port"))
    {
        if(nh.getParam("nmea_ctrl_port", param_state))
            statePort = param_state;
    }
    else
        nh.setParam("nmea_ctrl_port", statePort);

    rsModel->setNmeaConnectionParams(ctrlPort);
    ROS_INFO("nmea: [%d]", ctrlPort);

    // подключиться
    rsContr->connect2radar();
}

void RobotThread::produce_diagnostics(diagnostic_updater::DiagnosticStatusWrapper &stat)
{
    /*
    if( driver.getBrakePower() )
           stat.summary(diagnostic_msgs::DiagnosticStatus::OK, "Brake is enabled");
       else
           stat.summary(diagnostic_msgs::DiagnosticStatus::WARN, "Brake is disabled");
    */

    stat.add("RAIN", "0");
    stat.level = 0;
    stat.name = "radar_settings_state";
    stat.hardware_id = "radar";
}

void RobotThread::diagnosticRlmCallBack(const diagnostic_msgs::KeyValue& msg)
{
    RadarStationModel* rsModel = rsContr->getRSModel();
    rsModel->writeRlmCmd(msg.key.c_str(), msg.value.c_str());
    ROS_INFO("rlm: [%s] = [%s]", msg.key.c_str(), msg.value.c_str());
    Q_EMIT logMsg(QString("rlm: [%1] = [%2]").arg(msg.key.c_str()).arg(msg.value.c_str()));
}

void RobotThread::diagnosticRpCallBack(const diagnostic_msgs::KeyValue& msg)
{
    ROS_INFO("rp: [%s] = [%s]", msg.key.c_str(), msg.value.c_str());
    Q_EMIT logMsg(QString("rp: [%1] = [%2]").arg(msg.key.c_str()).arg(msg.value.c_str()));
}

void RobotThread::diagnosticVrliCallBack(const diagnostic_msgs::KeyValue& msg)
{
    ROS_INFO("vrli: [%s] = [%s]", msg.key.c_str(), msg.value.c_str());
    Q_EMIT logMsg(QString("vrli: [%1] = [%2]").arg(msg.key.c_str()).arg(msg.value.c_str()));
}

void RobotThread::run()
{
    ros::Rate loop_rate(100);
    // QMutex * pMutex;
    while (ros::ok())
    {
        //pMutex = new QMutex();
/*
        diagnostic_updater::DiagnosticStatusWrapper diagnosticWrapper;
        //QString("%1").arg(i_).toStdString().c_str();
        diagnosticWrapper.add("RAIN", QString("%1").arg(i_).toStdString().c_str());
        ++i_;
        diagnosticWrapper.level = 0;
        diagnosticWrapper.name = "radar_settings_state";
        diagnosticWrapper.hardware_id = "radar";
*/
        //diagnosticWrapper->update();
        //updater->update();

        //pMutex->lock();
        //pMutex->unlock();

        ros::spinOnce();
        loop_rate.sleep();
        //delete pMutex;
    }//do ros things.

    ROS_INFO("exit ROS");
}

//---------------------------------------
// обработчики датаграмм протоколов
//---------------------------------------
void RobotThread::handleRlmUdpDatagramm()
{
    Q_EMIT logMsg("handleRlmUdpDatagramm");

    diagnostic_msgs::DiagnosticStatus msg;
    msg.level = 0;
    msg.name = "radar_control_state";
    msg.hardware_id = "radar";

    QDateTime date = QDateTime::currentDateTime();
    msg.message = date.toString("hh-mm-ss").toStdString();

    RadarStationModel* rsModel = rsContr->getRSModel();

    diagnostic_msgs::KeyValue val;

    val.key = RlmStateLst::POWER.toStdString();
    val.value = rsModel->getRlmCmdValue(RlmStateLst::POWER).toStdString();
    msg.values.push_back(val);

    val.key = RlmStateLst::ANTENA.toStdString();
    val.value = rsModel->getRlmCmdValue(RlmStateLst::ANTENA).toStdString();
    msg.values.push_back(val);

    val.key = RlmStateLst::WORK.toStdString();
    val.value = rsModel->getRlmCmdValue(RlmStateLst::WORK).toStdString();
    msg.values.push_back(val);

    val.key = RlmStateLst::PULSE.toStdString();
    val.value = rsModel->getRlmCmdValue(RlmStateLst::PULSE).toStdString();
    msg.values.push_back(val);

    val.key = RlmStateLst::GAIN.toStdString();
    val.value = rsModel->getRlmCmdValue(RlmStateLst::GAIN).toStdString();
    msg.values.push_back(val);

    val.key = RlmStateLst::SEA_AMPL.toStdString();
    val.value = rsModel->getRlmCmdValue(RlmStateLst::SEA_AMPL).toStdString();
    msg.values.push_back(val);

    val.key = RlmStateLst::SEA_DIST.toStdString();
    val.value = rsModel->getRlmCmdValue(RlmStateLst::SEA_DIST).toStdString();
    msg.values.push_back(val);

    val.key = RlmStateLst::FAIL_SINCHRONIZATOR.toStdString();
    val.value = rsModel->getRlmCmdValue(RlmStateLst::FAIL_SINCHRONIZATOR).toStdString();
    msg.values.push_back(val);

    val.key = RlmStateLst::FAIL_ANT.toStdString();
    val.value = rsModel->getRlmCmdValue(RlmStateLst::FAIL_ANT).toStdString();
    msg.values.push_back(val);

    val.key = RlmStateLst::FAIL_TRANSMITTER.toStdString();
    val.value = rsModel->getRlmCmdValue(RlmStateLst::FAIL_TRANSMITTER).toStdString();
    msg.values.push_back(val);

    val.key = RlmStateLst::FAIL_RECIEVER.toStdString();
    val.value = rsModel->getRlmCmdValue(RlmStateLst::FAIL_RECIEVER).toStdString();
    msg.values.push_back(val);

    val.key = RlmStateLst::FAIL_RLMSOURCE1.toStdString();
    val.value = rsModel->getRlmCmdValue(RlmStateLst::FAIL_RLMSOURCE1).toStdString();
    msg.values.push_back(val);

    val.key = RlmStateLst::FAIL_RLMSOURCE2.toStdString();
    val.value = rsModel->getRlmCmdValue(RlmStateLst::FAIL_RLMSOURCE2).toStdString();
    msg.values.push_back(val);

    val.key = RlmStateLst::FAIL_RLMTRANSMITTERSOURCE.toStdString();
    val.value = rsModel->getRlmCmdValue(RlmStateLst::FAIL_RLMTRANSMITTERSOURCE).toStdString();
    msg.values.push_back(val);

    val.key = RlmStateLst::FAIL_RLMOVERHEATING.toStdString();
    val.value = rsModel->getRlmCmdValue(RlmStateLst::FAIL_RLMOVERHEATING).toStdString();
    msg.values.push_back(val);

    val.key = RlmStateLst::FAIL_RLMTIN.toStdString();
    val.value = rsModel->getRlmCmdValue(RlmStateLst::FAIL_RLMTIN).toStdString();
    msg.values.push_back(val);

    diagnostic_pub.publish(msg);
}

void RobotThread::handleRpUdpDatagramm()
{
    Q_EMIT logMsg("handleRpUdpDatagramm");

    diagnostic_msgs::DiagnosticStatus msg;
    msg.level = 0;
    msg.name = "radar_settings_state";
    msg.hardware_id = "radar";

    QDateTime date = QDateTime::currentDateTime();
    msg.message = date.toString("hh-mm-ss").toStdString();

    RadarStationModel* rsModel = rsContr->getRSModel();

    diagnostic_msgs::KeyValue val;

    val.key = RpStateLst::RAIN.toStdString();
    val.value = rsModel->getRpCmdValue(RpStateLst::RAIN).toStdString();
    msg.values.push_back(val);

    val.key = RpStateLst::LFFILTERVALUE.toStdString();
    val.value = rsModel->getRpCmdValue(RpStateLst::LFFILTERVALUE).toStdString();
    msg.values.push_back(val);

    val.key = RpStateLst::SYNCINTERFERENCEMODE.toStdString();
    val.value = rsModel->getRpCmdValue(RpStateLst::SYNCINTERFERENCEMODE).toStdString();
    msg.values.push_back(val);

    val.key = RpStateLst::COG.toStdString();
    val.value = rsModel->getRpCmdValue(RpStateLst::COG).toStdString();
    msg.values.push_back(val);

    val.key = RpStateLst::SOG.toStdString();
    val.value = rsModel->getRpCmdValue(RpStateLst::SOG).toStdString();
    msg.values.push_back(val);

    val.key = RpStateLst::RADARLATINGRAD.toStdString();
    val.value = rsModel->getRpCmdValue(RpStateLst::RADARLATINGRAD).toStdString();
    msg.values.push_back(val);

    val.key = RpStateLst::RADARLONINGRAD.toStdString();
    val.value = rsModel->getRpCmdValue(RpStateLst::RADARLONINGRAD).toStdString();
    msg.values.push_back(val);

    val.key = RpStateLst::HDG.toStdString();
    val.value = rsModel->getRpCmdValue(RpStateLst::HDG).toStdString();
    msg.values.push_back(val);

    val.key = RpStateLst::SPD.toStdString();
    val.value = rsModel->getRpCmdValue(RpStateLst::SPD).toStdString();
    msg.values.push_back(val);

    val.key = RpStateLst::GAIN.toStdString();
    val.value = rsModel->getRpCmdValue(RpStateLst::GAIN).toStdString();
    msg.values.push_back(val);

    val.key = RpStateLst::SEA.toStdString();
    val.value = rsModel->getRpCmdValue(RpStateLst::SEA).toStdString();
    msg.values.push_back(val);

    val.key = RpStateLst::SEA_DIST.toStdString();
    val.value = rsModel->getRpCmdValue(RpStateLst::SEA_DIST).toStdString();
    msg.values.push_back(val);

    diagnostic_pub.publish(msg);
}

void RobotThread::handleVrliUdpDatagramm()
{
    radar_station_mr2pvr::RadarTarget msg;

    QByteArray datagram;
    datagram = rsContr->getRSModel()->getVrliRaw();

    //QDateTime date = QDateTime::currentDateTime();
    //QString msgt(date.toString("hh-mm-ss"));
    Q_EMIT logMsg("handleVrliUdpDatagramm");

    QDataStream stream(datagram);
    stream.setVersion(QDataStream::Qt_5_2);
    stream.setByteOrder(QDataStream::LittleEndian);

    TargetMsgFormat header;
    TargetForm target;
    TargetMsgCrcFormat crc;

    while(!stream.atEnd())
    {
        stream >> header;
        // прочитать цели:
        int targetsCount((header.sz - 20)/23); // 20б - размер хедера, 23б - 1 цель.
        for(int i = 0; i < targetsCount; ++i)
        {
            stream >> target;

            //targetLogMessage(header, target);
            char timeStamp[7];
            memcpy(timeStamp, header.timeStamp, 6);
            timeStamp[6] = 0;

            msg.num = target.number;
            msg.latitude = target.latitude;
            msg.longitude = target.longitude;
            msg.course = target.course;
            msg.velocity = target.speed;
            msg.state = target.status;

            vrli_pub.publish(msg);

            /*
            ui->vrliLog->append(
                QString("h = %1, sz = %2, time = %3 : "
                        "num = %4 , speed = %5 , status = %6 , course = %7"
                        )
                        .arg(header.header)
                        .arg(header.sz)
                        .arg(timeStamp)
                        .arg(target.number)
                        .arg(target.speed)
                        .arg(target.status)
                        .arg(target.course));
            */
            //QThread::msleep(5);
        }

        stream >> crc.checkSum1;
        stream >> crc.checkSum2;

        // проверить crc
        /*
        QByteArray datagram;
        QDataStream stream2(&datagram, QIODevice::WriteOnly);
        stream2 << header;
        stream2 << target;

        quint8 checkSum1 = FletchSum((const quint8*)datagram.constData(), datagram.size());
        stream2 << checkSum1;
        quint8 checkSum2 = FletchSum((const quint8*)datagram.constData(), datagram.size());
        */

    }
}

void RobotThread::handlePrliUdpDatagramm()
{
    radar_station_mr2pvr::RadarRawLine msg;

    QByteArray datagram;
    datagram = rsContr->getRSModel()->getPrliRaw();

    //QDateTime date = QDateTime::currentDateTime();
    //QString msgt(date.toString("hh-mm-ss"));
    Q_EMIT logMsg("handlePrliUdpDatagramm");
    //ui->prliLog->append(QString("received: %1 , sz: %2").arg(msg).arg(datagram.size()));

    /*
    if(ui->prliSavePackets->isChecked())
    {
        QDateTime date = QDateTime::currentDateTime();
        QString str = date.toString("prli_yyMMdd_hhmmss") + ".dat";

        QFile file(str);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            ui->prliLog->append("error open file: " + str);
            return;
        }

        file.write(datagram);
        ui->prliLog->append("binary saved in the: " + str);
    }

    if(!ui->prliParsePackets->isChecked())
    {
        return;
    }
    */

    // разобрать прли
    QDataStream stream(datagram);
    stream.setVersion(QDataStream::Qt_5_2);
    stream.setByteOrder(QDataStream::LittleEndian);

    PrimaryRadarInformationHeader header;
    PrimaryRadarInformationData data;

    QByteArray in;

    while(!stream.atEnd())
    {
        stream >> header;
        stream >> data;

        in.resize(data.sz - 14);
        stream.readRawData(in.data(), data.sz - 14);

        msg.latitude = header.latitude;
        msg.longitude = header.longitude;
        msg.line_num = data.number;
        msg.scale = data.range;
        msg.data.resize(in.size());
        memcpy(&msg.data[0], in.data(), in.size());
        prli_pub.publish(msg);

        //QByteArray out;
        //Algorithm2PolyBitLine::decompression(in, out);

        /*
        ui->prliLog->append(
            QString("h = %1, lon = %2, lat = %3 : "
                    "sz = %4 , num = %5 , range = %6 , t1 = %7, "
                    "out.sz = %8")
                    .arg(header.header)
                    .arg(header.longitude)
                    .arg(header.latitude)
                    .arg(data.sz - 14)
                    .arg(data.number)
                    .arg(data.range)
                    .arg(data.t1)
                    .arg(out.size()));
                    */

    }

}

void RobotThread::diagnosticVrliCallBack(const radar_station_mr2pvr::NavINS& msg)
{

}

bool RobotThread::rlmCtrlCmdHandler(radar_station_mr2pvr::RlmCtrlCmd::Request  &req,
             radar_station_mr2pvr::RlmCtrlCmd::Response& /*res*/)
{
    RadarStationModel* rsModel = rsContr->getRSModel();
    rsModel->writeRlmCmd(req.key.c_str(), req.value.c_str());

    ROS_INFO("request rlm service: key = %s, y= %s", req.key.c_str(), req.value.c_str());
    return true;
}

bool RobotThread::rpCtrlCmdHandler(radar_station_mr2pvr::RpCtrlCmd::Request  &req,
             radar_station_mr2pvr::RpCtrlCmd::Response& /*res*/)
{
    RadarStationModel* rsModel = rsContr->getRSModel();
    rsModel->writeRpCmd(req.key.c_str(), req.value.c_str());

    ROS_INFO("request rp service: key = %s, y= %s", req.key.c_str(), req.value.c_str());
    return true;
}

bool RobotThread::vrliCtrlCmdHandler(radar_station_mr2pvr::VrliCtrlCmd::Request  &req,
             radar_station_mr2pvr::VrliCtrlCmd::Response& /*res*/)
{
    RadarStationModel* rsModel = rsContr->getRSModel();
    rsModel->writeVrliCmd(req.key.c_str(), req.value.c_str());

    ROS_INFO("request vrli service: key = %s, y= %s", req.key.c_str(), req.value.c_str());
    return true;
}
