/*
*
*
*
*
*
*/

#include<ros/ros.h>
#include<tf/transform_listener.h>
#include<sensor_msgs/PointCloud2.h>

#include<pcl_conversions/pcl_conversions.h>
#include<pcl_ros/transforms.h>
#include<pcl/io/pcd_io.h>


class transformer
{
  public:
  transformer(int n)
  :nh(), tf_listener(ros::Duration(n))
  {
  std::cout<<"Initializing"<<std::endl;
  // Create a ROS subscriber for the input point cloud
  sub = nh.subscribe ("input", 5, &transformer::cloud_cb, this);

  // Create a ROS publisher for the output point cloud
  pub = nh.advertise<sensor_msgs::PointCloud2> ("latest_pointcloud", 5);
  //pubG= nh.advertise<sensor_msgs::PointCloud2> ("global_pointcloud", 2);
  std::cout<<"Complete Initialization"<<std::endl;
  };

  private:
  void cloud_cb (const sensor_msgs::PointCloud2ConstPtr& input)
  {
    // Create a container for the data.
    sensor_msgs::PointCloud2 output;
    //if(!tf_listener.waitForTransform(input->header.frame_id, "map", input->header.stamp , ros::Duration(10)))
	//	return;

    if(pcl_ros::transformPointCloud("map", *input, output, tf_listener))
    {
	//output.header.stamp=ros::Time::now();  //yji
	pub.publish (output);
	//sensor_msgs::PointCloud2 temp;
	//if(pcl::concatenatePointCloud(output, globalPC, temp))
	//{
	//	globalPC=temp;
	//	pubG.publish(globalPC);
	//}
    }
    return;
  };
	
  ros::NodeHandle nh;
  ros::Subscriber sub;
  ros::Publisher pub;
  ros::Publisher pubG;
  tf::TransformListener tf_listener;
  //sensor_msgs::PointCloud2 globalPC; //pointcloud containing all points.  //way too big.

};

int main (int argc, char** argv)
{
  // Initialize ROS
  ros::init (argc, argv, "laser_map_assembler_node");
  std::cout<<"Node booting!"<<std::endl;
  transformer trans(30);

  // Spin
  ros::spin ();
}
