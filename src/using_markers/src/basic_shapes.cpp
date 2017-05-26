#include <ros/ros.h>
#include <visualization_msgs/Marker.h>
#include "using_markers/mouse_reader.h"
#include "std_msgs/String.h"
#include "std_msgs/Bool.h"


// ----- Mouse Handling ------ \\

//Importing basic function to handle the mouse (openMouse, findMouse, closeMouse, isReadable)

int Mouse::openMouse(const char *device_path)
{
  int fd = open(device_path, O_RDONLY);
  if(fd < 0)
  {
    ROS_ERROR("Failed to open \"%s\"\n", device_path);
    return -1;
  }
  char name[255];
  if(ioctl(fd, EVIOCGNAME(sizeof(name)), name) < 0)
  {
    ROS_ERROR("\"%s\": EVIOCGNAME failed.", device_path);
    close(fd);
    return -1;
  }
  std::ostringstream sstream;
  sstream << name;
  std::string name_as_string = sstream.str();
  int i;
  for (i=0; i < valid_substrings_.size(); i++)
  {
    std::size_t found = name_as_string.find( valid_substrings_[i] );
    if (found!=std::string::npos)
    {
       return fd;
    } 
  } 
  close(fd);
  return -1;
}

int Mouse::findMouse()
{
  glob_t gl;
  int num_event_dev = 0;
  if(glob("/dev/input/event*", GLOB_NOSORT, NULL, &gl) == 0)
  {
    // Get number of event files
    num_event_dev = gl.gl_pathc;
  }
  int i, r;
  for(i = 0; i < num_event_dev; i++)
  {
    r = openMouse(gl.gl_pathv[i]);

    if(r >= 0) return r;
  } 
  globfree(&gl);
  return -1;
} 


void Mouse::closeMouse()
{
  printf("Closing Mouse device.\n");
  close(descriptor_);
  return;
}


bool Mouse::isReadable ()
{
  if (descriptor_ < 0) return false;
  return true;
}



// Next, processEvent and spinMouse, here need to change the function to return a bool, not to be void
//this should retrun true based on the mouse 
bool Mouse::processEvent(struct input_event *ev, ros::Publisher& ros_publisher)
{

  bool pressed_left_;

  switch(ev->type)
  {
    case EV_SYN:				
      break; 

    case EV_MSC:				
      break;

    case EV_REL:			
      break;

    case EV_KEY:				
      if(ev->code == BTN_LEFT)
      {
	pressed_left_ = (bool)ev->value;
      }
      break;

    default:				
      ROS_WARN("Unexpected event type; ev->type = 0x%04x\n", ev->type);
  } 
  if (pressed_left_ == true) return true; 

  fflush(stdout);
} 


//OK so this probably needs some cleaning but it does the job for now :D it gets true from ProcessEvnt and passes it to Main ()
bool Mouse::spinMouse(ros::Publisher& ros_publisher)
{
  int const BUFFER_SIZE = 32;

  bool chp;
  
  struct input_event ibuffer[BUFFER_SIZE];
  int r, events, i;
 
    r = read(descriptor_, ibuffer, sizeof(struct input_event) * BUFFER_SIZE);
    if( r > 0 )
    {
      events = r / sizeof(struct input_event);
      for(i = 0; i < events; i++)
      {
	chp=processEvent(&ibuffer[i], ros_publisher);
      } 
    } 
  if (chp == true) return true; 
} 



// the Main ()

int main( int argc, char** argv )
{
  ros::init(argc, argv, "basic_shapes");
  ros::NodeHandle n;
  ros::Rate r(1);
  ros::Publisher marker_pub = n.advertise<visualization_msgs::Marker>("visualization_marker", 1);

  //The Mouse part
  std::string mouse_path;
  n.param<std::string>("path", mouse_path, "");
  //Mouse object 
  Mouse mouse(mouse_path);
  
  bool trig; //that's the trigger variable, when true publish the marker


  //now the markers
  uint32_t shape = visualization_msgs::Marker::CUBE;

  float cntr=0.01; //variable to change the marker size

  while (ros::ok())
  {
    visualization_msgs::Marker marker;
    marker.header.frame_id = "/my_frame";
    marker.header.stamp = ros::Time::now();

    //trig would be true if mouse button clicked
    trig=mouse.spinMouse(marker_pub);


    marker.ns = "basic_shapes";
    marker.id = 0;

    //type
    marker.type = shape;

    //action
    marker.action = visualization_msgs::Marker::ADD;

    //pose
    marker.pose.position.x = 0;
    marker.pose.position.y = 0;
    marker.pose.position.z = 0;
    marker.pose.orientation.x = 0.0;
    marker.pose.orientation.y = 0.0;
    marker.pose.orientation.z = 0.0;
    marker.pose.orientation.w = 1.0;

    //sizze
    marker.scale.x = cntr;
    marker.scale.y = cntr;
    marker.scale.z = cntr;
    cntr=cntr+.05;
   

    //colour 
    marker.color.r = 0.0f;
    marker.color.g = 1.0f;
    marker.color.b = 0.0f;
    marker.color.a = 1.0;

     marker.lifetime = ros::Duration();

    while (marker_pub.getNumSubscribers() < 1)
    {
      if (!ros::ok())
      {
        return 0;
      }
      ROS_WARN_ONCE("Please create a subscriber to the marker");
      sleep(1);
    }

    if (trig == true) marker_pub.publish(marker);

    switch (shape)
    {
    case visualization_msgs::Marker::CUBE:
      shape = visualization_msgs::Marker::SPHERE;
      break;
    case visualization_msgs::Marker::SPHERE:
      shape = visualization_msgs::Marker::ARROW;
      break;
    case visualization_msgs::Marker::ARROW:
      shape = visualization_msgs::Marker::CYLINDER;
      break;
    case visualization_msgs::Marker::CYLINDER:
      shape = visualization_msgs::Marker::CUBE;
      break;
    }


  }
}
