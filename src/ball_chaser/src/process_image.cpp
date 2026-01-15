#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
    // TODO: Request a service and pass the velocities to it to drive the robot
    ROS_INFO("DriveToTarget Request received - Linear x: %f , angular z: %f",lin_x,ang_z);
    
    // Create service request
    ball_chaser::DriveToTarget srv;
    srv.request.linear_x  = lin_x;
    srv.request.angular_z = ang_z;
    
    // Call the service and check for success
    if (client.call(srv)) {
        ROS_INFO("Service response: %s", srv.response.msg_feedback.c_str());
    } else {
        ROS_ERROR("Failed to call service /ball_chaser/command_robot");
    }
    
    
}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{

    int white_pixel = 255;

    // TODO: Loop through each pixel in the image and check if there's a bright white one
    // Then, identify if this pixel falls in the left, mid, or right side of the image
    // Depending on the white ball position, call the drive_bot function and pass velocities to it
    // Request a stop when there's no white ball seen by the camera
 
    drive_robot(0.0, 0.0);
    
    
    for (size_t i = 0; i < img.data.size(); i += 3) {
	int red = img.data[i];
	int green = img.data[i + 1];
	int blue = img.data[i + 2];

	// Next check if you found the white color ball
	if (red == white_pixel && green == white_pixel && blue == white_pixel) {
	// Found a white pixel, which means finding the ball
	// Next step would be decide how to move based on the
	// calculated pixel position or coordinates in the image
		int pixel_index = i / 3;
		int x = pixel_index % img.width;

		if (x < img.width / 3) {
		    ROS_INFO("Driving Left");
		    drive_robot(0.0, 0.1);

		} else if (x < (2 * img.width) / 3) {
		    ROS_INFO("Driving Forward");
		    drive_robot(0.5, 0.0);

		} else {
		    ROS_INFO("Driving Right");
		    drive_robot(0.0, -0.1);
		}

		return;
	}
	}
    

    
    
}

int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    // Handle ROS communication events
    ros::spin();

    return 0;
}
