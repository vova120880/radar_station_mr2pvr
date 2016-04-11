#include "ros/ros.h"
#include "radar_station_mr2pvr/RlmCtrlCmd.h"
#include "radar_station_mr2pvr/RpCtrlCmd.h"
#include "radar_station_mr2pvr/VrliCtrlCmd.h"
#include <cstdlib>
 
int main(int argc, char **argv)
{
  ros::init(argc, argv, "services_client");
  if (argc != 4)
  {
    ROS_INFO("usage: services_client T K V");
    return 1;
  }
 
  ros::NodeHandle n;

  if(!strcmp(argv[1],"rlm"))
  {
      ros::ServiceClient client = n.serviceClient<radar_station_mr2pvr::RlmCtrlCmd>("rlm_ctrl_cmd");
      radar_station_mr2pvr::RlmCtrlCmd srv;
      srv.request.key = argv[2];
      srv.request.value = argv[3];
      if (client.call(srv))
      {
        ROS_INFO("OK");
      }
      else
      {
        ROS_ERROR("Failed to call service rlm_ctrl_cmd");
        return 1;
      }
  }
  else if(!strcmp(argv[1],"rp"))
  {
      ros::ServiceClient client = n.serviceClient<radar_station_mr2pvr::RpCtrlCmd>("rp_ctrl_cmd");
      radar_station_mr2pvr::RpCtrlCmd srv;
      srv.request.key = argv[2];
      srv.request.value = argv[3];
      if (client.call(srv))
      {
        ROS_INFO("OK");
      }
      else
      {
        ROS_ERROR("Failed to call service rp_ctrl_cmd");
        return 1;
      }
  }
  else if(!strcmp(argv[1],"vrli"))
  {
      ros::ServiceClient client = n.serviceClient<radar_station_mr2pvr::VrliCtrlCmd>("vrli_ctrl_cmd");
      radar_station_mr2pvr::VrliCtrlCmd srv;
      srv.request.key = argv[2];
      srv.request.value = argv[3];
      if (client.call(srv))
      {
        ROS_INFO("OK");
      }
      else
      {
        ROS_ERROR("Failed to call service vrli_ctrl_cmd");
        return 1;
      }
  }

 
  return 0;
}
