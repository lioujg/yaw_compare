#include <iostream>
#include <ros/ros.h>
#include <cmath>
#include <geometry_msgs/Pose2D.h>
#include <tf/transform_datatypes.h>
#include "std_msgs/String.h"

using namespace std;

#define PI 3.14159265

float stm32_yaw;
float ground_truth_yaw;

void stm_callback(geometry_msgs::Pose2D stm_data)
{
	stm32_yaw = stm_data.theta;
}

void optitrack_callback(const geometry_msgs::PoseStamped::ConstPtr& msg)
{
    geometry_msgs::PoseStamped optitrack_data;
    optitrack_data = *msg;
	double quaternion_w, quaternion_x, quaternion_y, quaternion_z;
	double payload_roll, payload_yaw, payload_pitch;
	quaternion_x = optitrack_data.pose.orientation.x;
	quaternion_y = optitrack_data.pose.orientation.y;
	quaternion_z = optitrack_data.pose.orientation.z;
	quaternion_w = optitrack_data.pose.orientation.w;
	tf::Quaternion quaternion(quaternion_x, quaternion_y, quaternion_z, quaternion_w);
	tf::Matrix3x3(quaternion).getRPY(payload_roll, payload_pitch, payload_yaw);
	ground_truth_yaw = payload_yaw;
}

int main(int argc ,char **argv){
	ros::init(argc,argv,"yaw_compare");
	ros::NodeHandle n;
	ros::Subscriber stm32_sub = n.subscribe("/stm32_payload_yaw", 1000, stm_callback);
	ros::Subscriber optitrack_sub = n.subscribe("/vrpn_client_node/payload/pose",1000, optitrack_callback);
	ros::Rate loop_rate(10);
	float error_percent = 0;

	while (ros::ok())
	{
		stm32_yaw = stm32_yaw * PI / 180;
		ground_truth_yaw = ground_truth_yaw * PI / 180;
		error_percent = abs((stm32_yaw - ground_truth_yaw) / ground_truth_yaw);
		cout << "stm32: " << stm32_yaw << "\t ground truth: " << ground_truth_yaw << "\t Error: " << error_percent << endl;
		ros::spinOnce();
		loop_rate.sleep();
	}

	return 0;
}
