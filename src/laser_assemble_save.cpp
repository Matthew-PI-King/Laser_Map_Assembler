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

#include<octomap_ros/conversions.h>

class laser_map_assembler
{
  public:
  laser_map_assembler(int n)
  :nh(), haveRecieved(false), r(50), timeOut(n)
  {
  std::cout<<"Initializing"<<std::endl;
  // Create a ROS subscriber for the input point cloud
  sub = nh.subscribe ("latest_pointcloud", 5, &laser_map_assembler::cloud_cb, this);

  std::cout<<"Complete Initialization"<<std::endl;
  };

  void run()
  {
	if(haveRecieved)
		{
			ros::Duration timesinceLast = (ros::Time::now()-lastRecieved);
			std::cout<<timesinceLast<<std::endl;
			if(timesinceLast > timeOut)
				{
					std::cout<<"Time since last pointcloud has exceeded timeout! Saving and exiting now!"<<std::endl;
					saveAndExit();
				}

		}
	ros::spinOnce();
	r.sleep();
  	return;
  };



  private:
  void cloud_cb (const sensor_msgs::PointCloud2ConstPtr& input)
  {	haveRecieved=true;
	lastRecieved=ros::Time::now();
	sensor_msgs::PointCloud2 temp;
	if(pcl::concatenatePointCloud(*input, globalPC, temp))
	{
		globalPC=temp;
	}

  };
	
  void saveAndExit()
  {
	pcl::PCLPointCloud2 output;
	pcl_conversions::toPCL(globalPC, output); //convert from sensor_msgs::PointCloud2 to pcl::PCLPointCloud2
	//pcl::PCDWriter writer();
	//writer.write("my_pcd.pcd", output);
	pcl::io::savePCDFile("my_pcd.pcd", output);
	std::cout<<"Finished saving pointclouds! Exiting now!"<<std::endl;
	ros::shutdown();

  };
	
  ros::NodeHandle nh;
  ros::Subscriber sub;
  ros::Time lastRecieved; 
  ros::Duration timeOut;
  ros::Rate r;
  bool haveRecieved;
  sensor_msgs::PointCloud2 globalPC; //pointcloud containing all points.  //way too big.

};

int main (int argc, char** argv)
{
  // Initialize ROS
  ros::init (argc, argv, "assemble_and_save_node");
  std::cout<<"Node booting!"<<std::endl;
  laser_map_assembler assem(30);  //Will wait 30 seconds after recieving final pointcloud before timing out and saving results.
  while(ros::ok())
  assem.run(); 

  return 0;
}
