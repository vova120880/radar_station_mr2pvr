cmake_minimum_required(VERSION 2.8.3)
project(radar_station_mr2pvr)

SET (CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  rospy
  std_msgs
  message_generation
)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5Network REQUIRED)

## Generate messages in the 'msg' folder
add_message_files(
  FILES
  RadarRawLine.msg
  RadarTarget.msg
  NavINS.msg
)

## Generate services in the 'srv' folder
add_service_files(
   FILES
   RpCtrlCmd.srv
   RlmCtrlCmd.srv
   VrliCtrlCmd.srv
)

## Generate added messages and services with any dependencies listed here
generate_messages(
  DEPENDENCIES
  std_msgs
)

catkin_package(
#  INCLUDE_DIRS include
#  LIBRARIES radar_station_mr2pvr
  CATKIN_DEPENDS roscpp rospy std_msgs message_runtime
  DEPENDS system-lib
)

#qt5_add_resources(QT_RESOURCES_CPP qt_resources/arrows.qrc)
qt5_wrap_cpp(
  QT_MOC_HPP
  src/ControlWindow.h
  src/RobotThread.h
  src/rscontroller/radarstationcontroller.h
  src/rscontroller/rsmodel/radarstationmodel.h
  src/rscontroller/rsprotocols/radarstationprotocols.h
  src/rscontroller/rssockets/tcpsocket.h
  src/rscontroller/rssockets/udpsocket.h
  src/rscontroller/rsworkers/rsprotocolworker.h
)

# include_directories(include)
include_directories(
  ${catkin_INCLUDE_DIRS}
)
include_directories(
  ${Qt5Widgets_INCLUDE_DIRS}
  ${Qt5Network_INCLUDE_DIRS}
)

add_executable(
  radar_station_mr2pvr_node
  src/ControlWindow.cpp
  src/RobotThread.cpp
  src/main.cpp
  src/rscontroller/radarstationcontroller.cpp
  src/rscontroller/rsmodel/radarstationmodel.cpp
  src/rscontroller/rsprotocols/radarstationprotocols.cpp
  src/rscontroller/rssockets/tcpsocket.cpp
  src/rscontroller/rssockets/udpsocket.cpp
  src/rscontroller/rsworkers/rsprotocolworker.cpp
  ${QT_RESOURCES_CPP}
  ${QT_MOC_HPP}
)

target_link_libraries(
   radar_station_mr2pvr_node ${QT_LIBRARIES}
   ${catkin_LIBRARIES} Qt5::Widgets Qt5::Network
)

install(TARGETS radar_station_mr2pvr_node
  RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)

# %Tag(SRVCLIENT)%
add_executable(services_client src/services_client.cpp)
target_link_libraries(services_client ${catkin_LIBRARIES})
add_dependencies(services_client radar_station_mr2pvr_gencpp)
# %EndTag(SRVCLIENT)%
